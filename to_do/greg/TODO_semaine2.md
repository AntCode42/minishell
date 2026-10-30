# 🎯 TODO Détaillé - Semaine 2 (18-24 mars)

**Thème de la semaine :** Parsing basique & Builtins (Partie 1)

Ce document détaille au maximum les tâches de la Semaine 2 pour faciliter la répartition entre le travail de parsing et l'implémentation des fonctions basiques du shell.

---

## 🛠️ PERSONNE A : PARSING (Transformation des Tokens en Commandes)

Le but est de prendre la liste chaînée de `t_token` (qui marche depuis la semaine 1) et d'en faire une structure `t_cmd` prête à être envoyée à l'exécuteur.

- [ ] **Définir/Ajuster les structures (dans `minishell.h`)**
  - Validation de `t_cmd` : doit contenir un tableau de chaînes `char **cmd_args` (ex: `["ls", "-l", NULL]`).
  - Validation de `t_redir` : liste chaînée contenant le type de redirection (in, out, append, heredoc) et le nom du fichier ciblé.
  - S'assurer que `t_cmd` contient des pointeurs vers la commande suivante, la précédente (pour les pipes) et un pointeur vers ses redirections.

- [ ] **Boucle de Parsing Principale (Fichier: `src/parser/parser.c`)**
  - [ ] Écrire la fonction `parse_tokens(t_token *tokens)` qui renvoie un `t_cmd *`.
  - [ ] Parcourir les tokens un par un.
  - [ ] Compter le nombre de `WORD` pour une commande donnée afin de faire le malloc de `char **cmd_args`.

- [ ] **Gestion des Redirections**
  - [ ] Lorsqu'un token `<` , `>`, `>>` ou `<<` est racontré, le token *suivant* doit être isolé et stocké comme cible de la redirection (nom de fichier ou délimiteur).
  - [ ] Créer le nœud `t_redir` correspondant et l'ajouter à la sous-liste de redirections de la commande courante.
  - [ ] Ignorer/sauter ces tokens lors de la construction du tableau d'arguments `cmd_args`.

- [ ] **Vérifications Syntaxiques Basiques (Fichier: `src/parser/syntax.c`)**
  - [ ] Rejeter une redirection en fin de commande s'il n'y a pas de fichier (ex: `ls > ` -> erreur de syntaxe).
  - [ ] Rejeter les pipes vides (ex: `| ls`, `ls | | wc`).
  - [ ] Imprimer une erreur type bash : `minishell: syntax error near unexpected token ...`.

- [ ] **Debug et Mémoire**
  - [ ] Créer une fonction `print_cmd(t_cmd *cmd)` (similaire à `print_tokens`) pour visualiser correctement l'arbre.
  - [ ] Créer une fonction `free_cmd(t_cmd *cmd)` permettant de clean proprement l'arborescence (y compris le double tableau des args et les nœuds de redirections).

---

## 🛠️ PERSONNE B : BUILTINS & INTÉGRATION (Amorce de l'Exécution)

Le but de la semaine est d'implémenter un premier set de builtins fonctionnels, et de faire le lien entre le résultat du parser et le moteur d'exécution créé en semaine 1.

- [ ] **Architecture Builtin (dans un dossier `src/builtins/`)**
  - [ ] Créer le "dispatcher" : une fonction `is_builtin(char *cmd_name)` qui renvoie un entier ou booléen pour court-circuiter `execve`.
  - [ ] Créer une fonction `exec_builtin(t_cmd *cmd, t_mini *mini)` qui lance la bonne fonction en fonction de la chaîne `cmd->cmd_args[0]`.

- [ ] **Implémenter `echo`**
  - [ ] Imprimer les arguments séparés par un espace, suivis d'un retour à la ligne.
  - [ ] Gérer l'option `-n` : s'il est présent en tant que premier argument valide, ne pas imprimer le dernier retour à la ligne.
  - [ ] Attention aux multiples `-n` consécutifs (ex: `echo -n -n -n toto`).

- [ ] **Implémenter `pwd`**
  - [ ] Utiliser `getcwd(NULL, 0)` ou un buffer static pour récupérer le dossier courant.
  - [ ] L'imprimer sur la sortie standard.

- [ ] **Implémenter `env`**
  - [ ] Parcourir la liste chaînée `t_env` de la structure de base.
  - [ ] Ne print que les variables ayant une valeur (exclure celles déclarées seules dans export, ce sera pour plus tard).

- [ ] **Implémenter `cd`**
  - [ ] Utiliser la fonction système `chdir(path)`.
  - [ ] Mettre à jour `$PWD` avec le nouveau chemin.
  - [ ] Mettre à jour `$OLDPWD` avec l'ancien chemin (avant le `chdir`).
  - [ ] Gérer le cas `cd` seul (sans arg) -> doit utiliser la variable `HOME`.

- [ ] **Implémenter `exit` (Basique)**
  - [ ] Quitter proprement avec l'impression `exit` sur STDERR.
  - [ ] Si fourni avec un argument numérique (ex: `exit 42`), quitter avec le code de sortie 42.

- [ ] **Branchement Parser <-> Executor**
  - [ ] Adapter la boucle principale (`main.c` / `minishell_loop`) pour lancer `parse_tokens()` puis l'executor avec le `t_cmd` généré.
  - [ ] Modifier l'executor S1 pour qu'il exécute `t_cmd->cmd_args[0]` avec `t_cmd->cmd_args` comme argv, plutôt que l'ancienne logique textuelle de test rapide.

---

## 🚀 INTÉGRATION FINALE DE FIN DE SEMAINE

Ce qui doit fonctionner d'ici le 24 mars à l'issue du merge (test ensemble) :
1. `echo toto > fichier.txt` -> Le parser le découpe bien, l'executor lance un builtin `echo` (bon, les redirections ne sont pas encore formellement exécutées, mais elles sont isolées !).
2. Taper `pwd`, `cd ..`, `pwd` pour vérifier que le chemin dynamique se met à jour.
3. Vérifier avec Valgrind qu'après chaque commande dans la boucle infinie, les tokens ET les commandes sont libérés à 100% de la mémoire ("0 leaks in use").
