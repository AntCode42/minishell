#!/usr/bin/env bash
# MINISHELL TESTER RAPIDE - HTML VERSION
# 100% Bash pur, pas de Python, pas de Valgrind. Idéal pour la dev-loop.
# Compare la sortie à bash --posix (non interactif). Suffixe prompt (echo -n) : stdout ms. seulement.
set -u

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 /chemin/vers/minishell"
    exit 1
fi

MINISHELL_INPUT="$1"
if [[ ! -x "$MINISHELL_INPUT" ]]; then
    echo "Erreur: '$MINISHELL_INPUT' n'est pas exécutable."
    exit 1
fi

MINISHELL="$(cd "$(dirname "$MINISHELL_INPUT")" && pwd -P)/$(basename "$MINISHELL_INPUT")"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPORT_FILE="${REPORT_FILE:-$SCRIPT_DIR/minishell_report_rapide.html}"
TMP_ROOT="$(mktemp -d /tmp/ms_rapide_XXXXXX)"
WORKDIR="$TMP_ROOT/workdir"
mkdir -p "$WORKDIR"

TOTAL=0
PASS=0
FAIL=0

cleanup() {
    rm -rf "$TMP_ROOT"
}
trap cleanup EXIT

html_escape() {
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g'
}

normalize_output() {
    local file="$1"
    local is_ms="${2:-0}"
    local input_file="${3:-}"
    local stream="${4:-out}"

    # Nettoyage classique des ANSI et sauts de ligne DOS
    sed -i -E 's/\x1b\[[0-9;?]*[ -/]*[@-~]//g' "$file"
    sed -i 's/\r//g; s/\x00//g' "$file"
    
    # Prefix des erreurs
    sed -i -E 's/^bash: line [0-9]+: //g; s/^bash: //g; s/^minishell: //g' "$file"

    if [[ "$is_ms" == "1" && -f "$input_file" ]]; then
        # 1. Enlever tout type de prompt affiché en début de ligne
        sed -i -E 's/^(minishell\$ |minishell > |minishell\$|> )+//g' "$file"
        # Suffixe prompt : stdout minishell uniquement (ne pas casser la détection stderr vide/non vide)
        if [[ "$stream" == "out" ]]; then
            sed -i -E 's/minishell\$[[:space:]]*$//g' "$file"
            sed -i -E 's/minishell >[[:space:]]*$//g' "$file"
        fi
        
        # 2. Awk filter : Retire les commandes tapées (si readline a echo) ET le exit final
        awk -v infile="$input_file" '
        BEGIN {
            while ((getline line < infile) > 0) {
                if (line != "") inputs[line]++
            }
        }
        {
            # Si on retrouve exactement une des commandes du fichier, on la supprime (une fois)
            if ($0 in inputs && inputs[$0] > 0) {
                inputs[$0]--
                next
            }
            lines[++n] = $0
        }
        END {
            # Minishell print "exit" quand un fichier input s"achève (EOF / Ctrl+D)
            if (n > 0 && lines[n] == "exit") {
                n--
            }
            while (n > 1 && lines[n] == "") {
                n--
            }
            for (i=1; i<=n; i++) {
                print lines[i]
            }
        }' "$file" > "${file}.tmp" && mv "${file}.tmp" "$file"
    fi
}

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

cat > "$REPORT_FILE" <<EOF
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Rapport Minishell Rapide</title>
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
<h1>Rapport de test Minishell (Rapide)</h1>
<p class="small"><strong>Binaire :</strong> $(printf '%s' "$MINISHELL" | html_escape)</p>
<p class="small"><strong>Date :</strong> $(date '+%Y-%m-%d %H:%M:%S')</p>
<div class="summary">
<div class="box total">Tests<br>__TOTAL__</div>
<div class="box ok">OK<br>__PASS__</div>
<div class="box ko">KO<br>__FAIL__</div>
</div>
</div>
EOF

run_test() {
    local cmd="$1"
    local desc="${2:-$cmd}"
    
    local tdir="$TMP_ROOT/test_$TOTAL"
    mkdir -p "$tdir"
    echo -e "$cmd" > "$tdir/input.sh"
    
    (
        cd "$WORKDIR" || exit 1
        bash --posix < "$tdir/input.sh" > "$tdir/sh.out" 2> "$tdir/sh.err"
        echo "$?" > "$tdir/sh.code"
    )
    
    (
        cd "$WORKDIR" || exit 1
        "$MINISHELL" < "$tdir/input.sh" > "$tdir/ms.out" 2> "$tdir/ms.err"
        echo "$?" > "$tdir/ms.code"
    )
    
    normalize_output "$tdir/sh.out" "0" "$tdir/input.sh" "out"
    normalize_output "$tdir/ms.out" "1" "$tdir/input.sh" "out"
    ms_align_trailing_nl_with_ref "$tdir/ms.out" "$tdir/sh.out"
    normalize_output "$tdir/sh.err" "0" "$tdir/input.sh" "err"
    normalize_output "$tdir/ms.err" "1" "$tdir/input.sh" "err"

    local sh_rc ms_rc
    sh_rc=$(cat "$tdir/sh.code")
    ms_rc=$(cat "$tdir/ms.code")

    local out_ok=1 err_ok=1 rc_ok=1
    cmp -s "$tdir/ms.out" "$tdir/sh.out" || out_ok=0
    [[ "$sh_rc" == "$ms_rc" ]] || rc_ok=0
    
    local sh_has_err=0 ms_has_err=0
    [[ -s "$tdir/sh.err" ]] && sh_has_err=1
    [[ -s "$tdir/ms.err" ]] && ms_has_err=1
    [[ "$sh_has_err" == "$ms_has_err" ]] || err_ok=0
    
    local badge_class="ok" status="OK"
    if [[ $out_ok -eq 1 && $err_ok -eq 1 && $rc_ok -eq 1 ]]; then
        ((PASS++))
    else
        ((FAIL++))
        badge_class="ko"
        status="KO"
    fi
    ((TOTAL++))
    
    {
        echo '<div class="card">'
        printf '<div class="status %s">%s</div>\n' "$badge_class" "$status"
        printf '<h2>%s</h2>\n' "$(printf '%s' "$desc" | html_escape)"
        echo '<div class="substatus">'
        printf '<span class="pill %s">Exit Code: %s</span>\n' "$([[ "$rc_ok" == "1" ]] && echo ok || echo ko)" "$ms_rc (bash: $sh_rc)"
        printf '<span class="pill %s">Stderr présent: %s</span>\n' "$([[ "$err_ok" == "1" ]] && echo ok || echo ko)" "$ms_has_err (bash: $sh_has_err)"
        echo '</div>'
        
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
        
        echo '</div></div>'
    } >> "$REPORT_FILE.cards"
    
    if [[ "$status" == "OK" ]]; then
        echo -e "${GREEN}[OK]${NC} $desc"
    else
        echo -e "${RED}[KO]${NC} $desc"
    fi
}

run_test "echo > >" "Syntax Error: Double Chevron"
run_test "< <" "Syntax Error: Double input"
run_test "| |" "Syntax Error: Double Pipe"
run_test "ls | " "Syntax Error: Pipe en toute fin"
run_test "| ls" "Syntax Error: Pipe en tout début"
run_test "echo '" "Syntax Error: Quote non fermée"
run_test "echo \"'chapoux'\"" "Vraies variables dans doubles quotes"
run_test "echo '\"\$USER\"'" "Fausses variables dans simples quotes"
run_test "echo \$?\$?" "Variables spéciales collées"
run_test "ls | exit" "Exit dans un pipe"
run_test "export 1=2" "Export: Identifiant numérique invalide"
run_test "export =" "Export: Egal seul"
run_test "export VAR=test && echo \$VAR" "Export simple"
run_test "export VAR=test && unset VAR && echo \$VAR" "Unset simple"
run_test "unset PATH && ls" "Commande système sans PATH (doit renvoyer erreur)"
run_test "export EMPTY= && echo \$EMPTY" "Variable vide"
run_test "echo hello | cat | wc -c" "Triple pipe classique"
run_test "idontexist > out" "Commande inexistante avec redirection out"
run_test "cat < inexistant" "Redirection in fichier absent"
run_test "echo test > out1 && cat out1 > out2 && cat out2" "Redirections en cascade"
run_test "cat << EOF\nbonjour\nEOF" "Heredoc basique"
run_test "cat << \"EOF\"\n\$USER\nEOF" "Heredoc non-expansé"
run_test "cd doesnotexist" "cd: dossier inexistant"
run_test "cd out1 out2" "cd: trop d'arguments"
run_test "./out1" "Fichier standard executé"
run_test "mkdir testdir && ./testdir" "Exécution d'un dossier"

cat "$REPORT_FILE.cards" >> "$REPORT_FILE" || true
echo "</div></body></html>" >> "$REPORT_FILE"

sed -i -e "s/__TOTAL__/$TOTAL/g" -e "s/__PASS__/$PASS/g" -e "s/__FAIL__/$FAIL/g" "$REPORT_FILE"
rm -f "$REPORT_FILE.cards"

if [[ $FAIL -eq 0 ]]; then
    echo -e "${GREEN}==> TOUT EST OK ($PASS/$TOTAL). Rapport généré dans $REPORT_FILE <==${NC}"
    exit 0
else
    echo -e "${RED}==> ECHECS DÉTECTÉS ($FAIL KOs sur $TOTAL tests). Voir $REPORT_FILE <==${NC}"
    exit 1
fi
