#!/usr/bin/env bash
# MINISHELL TESTER COMPLET (AVEC HTML & VALGRIND)
# Couverture étendue : partie obligatoire du sujet (dont heredoc <<). Les bonus sujet
# (&& || parenthèses, wildcard *) ne sont ajoutés que si INCLUDE_BONUS_TESTS=1.
#
# Pièges volontaires (souvent KO sur des minishells “minimalistes”) :
#   - echo -n hello : POSIX autorise -n ; beaucoup de 42 ne gèrent qu’un seul -n exact.
#   - ligne seule "> fichier" puis utilisation : bash crée le fichier vide ; pas toujours le cas.
#   - redir avant mot : "> out cmd" ; ordre des mots du sujet.
#   - export VAR sans '=' puis commande externe : risque de bug mémoire selon implémentations.
#
# Options :
#   INCLUDE_BONUS_TESTS=1      — ajoute &&, || et * (bonus sujet uniquement).
#   SKIP_VALGRIND=1            — rapide : pas de valgrind (pas de vérif fuites).
#
# Chaque cas s’exécute dans un répertoire isolé ; bash --posix et LC_ALL=C pour comparaison stable.
#
# Hypothèses de comparaison (à connaître pour interpréter les KO) :
#   - bash est lancé en non-interactif : pas de prompt sur stdout/stderr comme en interactif.
#   - Valgrind FD : seuls les descripteurs > 2 encore ouverts à la fin sont comptés en échec
#     (stdin/stdout/stderr peuvent apparaître dans le log sans provoquer FD:KO).
#   - normalise la sortie minishell : stdout (suffixe prompt si echo -n) ; stdout et stderr :
#     lignes = script d’entrée et « exit » EOF retirés (réécho readline → évite stderr « faux »).
#     Pas de suffixe prompt sur stderr (lignes vides résiduelles → faux KO sur présence stderr).
set -u

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

INCLUDE_BONUS_TESTS="${INCLUDE_BONUS_TESTS:-0}"
SKIP_VALGRIND="${SKIP_VALGRIND:-0}"

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 /chemin/vers/minishell"
    echo "  INCLUDE_BONUS_TESTS=1 $0 ...  — ajoute les tests bonus (&&, ||, *)."
    echo "  SKIP_VALGRIND=1 $0 ...       — exécution rapide sans contrôle valgrind."
    exit 1
fi
MINISHELL_INPUT="$1"

if [[ ! -x "$MINISHELL_INPUT" ]]; then
    echo "Erreur: '$MINISHELL_INPUT' n'est pas exécutable."
    exit 1
fi

MINISHELL="$(cd "$(dirname "$MINISHELL_INPUT")" && pwd -P)/$(basename "$MINISHELL_INPUT")"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPORT_FILE="$SCRIPT_DIR/minishell_report_complet.html"
TMP_ROOT="$(mktemp -d /tmp/ms_complet_XXXXXX)"

TOTAL=0; PASS=0; FAIL=0; MEM_KO=0; FD_KO=0;

cleanup() { rm -rf "$TMP_ROOT"; }
trap cleanup EXIT

html_escape() { sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g'; }

# Retire lignes = script d'entrée (écho readline) et ligne « exit » finale (EOF), stdout ou stderr ms.
ms_strip_echoed_input_lines() {
    local file="$1" infile="$2"
    awk -v infile="$infile" '
    BEGIN {
        while ((getline line < infile) > 0) {
            if (line != "") inputs[line]++
        }
    }
    {
        if ($0 in inputs && inputs[$0] > 0) {
            inputs[$0]--
            next
        }
        lines[++n] = $0
    }
        END {
            if (n > 0 && lines[n] == "exit") n--
            while (n > 1 && lines[n] == "") n--
            for (i=1; i<=n; i++) print lines[i]
        }' "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
}

# do_awk=1 : stdout minishell (suffixes prompt + strip lignes entrée + exit)
# do_awk=0 : stderr minishell — strip des mêmes ghosts readline/exit (sans suffixe fin de ligne)
normalize_output() {
    local file="$1"
    local is_ms="${2:-0}"
    local input_file="${3:-}"
    local do_awk="${4:-1}"

    sed -i -E 's/\x1b\[[0-9;?]*[ -/]*[@-~]//g' "$file"
    sed -i 's/\r//g; s/\x00//g' "$file"
    # Lignes Valgrind parfois mélangées au fd 2 du processus instrumenté
    if [[ "$is_ms" == "1" ]]; then
        sed -i -E '/^==[0-9]+==/d' "$file"
    fi
    sed -i -E 's/^bash: line [0-9]+: //g; s/^bash: //g; s/^minishell: //g' "$file"

    if [[ "$is_ms" == "1" && -f "$input_file" ]]; then
        sed -i -E 's/^(minishell\$ |minishell > |minishell\$|> )+//g' "$file"
        # Uniquement stdout (do_awk=1) : évite de vider le stderr et casser la comparaison -s ms.err
        if [[ "$do_awk" == "1" ]]; then
            sed -i -E 's/minishell\$[[:space:]]*$//g' "$file"
            sed -i -E 's/minishell >[[:space:]]*$//g' "$file"
        fi
    fi

    if [[ "$is_ms" == "1" && -f "$input_file" ]]; then
        ms_strip_echoed_input_lines "$file" "$input_file"
    fi

    if [[ "$is_ms" == "1" && "$do_awk" == "0" ]]; then
        sed -i '/^[[:space:]]*$/d' "$file"
    fi
}

# bash « echo -n » n'a pas de \n final ; awk reconstruit avec print → un NL en trop. Aligner si ms == ref+NL.
ms_align_trailing_nl_with_ref() {
    local msf="$1" ref="$2"
    [[ ! -f "$ref" || ! -f "$msf" ]] && return 0
    cmp -s "$msf" "$ref" && return 0
    local rc mc
    rc=$(tail -c1 "$ref" 2>/dev/null | od -An -tx1 | tr -d ' \n' || true)
    mc=$(tail -c1 "$msf" 2>/dev/null | od -An -tx1 | tr -d ' \n' || true)
    [[ "$rc" == "0a" ]] && return 0
    [[ "$mc" != "0a" ]] && return 0
    local tmp
    tmp=$(mktemp)
    if head -c -1 "$msf" > "$tmp" 2>/dev/null && cmp -s "$tmp" "$ref"; then
        mv "$tmp" "$msf"
    else
        rm -f "$tmp"
    fi
}

check_valgrind() {
    local log="$1"
    local mem_ok=1 fd_ok=1

    if grep -qE "(definitely|indirectly|possibly) lost: [1-9]" "$log"; then
        mem_ok=0
    fi
    local fd_leaks
    fd_leaks=$(awk '/Open file descriptor/{ if ($5+0 > 2 && $0 !~ /valgrind.log/) print $0 }' "$log")
    [[ -n "$fd_leaks" ]] && fd_ok=0
    echo "$mem_ok $fd_ok"
}

cat > "$REPORT_FILE" <<EOF
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<title>Rapport Minishell Complet</title>
<style>
body { font-family: Arial, sans-serif; background:#f6f7fb; color:#222; margin:0; padding:24px; }
.container { max-width:1100px; margin:0 auto; }
.card { background:#fff; border-radius:14px; padding:18px 20px; margin-bottom:16px; box-shadow:0 2px 12px rgba(0,0,0,0.08); }
.status { display:inline-block; padding:6px 10px; border-radius:999px; font-weight:700; color:#fff; font-size:14px; }
.ok { background:#1f9d55; }
.ko { background:#d64545; }
.summary { display:grid; grid-template-columns: repeat(auto-fill, minmax(110px, 1fr)); gap:10px; margin-top:12px; }
.box { padding:12px; border-radius:12px; color:#fff; font-weight:700; text-align:center; font-size:13px; }
.box.total { background:#4a5568; }
.box.ok { background:#1f9d55; }
.box.ko { background:#d64545; }
.pill { display:inline-block; padding:4px 10px; border-radius:999px; color:#fff; font-weight:700; font-size:12px; margin:2px; }
.substatus { display:flex; flex-wrap:wrap; gap:6px; margin:8px 0 12px; }
pre { background:#111827; color:#e5e7eb; padding:12px; border-radius:10px; overflow:auto; white-space:pre-wrap; word-wrap:break-word; }
.small { color:#555; font-size:14px; }
.grid { display:grid; grid-template-columns:1fr 1fr; gap:12px; }
.code-title { font-size:13px; color:#666; margin-bottom:6px; font-weight:700; }
</style>
</head>
<body>
<div class="container">
<div class="card">
<h1>Rapport de test Minishell (Complet)</h1>
<p class="small"><strong>Binaire :</strong> $(printf '%s' "$MINISHELL" | html_escape)</p>
<p class="small"><strong>Date :</strong> $(date '+%Y-%m-%d %H:%M:%S')</p>
<p class="small"><strong>Locale :</strong> LC_ALL=C (comparaison avec bash --posix)</p>
<p class="small"><strong>Bonus :</strong> __BONUSLINE__</p>
<div class="summary">
<div class="box total">Tests<br>__TOTAL__</div>
<div class="box ok">OK<br>__PASS__</div>
<div class="box ko">KO (Fonctionnel)<br>__FAIL__</div>
<div class="box ko">Leaks Mémoire<br>__MEM_KO__</div>
<div class="box ko">Leaks FD<br>__FD_KO__</div>
</div>
</div>
EOF

run_test() {
    local cmd="$1"
    local desc="${2:-$cmd}"
    local tdir="$TMP_ROOT/test_$TOTAL"
    mkdir -p "$tdir/wd"
    echo -e "$cmd" > "$tdir/input.sh"

    ( cd "$tdir/wd" || exit 1; env LC_ALL=C LANG=C bash --posix < "$tdir/input.sh" > "$tdir/sh.out" 2> "$tdir/sh.err"; echo "$?" > "$tdir/sh.code" ) || true

    if [[ "${SKIP_VALGRIND}" == "1" ]]; then
        ( cd "$tdir/wd" || exit 1; env LC_ALL=C LANG=C "$MINISHELL" < "$tdir/input.sh" > "$tdir/ms.out" 2> "$tdir/ms.err"; echo "$?" > "$tdir/ms.code" ) || true
        echo "" > "$tdir/vg.log"
    else
        ( cd "$tdir/wd" || exit 1; env LC_ALL=C LANG=C valgrind --leak-check=full --track-fds=yes --log-file="$tdir/vg.log" "$MINISHELL" < "$tdir/input.sh" > "$tdir/ms.out" 2> "$tdir/ms.err"; echo "$?" > "$tdir/ms.code" ) || true
    fi

    normalize_output "$tdir/sh.out" "0" "$tdir/input.sh" "1"
    normalize_output "$tdir/ms.out" "1" "$tdir/input.sh" "1"
    ms_align_trailing_nl_with_ref "$tdir/ms.out" "$tdir/sh.out"
    normalize_output "$tdir/sh.err" "0" "$tdir/input.sh" "0"
    normalize_output "$tdir/ms.err" "1" "$tdir/input.sh" "0"

    local out_ok=1 err_ok=1 rc_ok=1
    cmp -s "$tdir/ms.out" "$tdir/sh.out" || out_ok=0
    [[ "$(cat "$tdir/sh.code")" == "$(cat "$tdir/ms.code")" ]] || rc_ok=0

    local sh_has_err=0 ms_has_err=0
    [[ -s "$tdir/sh.err" ]] && sh_has_err=1
    [[ -s "$tdir/ms.err" ]] && ms_has_err=1
    [[ "$sh_has_err" == "$ms_has_err" ]] || err_ok=0

    local vg_res
    if [[ "${SKIP_VALGRIND}" == "1" ]]; then
        vg_res="1 1"
    else
        vg_res=$(check_valgrind "$tdir/vg.log")
    fi
    local mem_ok=${vg_res% *}
    local fd_ok=${vg_res#* }

    local func_status="KO" mem_status="KO" fd_status="KO" badge="ko"
    [[ $out_ok -eq 1 && $err_ok -eq 1 && $rc_ok -eq 1 ]] && func_status="OK"
    [[ $mem_ok -eq 1 ]] && mem_status="OK"
    [[ $fd_ok -eq 1 ]] && fd_status="OK"

    if [[ "$func_status" == "OK" && "$mem_status" == "OK" && "$fd_status" == "OK" ]]; then
        badge="ok"; ((PASS++))
    else
        ((FAIL++))
    fi
    [[ $mem_ok -eq 0 ]] && ((MEM_KO++))
    [[ $fd_ok -eq 0 ]] && ((FD_KO++))
    ((TOTAL++))

    {
        echo '<div class="card">'
        printf '<div class="status %s">%s</div>\n' "$badge" "$([[ "$badge" == "ok" ]] && echo "OK" || echo "KO")"
        printf '<h2>%s</h2>\n' "$(printf '%s' "$desc" | html_escape)"
        echo '<div class="substatus">'
        printf '<span class="pill %s">Fonctionnel: %s</span>\n' "$([[ "$func_status" == "OK" ]] && echo ok || echo ko)" "$func_status"
        printf '<span class="pill %s">Mémoire: %s</span>\n' "$([[ "$mem_status" == "OK" ]] && echo ok || echo ko)" "$mem_status"
        printf '<span class="pill %s">FD: %s</span>\n' "$([[ "$fd_status" == "OK" ]] && echo ok || echo ko)" "$fd_status"
        echo '</div>'
        if [[ "$func_status" == "KO" ]]; then
            echo '<p class="small" style="margin:0 0 8px 0;"><strong>Détail fonctionnel :</strong> '
            [[ $out_ok -eq 1 ]] && echo -n 'stdout identique · ' || echo -n '<span style="color:#f87171">stdout différent</span> · '
            [[ $rc_ok -eq 1 ]] && echo -n 'code retour identique · ' || echo -n '<span style="color:#f87171">code retour différent</span> · '
            [[ $err_ok -eq 1 ]] && echo -n 'stderr (vide/non vide) aligné bash' || echo -n '<span style="color:#f87171">stderr : bash vide et minishell non (ou l’inverse) — voir ci-dessous</span>'
            echo '</p>'
        fi

        echo '<div class="code-title">Commande envoyée</div><pre>'
        html_escape < "$tdir/input.sh"
        echo '</pre>'

        echo '<div class="grid">'
        echo '<div><div class="code-title">STDOUT minishell</div><pre>'
        html_escape < "$tdir/ms.out"
        echo '</pre></div>'

        echo '<div><div class="code-title">STDOUT bash</div><pre>'
        html_escape < "$tdir/sh.out"
        echo '</pre></div>'

        if [[ $err_ok -eq 0 || $sh_has_err -eq 1 || $ms_has_err -eq 1 ]]; then
            echo '<div><div class="code-title">STDERR minishell</div><pre>'
            html_escape < "$tdir/ms.err"
            echo '</pre></div>'
            echo '<div><div class="code-title">STDERR bash</div><pre>'
            html_escape < "$tdir/sh.err"
            echo '</pre></div>'
        fi
        echo '</div>'

        if [[ "$badge" == "ko" ]]; then
            echo '<div class="code-title" style="margin-top:10px;">Logs Valgrind (extraits)</div><pre style="color:red;">'
            if [[ "${SKIP_VALGRIND}" == "1" ]]; then
                echo '<span class="small">Valgrind non exécuté (SKIP_VALGRIND=1).</span>'
            else
                grep -E "(lost:|Invalid|uninitialised)" "$tdir/vg.log" 2>/dev/null | grep -v 'valgrind.log' | html_escape || true
                fd_ko_lines=$(awk '/Open file descriptor/{ if ($5+0 > 2 && $0 !~ /valgrind.log/) print $0 }' "$tdir/vg.log" 2>/dev/null || true)
                if [[ -n "$fd_ko_lines" ]]; then
                    echo ''
                    echo '<span class="small"><strong>FD &gt; 2</strong> encore ouverts (c’est ce qui fait échouer « FD » dans ce testeur, pas les lignes 0/1/2 ci-dessous).</span>'
                    printf '%s\n' "$fd_ko_lines" | html_escape
                fi
                fd_std_lines=$(grep -E 'Open file descriptor (0|1|2):' "$tdir/vg.log" 2>/dev/null | grep -v 'valgrind.log' || true)
                if [[ -n "$fd_std_lines" ]]; then
                    echo ''
                    echo '<span class="small"><strong>FD 0, 1 ou 2</strong> : informatif — le script ignore ces descripteurs pour le badge FD.</span>'
                    printf '%s\n' "$fd_std_lines" | html_escape
                fi
                if [[ -z "$(grep -E '(lost:|Invalid|uninitialised|Open file descriptor)' "$tdir/vg.log" 2>/dev/null)" ]]; then
                    echo 'Aucune ligne Valgrind capturée (log vide ou filtre).'
                fi
            fi
            echo '</pre>'
        fi
        echo '</div>'
    } >> "$REPORT_FILE.cards"

    if [[ "$badge" == "ok" ]]; then
        echo -e "${GREEN}[OK]${NC} $desc"
    else
        echo -e "${RED}[KO]${NC} $desc (Func:$func_status Mem:$mem_status FD:$fd_status)"
    fi
}

echo -e "${YELLOW}=== Erreurs de syntaxe / parsing ===${NC}"

run_test "echo > >" "Syntaxe: double chevron sortie"
run_test "< <" "Syntaxe: double redirection entrée"
run_test "| |" "Syntaxe: double pipe"
run_test "ls | " "Syntaxe: pipe en fin de ligne"
run_test "| ls" "Syntaxe: pipe en début de ligne"
run_test "echo '" "Syntaxe: quote simple non fermée"
run_test "echo \"" "Syntaxe: quote double non fermée"
run_test "echo hi >" "Syntaxe: > sans fichier"
run_test "echo hi <" "Syntaxe: < sans fichier"
run_test "echo hi >>" "Syntaxe: >> sans fichier"
run_test "echo hi|" "Syntaxe: pipe attaché au mot (fin ligne)"
run_test ">out_nospace\ncat out_nospace" "Syntaxe: > collé au fichier sans espace"
run_test "echo p|echo q" "pipe serré sans espaces"

echo -e "${YELLOW}=== echo / quotes / expansion ===${NC}"

run_test "echo hello world" "echo: mots multiples"
run_test "echo" "echo: sans argument"
run_test "echo -n" "echo: -n seul"
run_test "echo -n hello" "echo: -n basique"
run_test "echo \"'chapoux'\"" "doubles quotes contenant simples quotes"
run_test "echo '\"\$USER\"'" "simples quotes — pas d'expansion"
run_test "echo \$?\$?" "?\$/\$? collés (après true implicite — 00)"
run_test "/bin/true\nfalse\necho \$?" "?\$ après false (1)"
run_test "echo \"\"" "echo: chaîne vide"
run_test "echo \$UNSET_VAR_XYZ" "expansion variable inexistante"
run_test "export ZZ_MS=42\necho \$ZZ_MS" "expansion variable exportée"
run_test "export ZZ_MS=\"  spaced  \"\necho \$ZZ_MS" "export: espaces dans la valeur"
run_test "export ZZ_U=_\necho \$ZZ_U" "export: valeur seulement underscore"

echo -e "${YELLOW}=== pipes ===${NC}"

run_test "ls | cat" "pipe: ls vers cat"
run_test "echo hello | cat | wc -c" "pipe: trois maillons"
run_test "cat /dev/null | wc -c" "pipe: stdin vide"
run_test "echo a | echo b | echo c" "pipe: echo mange stdin (ordre sortie)"
run_test "echo x | cat | /bin/cat" "pipe: triple maillon (sans printf — parseur quotes)"
run_test "ls | exit" "pipe: exit en fin (code retour pipeline)"

echo -e "${YELLOW}=== redirections ===${NC}"

run_test "echo one > r_out\ncat r_out" "> création / écrasement"
run_test "echo a > r_app\necho b >> r_app\ncat r_app" ">> append"
run_test "> r_pre echo pre_redir" "> avant la commande (word order)"
run_test "echo ignored > r_only > r_last\ncat r_last" "double > même commande (dernier gagne)"
run_test "cat < /dev/null" "< depuis fichier vide"
run_test "cat < r_missing_404" "< fichier absent"
run_test "echo zot > r_stdout\n/bin/cat r_stdout" "> vers fichier puis lecture"

echo -e "${YELLOW}=== builtins env / cd / pwd ===${NC}"

run_test "export MSKV=bonjour\nenv | grep '^MSKV='" "env: variable avec valeur visible"
run_test "export MSKV_EMPTY=\nenv | grep '^MSKV_EMPTY='\n/bin/true" "env: variable avec valeur vide (VAR=)"
run_test "unset MSKV_TEST\nexport MSKV_TEST=1\nunset MSKV_TEST\nenv | grep '^MSKV_TEST='\necho done\n/bin/true" "unset supprime la variable (stdout: done seul)"
run_test "export MS_A=1\nexport MS_B=2\nenv | grep '^MS_' | sort" "export multiples + env"
run_test "export 1=2" "export: identifiant invalide (commence par chiffre)"
run_test "export =" "export: '=' seul"
run_test "export _valid_key=ok\necho \$_valid_key" "export: clé avec underscore"
run_test "cd ..\npwd" "cd: parent (nécessite sous-répertoire)"
run_test "mkdir -p sub_cd\ncd sub_cd\npwd" "cd: sous-dossier créé dans le cas"
run_test "mkdir -p sub_cd\ncd sub_cd\ncd ..\nrmdir sub_cd\necho up" "cd: retour arrière dossier"
run_test "cd /tmp\npwd" "cd: chemin absolu /tmp"
run_test "cd /nope_dir_ms_42_xyz" "cd: dossier inexistant"
run_test "cd a b" "cd: trop d'arguments"
run_test "cd" "cd: sans arg (HOME)"

echo -e "${YELLOW}=== commandes inexistantes / permissions ===${NC}"

run_test "idontexist_ms_xyz_123" "commande absente (PATH)"
run_test "idontexist > r_x" "cmd absente avec > (fichier créé ou non selon ordre d'exé — piège)"
run_test "/bin/ls /nope_path_perm" "ls: chemin invalide"

echo -e "${YELLOW}=== exécution / chemins ===${NC}"

run_test "printf '%s\\n' '#!/bin/sh' 'echo exec_ok' > xc.sh\nchmod +x xc.sh\n./xc.sh" "script executable ./ relatif (shebang fichier multi-lignes)"
run_test "printf '%s\\n' '#!/bin/sh' 'echo x' > xc2.sh\nchmod -x xc2.sh\n./xc2.sh" "./ sans +x (erreur)"
run_test "export PATH=\n/bin/echo path_bypass" "PATH vide via export PATH="

echo -e "${YELLOW}=== exit ===${NC}"

run_test "exit" "exit: sans code (0 si dernier status 0)"
run_test "exit 0" "exit: 0 explicite"
run_test "exit 42" "exit: code 42"
run_test "exit 257" "exit: modulo 256 attendu (1)"
run_test "false\nexit 1 2\necho survived\n/bin/true" "exit: trop d'arguments — le shell continue (stdout survived)"
run_test "exit abc" "exit: argument non numérique (code 2)"

echo -e "${YELLOW}=== intégration / ordre / cas limites ===${NC}"

run_test "echo bon > out_mix\n/bin/cat out_mix\necho jour >> out_mix\n/bin/cat out_mix" "sequence > puis >> sur meme fichier"
run_test "pwd\npwd" "pwd appele deux fois"
run_test "/bin/sh -c '/bin/echo shell_sub'" "invocation sh -c (externe)"
run_test "echo hola > /dev/null\n/bin/echo next" "redir vers /dev/null"
run_test "export DUPE=1\nexport DUPE=2\nenv | grep '^DUPE='" "re-export avec nouvelle valeur"
run_test "echo '\$'" "dollar litteral en quotes simples"
run_test "echo (" "Syntaxe: parenthese ouverte seule"

run_test "> onlytouch\nwc -c < onlytouch" "redir seule: fichier vide (0 octets)"
run_test "export E1=a\nunset E1\nexport E1=\nenv | grep '^E1='\necho after\n/bin/true" "unset puis export avec valeur vide"
run_test "/bin/false\necho \$?" "code retour après échec builtin externe"
run_test "true\necho \$?" "code retour apres succes (0)"
run_test "echo hi>g_nospace\ncat g_nospace" "> colle au mot precedent (sans espace avant fichier)"
run_test "echo \$HOME" "expansion HOME (si defini dans lenv import)"
run_test "cd ~\npwd" "cd tilde vers HOME"
run_test "export VAR_EQ=a=b=c\necho \$VAR_EQ" "export valeur contenant signe ="
run_test $'/bin/echo hello\tworld' "separateur tab entre arguments echo externe"
run_test "ls  |  cat" "pipe avec espaces multiples"
run_test "echo hello | export ZX_PIPE=1\nenv | grep '^ZX_PIPE='\n/bin/true" "export dans pipeline (souvent ignore cote parent)"
run_test "nope_cmd_absent_99" "commande inexistante simple (stderr)"
run_test "unset UNSET_NOPE_42\n/bin/true" "unset variable inexistante (pas derreur)"

run_test "echo | /bin/cat" "pipe: echo stdin vide vers cat"
run_test "/bin/echo -n\n/bin/true" "echo externe -n newline (externe pas builtin)"

echo -e "${YELLOW}=== heredoc (<<) — obligatoire sujet ===${NC}"

run_test "cat << EOF\nbonjour\nEOF" "heredoc: basique"
run_test "cat << \"EOF\"\n\$USER\nEOF" "heredoc: délimiteur entre guillemets doubles — pas d'expansion"
run_test "export HQ=must_not_expand\ncat << 'EOF'\n\$HQ\nEOF" "heredoc: délimiteur entre quotes simples — pas d'expansion (\$HQ littéral)"
run_test "export MS_HD_VAL=expanded_value\ncat << END\n\$MS_HD_VAL\nEND" "heredoc: délimiteur non quoté — expansion variable"
run_test "/bin/false\ncat << X\n\$?\nX" "heredoc: délimiteur non quoté — expansion \$? (après false → 1)"
run_test "cat << EOF | /bin/cat\npiped_line\nEOF" "heredoc + pipe (stdin de cat vers second maillon)"
run_test "printf 'outer\\n' > hd_outer\ncat < hd_outer << EOF\ninner\nEOF\n/bin/cat hd_outer" "heredoc après < fichier : stdin du heredoc (stdout: inner seul; fichier reste outer)"

if [[ "${INCLUDE_BONUS_TESTS}" == "1" ]]; then
    echo -e "${YELLOW}=== bonus sujet: &&, ||, wildcard * ===${NC}"
    run_test "true && echo ms_bonus_and_ok" "bonus: && — branche si succès"
    run_test "false || echo ms_bonus_or_ok" "bonus: || — branche si échec"
    run_test "mkdir -p ms_glob_one && cd ms_glob_one && touch soleil && echo *" "bonus: * un seul fichier dans cwd"
else
    echo -e "${YELLOW}=== bonus (&&, ||, *) ignoré — INCLUDE_BONUS_TESTS=1 pour activer ===${NC}"
fi

if [[ "${INCLUDE_BONUS_TESTS}" == "1" ]]; then
    BONUSLINE="tests bonus (&&, ||, *) actifs."
else
    BONUSLINE="tests bonus desactifs. Utiliser INCLUDE_BONUS_TESTS=1 pour &&, || et *."
fi

cat "$REPORT_FILE.cards" >> "$REPORT_FILE" || true
echo "</div></body></html>" >> "$REPORT_FILE"

HTML_BONUS=$(printf '%s' "$BONUSLINE" | sed -e 's/&/\&amp;/g')
# Délimiteur | interdit : le texte du bonus contient « || » et cassait tout le sed -i (__, __ restaient).
HTML_BONUS_FOR_SED=$(printf '%s' "$HTML_BONUS" | sed -e 's/[\/&]/\\&/g' -e 's/#/\\#/g')
sed -i \
    -e "s/__TOTAL__/$TOTAL/g" \
    -e "s/__PASS__/$PASS/g" \
    -e "s/__FAIL__/$FAIL/g" \
    -e "s/__MEM_KO__/$MEM_KO/g" \
    -e "s/__FD_KO__/$FD_KO/g" \
    -e "s#__BONUSLINE__#${HTML_BONUS_FOR_SED}#g" "$REPORT_FILE"
rm -f "$REPORT_FILE.cards"

if [[ $FAIL -eq 0 && $MEM_KO -eq 0 && $FD_KO -eq 0 ]]; then
    echo -e "${GREEN}==> TOUT EST OK ($PASS/$TOTAL). Rapport: $REPORT_FILE <==${NC}"
    exit 0
else
    echo -e "${RED}==> ECHECS ($FAIL KO func, $MEM_KO leaks, $FD_KO fd). Voir $REPORT_FILE <==${NC}"
    exit 1
fi
