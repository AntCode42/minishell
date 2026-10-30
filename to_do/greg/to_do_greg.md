# projet MiniShell — Suivi des tâches

> ⚠️ **Ce fichier est remplacé par [SUIVI.md](SUIVI.md)** pour un suivi détaillé semaine par semaine.
> Voir aussi [ORGANISATION.md](ORGANISATION.md) pour le planning global.

## Documentations

### Liens utiles

- https://42-cursus.gitbook.io/guide/3-rank-03/minishell/functions
- https://www.gnu.org/software/bash/manual/bash.html
- https://craftinginterpreters.com/ (chapitres Scanning & Parsing)
- https://pubs.opengroup.org/onlinepubs/009695399/utilities/xcu_chap02.html
- https://pubs.opengroup.org/onlinepubs/009695399/utilities/xcu_chap02.html#tag_02_10

## Organisation du travail

### Répartition du boulot

- **Personne A** : Lexing / Parsing / Expansion / Quotes / Heredoc
- **Personne B** : Execution / Builtins / Pipes / Redirections / Signaux / Env

### Suivi détaillé → [SUIVI.md](SUIVI.md)

# 📋 Minishell — To Do List

## 🔴 Phase 1 — Setup & Structures de base
- [ ] Corriger le Makefile (~~fdf~~ → minishell) ✅ FAIT
- [ ] Créer `include/minishell.h` (header principal)
- [ ] Définir les structures : `t_token`, `t_cmd`, `t_env`, `t_shell`
- [ ] Implémenter `env_init()` : copier `envp` en liste chaînée
- [ ] Implémenter `env_get()`, `env_set()`, `env_to_array()`
- [ ] Créer `src/main.c` : boucle readline de base
- [ ] Mettre en place les signaux (prompt)

## 🟡 Phase 2 — Parsing
- [ ] **Lexer** : tokeniser l'input (WORD, PIPE, REDIR, QUOTE, etc.)
- [ ] **Syntax check** : détecter les erreurs (`||`, `>>` sans fichier, etc.)
- [ ] **Expander** : remplacer `$VAR`, `$?`
- [ ] **Quotes** : gérer les simples et doubles quotes
- [ ] **Parser** : construire la liste/arbre de commandes

## 🟡 Phase 3 — Exécution simple
- [ ] Résolution du PATH (`path.c`)
- [ ] Exécuter une commande simple (`fork` + `execve` + `waitpid`)
- [ ] Gérer le chemin absolu/relatif
- [ ] Gérer les erreurs : commande non trouvée (127), permission denied (126)
- [ ] Mettre à jour `$?` après chaque commande

## 🟡 Phase 4 — Builtins
- [ ] `echo` (avec `-n`)
- [ ] `cd` (relatif, absolu, sans arg, `-`, mise à jour `$PWD`/`$OLDPWD`)
- [ ] `pwd`
- [ ] `export` (avec et sans argument, affichage trié)
- [ ] `unset`
- [ ] `env`
- [ ] `exit` (tous les cas limites)
- [ ] Builtins seuls = dans le parent, dans un pipe = dans un fork

## 🟡 Phase 5 — Redirections
- [ ] `<` (redirect stdin)
- [ ] `>` (redirect stdout, crée/écrase)
- [ ] `>>` (redirect stdout, append)
- [ ] Redirections multiples (évaluation gauche→droite)
- [ ] Gestion des erreurs (fichier inexistant, permissions)

## 🟡 Phase 6 — Heredoc
- [ ] `<<` avec délimiteur simple (avec expansion)
- [ ] `<<` avec délimiteur entre quotes (sans expansion)
- [ ] `ctrl-C` dans heredoc (interruption propre)
- [ ] `ctrl-D` dans heredoc (warning EOF)
- [ ] Heredocs multiples

## 🟡 Phase 7 — Pipes
- [ ] Pipe simple (`cmd1 | cmd2`)
- [ ] Pipes multiples (`cmd1 | cmd2 | cmd3 | ...`)
- [ ] `$?` = exit status de la dernière commande du pipeline
- [ ] Fermeture correcte de tous les file descriptors

## 🟡 Phase 8 — Signaux complets
- [ ] `ctrl-C` au prompt (nouvelle ligne)
- [ ] `ctrl-D` au prompt (exit)
- [ ] `ctrl-\` au prompt (ignoré)
- [ ] `ctrl-C` pendant une commande (tue l'enfant)
- [ ] `ctrl-\` pendant une commande (core dump)
- [ ] `ctrl-C` dans un heredoc
- [ ] Signaux dans les processus enfants (SIG_DFL)

## 🟢 Phase 9 — Tests & Debug
- [ ] Tests manuels (voir ORGANISATION.md)
- [ ] Tester tous les cas limites (quotes, variables, redirections)
- [ ] Valgrind : 0 leaks (hors readline)
- [ ] Valgrind : 0 fd leaks (`--track-fds=yes`)
- [ ] Passer `minishell_tester` (thallard)
- [ ] Norminette : 0 erreurs
- [ ] Tester minishell dans minishell

## 🟢 Phase 10 — Polish
- [ ] Nettoyage du code
- [ ] Commentaires utiles
- [ ] Messages d'erreur au format bash : `minishell: cmd: error message`
- [ ] Vérification finale de la checklist (ORGANISATION.md)

---

> Légende : 🔴 En cours | 🟡 À faire | 🟢 À venir | ✅ Fait
