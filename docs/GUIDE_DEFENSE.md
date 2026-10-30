# Guide de Soutenance : Préparation et Questions Pièges (Minishell)

Ce guide est conçu pour vous préparer à la soutenance du projet Minishell. Lors de l'évaluation, le correcteur testera la robustesse de votre shell, mais vous posera surtout des questions de compréhension sur les mécanismes internes (système, signaux, pipes) et cherchera les failles classiques.

---

## 1. Les Concepts Fondamentaux à Maîtriser

Avant la soutenance, vous devez tous être capables d'expliquer ces fonctions en détail :

*   **`fork()`** : Duplique le processus actuel. Le parent reçoit le PID de l'enfant (ex: 4501), l'enfant reçoit 0. Cela nous permet d'exécuter une commande sans que Minishell ne se ferme.
*   **`execve(path, args, envp)`** : Remplace le programme actuel (le processus enfant) par un autre programme (ex: `/bin/ls`). *Note : si `execve` réussit, il ne retourne jamais. Tout code après `execve` n'est lu que s'il échoue.*
*   **`pipe(fd)`** : Crée un canal de communication. `fd[1]` est l'entrée (écriture), `fd[0]` est la sortie (lecture). Unidirectionnel.
*   **`dup2(old_fd, new_fd)`** : Copie un descripteur de fichier. Extrêmement utilisé pour dire : "Fais en sorte que la sortie standard 1 pointe désormais vers mon descripteur de fichier `fd_pipe[1]` ou le fichier `out.txt`".
*   **`waitpid()`** : Met le parent en pause jusqu'à ce qu'un (ou tous) ses enfants soient terminés, et récupère leur statut de sortie (pour `$?`).

---

## 2. Questions Fréquentes & Pièges (À retenir !)

### 💡 Sur les Built-ins
**Q : Pourquoi `cd`, `export`, `unset` ou `exit` modifient-ils votre environnement, mais pas quand ils sont dans un pipe (ex : `cd /tmp | echo hello`) ?**
> **Réponse attendue** : Si un built-in est dans un pipe, il s'exécute dans un *fork* (un processus enfant). Le `cd` ou `export` modifie donc l'environnement *de l'enfant*, qui est détruit juste après. Le processus parent (Minishell) reste inchangé. S'ils sont lancés SEULS (sans pipe), on ne fait PAS de fork, on les exécute directement dans le parent.

**Q : Que fait `$?` et comment est-il mis à jour ?**
> **Réponse attendue** : `$?` contient le code de retour de *la dernière commande du pipe*. Si on lance `ls | wc`, `$?` contiendra le retour de `wc`. Dans le code, on récupère le status via `waitpid()`, on l'analyse avec les macros `WIFEXITED(status)` ou `WIFSIGNALED(status)` et on le stocke dans `mini->exit_status`.

### 💡 Sur les Signaux
**Q : Comment gérez-vous "Ctrl+C" (`SIGINT`) ?**
> **Réponse attendue** : Notre gestion dépend du "mode" (`t_signal_mode`).
> - **Dans le prompt (Interactif)** : Affiche un nouveau prompt via `rl_on_new_line()` et `rl_replace_line("", 0)`.
> - **Pendant `cat` (Exécution)** : Le parent ignore le signal pour ne pas mourir. L'enfant meurt, le parent capte sa mort via `waitpid()` et met `$?` à 130.
> - **Dans un Heredoc** : Met à jour une variable globale pour casser la boucle `readline` et annuler proprement.

**Q : Que fait "Ctrl+D" (EOF) ? C'est un signal ?**
> **Réponse attendue** : Non, `Ctrl+D` n'est pas un signal envoyé par le système. C'est la touche `EOF` (End Of File). Dans Readline, il se traduit par le retour d'un pointeur `NULL`. S'il y a un retour `NULL` alors qu'on attend une commande, on appelle le built-in `exit`.

### 💡 Sur le Parsing et l'Expander
**Q : Quelle est la différence entre les Single Quotes `' '` et Double Quotes `" "` ?**
> **Réponse attendue** : Dans les single quotes, TOUT est du texte littéral (rien n'est interprété, `$VAR` reste `$VAR`). Dans les double quotes, le texte est préservé mais l'Expander remplace les variables d'environnement (`$VAR`) et `$?`. Aucune des deux ne lance de découpage de mots supplémentaire au milieu de la quote.

**Q : Comment gérez-vous le heredoc `<< EOF` et `<< "EOF"` ?**
> **Réponse attendue** : Si le mot-clé (délimiteur) contient des quotes (n'importe lesquelles), l'Expander *ne remplacera pas* les variables `$VAR` dans les lignes tapées par l'utilisateur. Si le EOF n'est pas quoté, les variables seront étendues. C'est géré par le flag `quoted` du lexer/parser.

### 💡 Sur les Pipes, FDs et Appels Systèmes
**Q : Pourquoi faut-il ABSOLUMENT fermer les bouts de pipe que l'on n'utilise pas ?**
> **Réponse attendue** : Si un processus parent (ou un autre enfant) garde ouvert le côté écriture (`fd[1]`) d'un pipe, la commande qui lit (`fd[0]`) croira que des données peuvent encore arriver et se mettra en attente infinie (elle "pends" et freeze). De plus, ça génère des "FD leaks" limités par le système (généralement 1024 fichiers ouverts max par processus).

**Q : Quelle est la différence entre `dup()` et `dup2()` ?**
> **Réponse attendue** : `dup()` crée une copie du file descriptor en utilisant le plus petit FD disponible fourni par l'OS. `dup2(old_fd, new_fd)` force la copie sur un FD cible spécifique (par exemple `STDOUT_FILENO` = 1). Si `new_fd` était déjà ouvert, `dup2` le ferme d'abord proprement.

**Q : Comment fonctionne `execve` et pourquoi devez-vous recréer un `char **envp` ?**
> **Réponse attendue** : `execve` remplace le programme appelant. Il a besoin de 3 éléments temporels : le chemin exact (`/bin/ls`), le tableau d'arguments (`["ls", "-l", NULL]`) et l'environnement sous format strict `char **`. Puisque notre environnement interne optimal est une liste chaînée `t_env`, on doit momentanément la reconvertir en tableau dynamique de strings juste avant l'appel.

**Q : Comment gérez-vous l'attente de plusieurs processus dans le cas `cmd1 | cmd2 | cmd3` ?**
> **Réponse attendue** : Minishell lance tous les processus via `fork()` en une chaîne ininterrompue. Ensuite, on utilise une boucle autour de `waitpid()` pour s'assurer que TOUS les processus enfants se ferment afin d'éviter les processus "zombies". Le statut récupéré pour `$?` doit spécifiquement être le retour de la *dernière* commande du pipe (`cmd3`).

### 💡 Sur le Parsing et le Fonctionnement Interne
**Q : Que se passe-t-il si l'utilisateur tape `> file1 > file2 > file3 echo oui` ?**
> **Réponse attendue** : Les trois fichiers vont être visités par le shell (créés ou écrasés). Cependant, seule l'écriture finale s'appliquera au dernier descripteur assigné. Résultat : les 3 fichiers existent, mais seuls `file3` contiendra "oui".

**Q : Quelle est la différence entre l'appel `access(path, F_OK)` et `access(path, X_OK)` ?**
> **Réponse attendue** : `F_OK` vérifie seulement si le fichier existe sur le disque. `X_OK` vérifie si l'utilisateur a explicitement des permissions d'exécution dessus. Cela nous permet d'afficher l'erreur exacte : soit "Command not found" (Exit 127) si ça n'existe pas, soit "Permission denied" (Exit 126) si ce n'est pas exécutable.

**Q : Pourquoi une erreur de syntaxe (ex: `| |`) modifie `$?` à 2 ?**
> **Réponse attendue** : C'est la convention standard adoptée par Bash (et POSIX). Lorsqu'un interpréteur stoppe à cause d'un "unexpected token", l'erreur logique de parsing est symbolisée par le code 2.

### 💡 Sur les Built-ins Temporels et Environnement
**Q : Quel est le comportement exact de `export` quand on l'invoque sans argument ?**
> **Réponse attendue** : L'environnement actif complet doit être affiché mais avec des contraintes spécifiques : la liste entière doit être affichée *dans l'ordre alphabétique*, et chaque élément est préfixé d'un "declare -x ".

**Q : Que fait Bash avec la variable `$SHLVL` (Shell Level) au démarrage ?**
> **Réponse attendue** : SHLVL compte à quelle profondeur de sous-shell on se trouve. Si on lance(`./minishell`) par dessus un shell existant, on doit analyser le SHLVL actuel, l'incrémenter numériquement de 1, et l'exporter. Si elle n'existait pas, on l'initialise à 1.

**Q : Le code d'erreur (`exit X`) a-t-il une limite mathématique ? Que donne `exit 256` ou `exit -1` ?**
> **Réponse attendue** : L'exit status posix utilise seulement 1 octet (valeurs 0 à 255). Donc un `exit 256` bouclera avec le modulo et renverra le code `0`. Un `exit -1` renverra `255`. Lancer un exit non valide numériquement `exit abc` provoquera une erreur "numeric argument required" et un return de 2.

**Q : Que se passe-t-il si on `unset PWD` puis qu'on utilise `cd` ?**
> **Réponse attendue** : Lorsque `PWD` ou `OLDPWD` est supprimé de l'environnement (avec `unset`), un simple `pwd` fonctionnera toujours s'il utilise l'appel système `getcwd`. Cependant, lors de l'exécution de `cd`, le shell a le devoir de mettre à jour son environnement. S'il constate que `PWD` ou `OLDPWD` n'existent plus dans la liste (à cause du `unset`), il doit les recréer entièrement avec les nouveaux chemins et les rajouter à l'environnement. C'est un test très spécifique récurrent des évaluations.

### 💡 Mémoire et Débogage (Valgrind)
**Q : Quelle est la différence sur Valgrind entre une "Definitely lost" et "Still reachable" ?**
> **Réponse attendue** : "Definitely lost" signale une mémoire qu'on a allouée (`malloc`) mais dont on a écrasé ou perdu le pointeur : on doit corriger ça (fail de projet). "Still reachable" veut dire que la mémoire est restée allouée à la mort du programme mais on en connaissait l'adresse. En projet 42, la fonction système `readline` génère inévitablement des `still reachable` inoffensifs qui sont acceptés par les correcteurs.

**Q : Expliquez votre logique de libération mémoire (Garbage Collector) pour `t_cmd` vs `t_env` ?**
> **Réponse attendue** : `t_env` a l'espérance de vie maximale. Il est alloué au boot, modifié si nécessaire, et libéré tout en dernier lors de la mort volontaire de l'application (exit). Cependant, `t_cmd` et ses arguments et ses listes `t_redir` sont éphémères : elles sont allouées individuellement pour *chaque ligne saisie* et doivent obligatoirement être free() avant un nouveau passage à l'attente `readline()`, faute de quoi chaque "Entrée" augmenterait l'utilisation de la RAM.

---

## 3. Les Cas "Extrêmes" (Edge Cases) à tester

Le correcteur testera sûrement ces commandes pour casser votre shell. Assurez-vous que votre Minishell y survit sans Segfault ni Leaks :

1.  **Multiple pipes sans rien :**
    `cat | cat | cat | ls` (La commande s'arrête gentiment, sans bloquer).
2.  **Variables collées :**
    `echo $USER$USER` (Doit afficher chapouxchapoux).
    `echo "$USER="` (Doit afficher chapoux=).
3.  **Variable inexistante :**
    `echo $INEXISTANT bonjour` (Doit afficher " bonjour" et `$INEXISTANT` disparaît proprement au lieu de casser).
4.  **Redirection sur fichier sans droit :**
    Faites `chmod 000 secret.txt` puis `ls -l > secret.txt`. (Doit afficher "Permission denied", `$?` doit valoir 1, et le shell ne doit pas Segfault).
5.  **Exécution d'un dossier (et non d'un binaire) :**
    `./src` (Doit afficher "Is a directory" sans exécuter).
6.  **La variable PATH supprimée :**
    Tapez `unset PATH`, puis essayez de taper `ls`. (Doit afficher "No such file or directory" car il ne trouve plus de binaire externe sans PATH. Les built-ins comme `echo` ou `export` doivent eux continuer de marcher !).
7.  **Exit numérique et non-numérique :**
    `exit 42` (Le shell ferme, fait un `echo $?` -> affiche 42).
    `exit bonjour` (Affiche `numeric argument required` et exit statut = 2).
8.  **Le piège de PWD :**
    Tapez `unset PWD` (ou `unset OLDPWD`), puis faites `cd /tmp` et vérifiez l'état de `env`. (Le shell doit avoir recréé les variables `PWD` et `OLDPWD` automatiquement pour contenir les bons répertoires, et ne doit pas crasher).

---

## 4. Checklist Avant l'Évaluation

- [ ] Lancer `valgrind ./minishell` et tester plein de commandes. **Il ne doit y avoir aucun leak** ou bloc mémoire "definitely lost".
- [ ] Tester les FDs qui fuient avec `valgrind --track-fds=yes ./minishell`. Chaque fichier ouvert *doit* être fermé (fermez bien vos FDs en cas d'erreur de commande).
- [ ] Vérifier si les signaux fonctionnent proprement (Le `Ctrl+C` et `Ctrl+\` dans `sleep 10` ou dans `cat` font exactement la même chose que sur bash).
- [ ] `env -i ./minishell` (Qu'est-ce qui se passe quand on lance Minishell dans un environnement 100% vide ? Votre programme ne doit pas segfault, même sans `PATH` ni `PWD`).

*Astuce finale :* Si le correcteur parvient à casser un cas tellement absurde que Bash lui-même fait une erreur bizarre, montrez-lui comment le vrai Bash réagit pour argumenter que vous avez correctement imité le comportement officiel !

---

## 5. Exemples de Workflows (Pipes & Redirections)

Pour réussir la soutenance, vous devez être capables de dessiner au tableau ce qui se passe sous le capot pour chaque type de flux. 

### 🔄 Le Pipe (`|`)
**Exemple : `ls | wc`**
1. Le parent (Minishell) appelle `pipe(fd)`. Cela crée `fd[0]` (lecture) et `fd[1]` (écriture).
2. Le parent fait un premier `fork()` pour la commande `ls`.
3. Dans l'enfant `ls` : appel de `dup2(fd[1], STDOUT_FILENO)`. La sortie standard va dans le pipe. Fermeture de `fd[0]` et `fd[1]`. `execve("ls")`.
4. Le parent fait un deuxième `fork()` pour `wc`.
5. Dans l'enfant `wc` : appel de `dup2(fd[0], STDIN_FILENO)`. L'entrée standard provient du pipe. Fermeture de `fd[0]` et `fd[1]`. `execve("wc")`.
6. Dans le parent (Minishell) : **Fermeture impérative de `fd[0]` et `fd[1]`**. Puis `waitpid()` sur les enfants.

### 📤 Redirections de sortie (`>` et `>>`)
**Exemple : `echo salut > fichier.txt`**
1. Fork pour exécuter `echo`.
2. Dans l'enfant : appel `open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)`.
   *Note : Pour `>>`, on utilise `O_APPEND` au lieu de `O_TRUNC`.*
3. `open` retourne un nouveau FD (ex: `fd=3`).
4. Appel de `dup2(3, STDOUT_FILENO)`. STDOUT pointe alors proprement vers `fichier.txt`.
5. Appel de `close(3)`. (FD d'origine plus nécessaire puisque pointé par STDOUT).
6. `execve("echo")`.

### 📥 Redirection d'entrée (`<`)
**Exemple : `cat < input.txt`**
1. Fork. Dans l'enfant : `open("input.txt", O_RDONLY)`.
2. Si le fichier n'existe pas ou sans droits d'accès, `open` renvoie `-1`. L'enfant affiche l'erreur, libère la mémoire et `exit(1)`.
3. Si succès (ex: `fd=3`), appel de `dup2(3, STDIN_FILENO)`.
4. `close(3)` puis `execve("cat")`.

### 📝 Le Heredoc (`<<`)
**Exemple : `cat << EOF`**
1. **Avant même l'exécution** principale, le shell lance un processus de récupération avec une boucle `readline("> ")`.
2. Il lit ligne par ligne. Si la ligne contient des variables (ex: `$USER`) ET que `EOF` n'était pas quoté, l'Expander remplace les variables à la volée.
3. Les lignes lues sont écrites dans le côté écriture d'un *Pipe temporaire* (ou un fichier temporaire caché `/tmp/minishell_heredoc...`).
4. Si l'utilisateur tape le délimiteur `EOF`, le prompt s'arrête normalement. S'il utilise `Ctrl+D` (fin de fichier prématurée), le shell s'arrête mais doit afficher un avertissement précis incluant la ligne exacte (ex: `minishell: warning: here-document at line X delimited by end-of-file (wanted 'EOF')`) pour correspondre strictement à bash.
5. Au moment de l'exécution, l'enfant `cat` fait un `dup2` sur le bout de lecture du pipe temporaire (ou du fichier). L'entrée redirige donc la saisie accumulée.