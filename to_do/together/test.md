# 🧪 Tests Exhaustifs pour Minishell

Ce fichier regroupe les commandes et cas limites à tester régulièrement pour s'assurer de la stabilité du Minishell, en particulier pour le **Lexer**, **Parser**, et **Expander**. 

Pensez à lancer votre shell avec `valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./minishell` pour vérifier qu'aucune fuite de mémoire ou file descriptor ouvert ne traîne sur ces cas extrêmes.

---

## 1. 🔤 Quotes & Strings (Lexer)

Vérifier la bonne encapsulation des chaînes et le rejet des quotes mal formées.

*   **Quotes non fermées** (Doit afficher une erreur de syntaxe et ne pas crasher)
    `echo "bonjour`
    `echo 'test`
    `echo "hello'`
    `ls -la" | grep "*.c"`

*   **Quotes vides**
    `echo ""`
    `echo ''`
    `""`

*   **Quotes imbriquées**
    `echo " ' "`
    `echo ' " '`
    `echo "'bonjour'"`
    `echo '"bonjour"'`

*   **Quotes collées** (Doit concaténer en un seul grand argument)
    `echo "bon"'jour'`
    `echo 'a'"b"'c'`

*   **Espaces et tabulations multiples**
    `echo "a     b"`
    `echo a     b`

---

## 2. 💵 Expansions & Variables (Expander)

Vérifier que les variables d'environnement sont bien remplacées, correctement gérées dans/hors des quotes, et que le `$?` fonctionne.

*   **Cas basiques**
    `echo $USER`
    `echo $PWD`

*   **Variables non existantes**
    `echo $NEXISTEPAS`
    `echo "$NEXISTEPAS"`
    `echo '$NEXISTEPAS'`

*   **Variables collées**
    `echo $USER$PWD`
    `echo "$USER"$PWD`
    `echo "bonjour$USER"`
    `echo "$USER"bonjour`

*   **Limites alphanumériques des variables (très important !)**
    `echo $USER-bonjour` *(Doit donner "chapoux-bonjour")*
    `echo $USER_bonjour` *(Doit chercher la variable "USER_bonjour", donc vide si elle n'existe pas)*
    `echo $123` *(Variable numérique, doit donner vide ou ignorer)*

*   **Cas spéciaux du signe $**
    `echo $`
    `echo " $ "`
    `echo '$'`
    `echo "$"`

*   **Code de retour (Exit Status)**
    `echo $?`
    `echo "$?"`
    `echo '$?'`
    `echo $?$?$?`

---

## 3. 🚦 Syntaxe, Pipes & Redirections (Parser)

Vérifier les erreurs détectées par la fonction de vérification syntaxique (`check_syntax`).

*   **Commandes vides**
    *(Ne taper que des espaces et des tabulations, taper sur Entrée)*

*   **Syntaxe de Pipes**
    `|` *(Erreur : pipe tout seul)*
    `echo a | | echo b` *(Erreur : double pipe)*
    `| echo a` *(Erreur : pipe en début)*
    `echo a |` *(Généralement une erreur de syntaxe dans Minishell car pas de commande qui suit)*

*   **Syntaxe de Redirections**
    `>` *(Erreur de syntaxe : chevron sans fichier)*
    `<` *(Erreur de syntaxe)*
    `echo a > > file` *(Erreur)*
    `echo a >< file` *(Erreur)*
    `echo a >>> file` *(Erreur)*
    `<<<<<<< file` *(Erreur)*
    `echo a > | grep b` *(Erreur : pas de fichier avant le pipe)*

*   **Redirections valides complexes**
    `> out.txt` *(Création ou écrasement avec rien dedans)*
    `< in.txt cat > out.txt | grep a >> out2.txt`
    `cat < Makefile | grep NAME > test.txt`

---

## 4. 🛠️ Builtins (Pour plus tard, à valider)

*   **echo**
    `echo -n bonjour`
    `echo -n -n -n bonjour`
    `echo -nnnnn bonjour`
    `echo -n -x bonjour` *(Affiche "-x bonjour")*

*   **cd & pwd**
    `cd /tmp`
    `pwd`
    `cd ..`
    `cd` *(Si géré, doit ramener à HOME)*
    `cd ""` *(Ne devrait pas crasher)*

*   **export & unset**
    `export TEST=bonjour`
    `echo $TEST`
    `export TEST="hello world"`
    `echo $TEST`
    `unset TEST`
    `echo $TEST`

*   **exit**
    `exit`
    `exit 42`
    `exit a` *(Numeric argument required)*
    `exit 1 2` *(Too many arguments)*

---

## 5. 💣 Crash Tests Exécuteur (Pour la Personne B)

Ces commandes font souvent planter les exécuteurs/pipes mal sécurisés (laissent des processus zombies ou segfault).

*   **Null commands**
    `/bin/ls ""`
    `/bin/echo "" "" ""`

*   **Les commandes qui bouclent et forcent le SIGPIPE**
    `cat /dev/urandom | head -n 1`
    `yes | head -n 10`

*   **Les chemins incorrects ou dossiers**
    `/bin/ls/`
    `/nonexistent_directory/`
    `./` *(Is a directory)*

*   **Lancement récursif**
    `./minishell` *(Doit lancer un sous-shell correctement et on peut quitter ce sous-shell)*