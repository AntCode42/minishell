# 🎯 TODO Détaillé - Semaine 3 (25-31 mars)

**Thème de la semaine :** Pipes, Redirections effectives & Expansions

Ce document détaille au maximum les tâches de la Semaine 3 pour finaliser le traitement des commandes complexes et l'exécution générale du shell.

---

## 🛠️ PERSONNE A : EXPANSIONS & QUOTES (Le traitement des variables et chaînes)

Le but est de gérer le remplacement des variables d'environnement (`$VAR`), le code de retour (`$?`) et de retirer correctement les guillemets (quotes) avant l'exécution.

- [ ] **Gestion des Quotes (Simple vs Double)**
  - [ ] Implémenter la logique pour ignorer le contenu entre single quotes `' '` (pas d'expansion).
  - [ ] Implémenter la logique pour permettre l'expansion à l'intérieur des double quotes `" "`.
  - [ ] Retirer les quotes autour des mots une fois le traitement terminé (ex: `"bonjour"` devient `bonjour`).

- [ ] **Expansion des Variables d'Environnement (`$VAR`)**
  - [ ] Écrire une fonction `expand_var(char *str)` qui repère le signe `$`.
  - [ ] Extraire le nom de la variable (composée de caractères alphanumériques et de `_`).
  - [ ] Chercher la valeur correspondante dans la liste chaînée `t_env`.
  - [ ] Remplacer `$VAR` par sa valeur dans la chaîne de caractères (ou par une chaîne vide si elle n'existe pas).

- [ ] **Expansion du Code de Retour (`$?`)**
  - [ ] Intercepter spécifiquement la séquence `$?`.
  - [ ] La remplacer par le code de retour de la dernière commande exécutée (stocké dans une variable globale ou dans `t_mini`).

- [ ] **Builtins d'Environnement Complémentaires**
  - [ ] **Implémenter `export`** :
    - Sans argument : afficher les variables d'environnement triées alphabétiquement (avec format `declare -x VAR="value"`).
    - Avec argument : ajouter ou modifier une variable dans `t_env`.
  - [ ] **Implémenter `unset`** : supprimer une variable de la liste chaînée `t_env`.

---

## 🛠️ PERSONNE B : PIPES, REDIRECTIONS & EXÉCUTION EXTERNE (Le cœur de l'exécution)

Le but est de relier ensemble plusieurs commandes via des pipes, d'appliquer concrètement les redirections de fichiers et d'exécuter les vrais programmes système.

- [ ] **Exécution des Commandes Externes (`execve`)**
  - [ ] Écrire une fonction `get_cmd_path(char *cmd, t_env *env)` qui parcourt la variable `PATH` pour trouver l'exécutable absolu.
  - [ ] Relier le résultat avec `execve` dans un processus enfant (fork).
  - [ ] Préparer une fonction pour convertir la liste chaînée `t_env` en tableau de chaînes `char **envp` requis par `execve`.

- [ ] **Gestion des Redirections Fichiers (`<`, `>`, `>>`)**
  - [ ] Écrire une fonction qui parcourt la liste des redirections d'une commande `t_cmd`.
  - [ ] Ouvrir les fichiers avec les bons flags (`O_RDONLY` pour `<`, `O_WRONLY | O_CREAT | O_TRUNC` pour `>`, `O_WRONLY | O_CREAT | O_APPEND` pour `>>`).
  - [ ] Utiliser `dup2` pour rediriger `STDIN` (0) ou `STDOUT` (1) vers le file descriptor du fichier ouvert.
  - [ ] Gérer l'erreur si un fichier n'existe pas ou n'a pas les droits.

- [ ] **Implémenter le Here-Doc (`<<`)**
  - [ ] Ouvrir un pipe ou un fichier temporaire lorsqu'un here-doc est détecté.
  - [ ] Lancer une boucle `readline` (ou `get_next_line`) qui lit l'entrée utilisateur jusqu'à rencontrer le délimiteur.
  - [ ] Rediriger le bout de lecture vers `STDIN` de la commande.

- [ ] **Implémenter l'Architecture des Pipes (`|`)**
  - [ ] Gérer le branchement multi-processus : si la commande suivante existe, appeler `pipe(fd)`.
  - [ ] Faire un `fork()` pour chaque commande connectée par un pipe.
  - [ ] Brancher correctement le `STDOUT` de la commande N vers le `fd[1]` du pipe, et le `STDIN` de la commande N+1 vers le `fd[0]` du pipe parent.
  - [ ] Fermer tous les file descriptors inutilisés dans chaque processus pour éviter les blocages.
  - [ ] Utiliser `wait`/`waitpid` dans le parent pour attendre la fin de chaque processus et récupérer le code de sortie (`$?`).

---

## 🚀 INTÉGRATION FINALE DE FIN DE SEMAINE

Ce qui doit fonctionner d'ici la fin de semaine :

1. Les pipes : `ls -l | grep "src" | wc -l` fonctionnent ensemble correctement sans rester bloqués.
2. Les redirections : `< Makefile cat | grep "NAME" > result.txt` marche de bout en bout.
3. Les expansions : `echo "Le chemin est $PATH"` (avec variables correctement interprétées dans les doubles quotes, mais pas dans `'$PATH'`).
4. Vérification Valgrind : S'assurer qu'aucun file descriptor ne reste ouvert par erreur (`--track-fds=yes`).
