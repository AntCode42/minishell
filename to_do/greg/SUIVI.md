# 📊 Minishell — Suivi d'avancement

> **Mise à jour** : chaque vendredi (revue de semaine) + chaque jour (stand-up)
> **Référence** : [ORGANISATION.md](ORGANISATION.md) pour le planning complet

---

## 🔄 Stand-up du jour

> Remplir chaque matin au début de la session de travail.

| Date | Personne | Hier j'ai fait... | Aujourd'hui je fais... | Bloqué sur... |
|------|----------|-------------------|----------------------|---------------|
| 18/03 | A | Finition lexer et clean listes | Début parser / t_cmd | — |
| 18/03 | B | Exec avec fork et resolve PATH | Builtins basiques (echo, pwd, env) | — |
| 19/03 | A | Début parser (WORD -> t_cmd) | Parser: gérer redirections | — |
| 19/03 | B | Fin fork et tests execve | Builtins: cd et exit | — |

---

## 📅 Semaine 1 (11-17 mars) — Setup & Fondations

### Objectifs
- [x] Structures de données définies et validées ensemble
- [x] Repo git configuré (branches, .gitignore)
- [x] Lexer tokenise des commandes simples (WORD, PIPE, redirections)
- [x] Copie de l'environnement fonctionnelle
- [x] Exécution simple (`fork` + `execve`) avec `/bin/ls`

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 1.1 | Lire et comprendre le sujet | Ensemble | 🔴 Haute | ✅ | |
| 1.2 | Étudier le comportement de bash (tests manuels) | Ensemble | 🔴 Haute | ✅ | |
| 1.3 | Définir les structures (`t_token`, `t_cmd`, `t_env`, `t_mini`) | Ensemble | 🔴 Haute | ✅ | Voir ORGANISATION.md §4 |
| 1.4 | Setup repo git (branches main/develop, .gitignore) | Ensemble | 🔴 Haute | ✅ | |
| 1.5 | Créer l'arborescence `src/` | Ensemble | 🟡 Moyenne | ✅ | |
| 1.6 | Écrire `minishell.h` avec les structures | Ensemble | 🔴 Haute | ✅ | |
| 1.7 | Lexer : découper en tokens WORD et PIPE | A | 🔴 Haute | ✅ | `lexer.c` ok |
| 1.8 | Lexer : ajouter tokens de redirection (< > >> <<) | A | 🟡 Moyenne | ✅ | Géré dans le lexer |
| 1.9 | Lexer : fonction d'affichage des tokens (debug) | A | 🟡 Moyenne | ✅ | `print_tokens` faite |
| 1.10 | Env : copie de `envp` dans `t_env` (liste chaînée) | B | 🔴 Haute | ✅ | |
| 1.11 | Env : fonctions `env_get`, `env_set`, `env_unset` | B | 🔴 Haute | ✅ | |
| 1.12 | Exec : `fork()` + `execve()` pour une commande simple | B | 🟡 Moyenne | ✅ | `fork_management.c` |
| 1.13 | Exec : résolution du PATH (`find_command_path`) | B | 🟡 Moyenne | ✅ | `find_path` ok |
| 1.14 | Main loop basique : `readline` → lexer → afficher tokens | A | 🟡 Moyenne | ✅ | `minishell_loop` dans `main.c` ok |

### Revue de fin de semaine (17 mars)

| Question | Réponse |
|----------|---------|
| Livrable S1 atteint ? | ✅ Oui / ⬜ Non |
| Tests effectués | Lancement simple, tokens corrects, exécution base |
| Problèmes rencontrés | Résolution chemin PATH |
| Ajustements pour S2 | Attention à parser et gérer la mémoire (free_tokens ok) |
| Merge dans `develop` fait ? | ✅ Oui / ⬜ Non |

---

## 📅 Semaine 2 (18-24 mars) — Parsing basique & Builtins

### Objectifs
- [ ] Parser construit un `t_cmd *` à partir des tokens
- [ ] Parser gère les redirections
- [ ] Builtins `echo`, `pwd`, `env`, `cd`, `exit` fonctionnels
- [ ] Intégration parser ↔ executor testée (commande simple)

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 2.1 | Parser : tokens → `t_cmd` (commande + args) | A | 🔴 Haute | ⬜ | |
| 2.2 | Parser : extraire les redirections dans `t_redir` | A | 🔴 Haute | ⬜ | |
| 2.3 | Parser : vérification syntaxique basique | A | 🟡 Moyenne | ⬜ | |
| 2.4 | Builtin : `echo` (avec `-n`) | B | 🔴 Haute | ⬜ | |
| 2.5 | Builtin : `pwd` | B | 🟢 Facile | ⬜ | |
| 2.6 | Builtin : `env` (afficher l'environnement) | B | 🟢 Facile | ⬜ | |
| 2.7 | Builtin : `cd` (avec `$HOME`) | B | 🟡 Moyenne | ⬜ | |
| 2.8 | Builtin : `exit` (cas basiques) | B | 🟡 Moyenne | ⬜ | |
| 2.9 | Executor : détecter si builtin ou commande externe | B | 🟡 Moyenne | ⬜ | |
| 2.10 | **INTÉGRATION** : parser → executor, tester `ls -la`, `echo hello`, `pwd` | Ensemble | 🔴 Haute | ⬜ | |
| 2.11 | Free : libérer correctement `t_token` et `t_cmd` | Ensemble | 🔴 Haute | ⬜ | |

### Revue de fin de semaine (18 mars)

| Question | Réponse |
|----------|---------|
| Livrable S2 atteint ? | ⬜ Oui / ⬜ Non |
| Tests effectués | |
| Problèmes rencontrés | |
| Ajustements pour S3 | |
| Merge dans `develop` fait ? | ⬜ Oui / ⬜ Non |

---

## 📅 Semaine 3 (19-25 mars) — Pipes & Quotes

### Objectifs
- [ ] Quotes simples et doubles gérées dans le lexer
- [ ] Pipes fonctionnels pour N commandes
- [ ] Parser gère les pipes (liste chaînée)

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 3.1 | Lexer : gestion des single quotes | A | 🔴 Haute | ⬜ | |
| 3.2 | Lexer : gestion des double quotes | A | 🔴 Haute | ⬜ | |
| 3.3 | Parser : pipes → liste chaînée de `t_cmd` | A | 🔴 Haute | ⬜ | |
| 3.4 | Exec : `pipe()` + `dup2()` pour 2 commandes | B | 🔴 Haute | ⬜ | |
| 3.5 | Exec : pipes pour N commandes | B | 🔴 Haute | ⬜ | |
| 3.6 | Exec : fermer tous les fd correctement | B | 🔴 Haute | ⬜ | |
| 3.7 | Exec : `waitpid` correct (attendre tous les enfants) | B | 🟡 Moyenne | ⬜ | |
| 3.8 | **INTÉGRATION** : `ls \| grep .c \| wc -l` doit fonctionner | Ensemble | 🔴 Haute | ⬜ | |

### Revue de fin de semaine (25 mars)

| Question | Réponse |
|----------|---------|
| Livrable S3 atteint ? | ⬜ Oui / ⬜ Non |
| Valgrind lancé ? Résultat ? | |
| Norminette passée ? | |
| Problèmes rencontrés | |
| Ajustements pour S4 | |

---

## 📅 Semaine 4 (26 mars - 1er avril) — Expansion & Redirections

### Objectifs
- [ ] `$VAR` et `$?` expandés correctement (sauf dans single quotes)
- [ ] Redirections `<`, `>`, `>>` fonctionnelles
- [ ] Combos pipes + redirections + expansion

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 4.1 | Expander : `$VAR` → valeur | A | 🔴 Haute | ⬜ | |
| 4.2 | Expander : `$?` → exit status | A | 🔴 Haute | ⬜ | |
| 4.3 | Expander : `$` dans double quotes | A | 🔴 Haute | ⬜ | |
| 4.4 | Expander : pas d'expansion dans single quotes | A | 🔴 Haute | ⬜ | |
| 4.5 | Expander : cas limites (`$`, `$""`, `"$"`) | A | 🟡 Moyenne | ⬜ | |
| 4.6 | Quote removal après expansion | A | 🟡 Moyenne | ⬜ | |
| 4.7 | Redir : `>` (créer/tronquer fichier) | B | 🔴 Haute | ⬜ | |
| 4.8 | Redir : `>>` (append) | B | 🔴 Haute | ⬜ | |
| 4.9 | Redir : `<` (input depuis fichier) | B | 🔴 Haute | ⬜ | |
| 4.10 | Redir : multiples redirections sur une commande | B | 🟡 Moyenne | ⬜ | |
| 4.11 | **INTÉGRATION** : `echo $USER \| cat > file` | Ensemble | 🔴 Haute | ⬜ | |

### Revue de fin de semaine (1er avril)

| Question | Réponse |
|----------|---------|
| Livrable S4 atteint ? | ⬜ Oui / ⬜ Non |
| Valgrind lancé ? Résultat ? | |
| Problèmes rencontrés | |
| Point mi-parcours : sommes-nous dans les temps ? | |

---

## 📅 Semaine 5 (2-8 avril) — Heredoc & Signaux

### Objectifs
- [ ] Heredoc (`<<`) fonctionnel
- [ ] Signaux corrects dans tous les contextes (prompt, exec, heredoc)
- [ ] `ctrl-C`, `ctrl-D`, `ctrl-\` gérés

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 5.1 | Heredoc : lecture jusqu'au delimiter | A | 🔴 Haute | ⬜ | |
| 5.2 | Heredoc : expansion de `$` (delimiter non quoté) | A | 🟡 Moyenne | ⬜ | |
| 5.3 | Heredoc : pas d'expansion si delimiter quoté | A | 🟡 Moyenne | ⬜ | |
| 5.4 | Heredoc : stocker dans un pipe fd | A | 🔴 Haute | ⬜ | |
| 5.5 | Signaux : `ctrl-C` dans le prompt | B | 🔴 Haute | ⬜ | |
| 5.6 | Signaux : `ctrl-D` dans le prompt (EOF → exit) | B | 🔴 Haute | ⬜ | |
| 5.7 | Signaux : `ctrl-\` dans le prompt (rien) | B | 🟡 Moyenne | ⬜ | |
| 5.8 | Signaux : `ctrl-C` pendant exécution | B | 🔴 Haute | ⬜ | |
| 5.9 | Signaux : `ctrl-C` dans un heredoc | Ensemble | 🔴 Haute | ⬜ | |
| 5.10 | **INTÉGRATION** : tester heredoc + signaux | Ensemble | 🔴 Haute | ⬜ | |

### Revue de fin de semaine (8 avril)

| Question | Réponse |
|----------|---------|
| Livrable S5 atteint ? | ⬜ Oui / ⬜ Non |
| Signaux testés dans tous les contextes ? | |
| Heredoc testé avec ctrl-C ? | |

---

## 📅 Semaine 6 (9-15 avril) — Export, Unset & Cas limites

### Objectifs
- [ ] `export` complet (affichage trié, assignation)
- [ ] `unset` fonctionnel
- [ ] Vérification syntaxique avancée
- [ ] Tous les cas limites du parsing

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 6.1 | Syntax check : `\| \|`, `>>>`, `\|` en fin | A | 🔴 Haute | ⬜ | |
| 6.2 | Syntax check : quotes non fermées | A | 🔴 Haute | ⬜ | |
| 6.3 | Parsing : tokens vides, commande vide après expansion | A | 🟡 Moyenne | ⬜ | |
| 6.4 | Parsing : redirections avant la commande | A | 🟡 Moyenne | ⬜ | |
| 6.5 | `export` sans arg = affichage trié `declare -x` | B | 🔴 Haute | ⬜ | |
| 6.6 | `export A=1 B=2` (multiples assignations) | B | 🟡 Moyenne | ⬜ | |
| 6.7 | `unset` | B | 🟡 Moyenne | ⬜ | |
| 6.8 | `exit` cas complets (voir Definition of Done) | B | 🔴 Haute | ⬜ | |
| 6.9 | `$?` : vérifier tous les codes de retour | B | 🔴 Haute | ⬜ | |
| 6.10 | Builtins dans un pipe = fork | B | 🔴 Haute | ⬜ | |
| 6.11 | **TEST** : tous les cas limites de la section 9 | Ensemble | 🔴 Haute | ⬜ | |

### Revue de fin de semaine (15 avril)

| Question | Réponse |
|----------|---------|
| Livrable S6 atteint ? | ⬜ Oui / ⬜ Non |
| Tous les builtins fonctionnels ? | |
| Liste des bugs connus ? | |

---

## 📅 Semaine 7 (16-22 avril) — Nettoyage & Tests

### Objectifs
- [ ] Code propre et norminetté
- [ ] Passer les testers automatisés
- [ ] Corriger tous les bugs trouvés

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 7.1 | Code review croisée complète | Ensemble | 🔴 Haute | ⬜ | |
| 7.2 | Norminette sur tous les fichiers | Ensemble | 🔴 Haute | ⬜ | |
| 7.3 | Installer et lancer minishell_tester | Ensemble | 🔴 Haute | ⬜ | |
| 7.4 | Lancer le tester de zstenger93 | Ensemble | 🟡 Moyenne | ⬜ | |
| 7.5 | Corriger les bugs trouvés | Ensemble | 🔴 Haute | ⬜ | |
| 7.6 | Relire et commenter le code de l'autre | Ensemble | 🟡 Moyenne | ⬜ | |

### Revue de fin de semaine (22 avril)

| Question | Réponse |
|----------|---------|
| Tester 1 : % de tests passés | |
| Tester 2 : % de tests passés | |
| Bugs restants | |
| Prêt pour le polish final ? | |

---

## 📅 Semaine 8 (23-30 avril) — Polish & Défense

### Objectifs
- [ ] Zéro leak Valgrind (hors readline)
- [ ] Norminette 100%
- [ ] Simulation de défense passée
- [ ] Push final

### Tâches détaillées

| # | Tâche | Responsable | Priorité | Statut | Notes |
|---|-------|-------------|----------|--------|-------|
| 8.1 | Valgrind sur tous les scénarios | Ensemble | 🔴 Haute | ⬜ | |
| 8.2 | Corriger les derniers leaks | Ensemble | 🔴 Haute | ⬜ | |
| 8.3 | Norminette finale | Ensemble | 🔴 Haute | ⬜ | |
| 8.4 | Simulation de défense (A questionne B et vice versa) | Ensemble | 🔴 Haute | ⬜ | |
| 8.5 | Vérifier la checklist complète (ORGANISATION.md) | Ensemble | 🔴 Haute | ⬜ | |
| 8.6 | Push final sur `main` | Ensemble | 🔴 Haute | ⬜ | |

### Revue finale (30 avril)

| Question | Réponse |
|----------|---------|
| Checklist 100% complète ? | |
| Les deux peuvent tout expliquer ? | |
| Confiance pour la défense (1-10) ? | |

---

## 📈 Métriques globales

> Mettre à jour chaque vendredi.

| Semaine | Fichiers .c | Tests passés | Builtins OK | Leaks | Crashs | Norminette |
|---------|-------------|-------------|-------------|-------|--------|------------|
| S1 | | | 0/7 | N/A | N/A | N/A |
| S2 | | | /7 | | | |
| S3 | | | /7 | | | |
| S4 | | | /7 | | | |
| S5 | | | /7 | | | |
| S6 | | | /7 | | | |
| S7 | | | /7 | | | |
| S8 | | | 7/7 | 0 | 0 | ✅ |

---

## 🐛 Bug tracker

> Ajouter les bugs trouvés ici. Les marquer ✅ quand corrigés.

| # | Date | Description | Sévérité | Trouvé par | Corrigé par | Statut |
|---|------|------------|----------|-----------|-------------|--------|
| | | | | | | |

---

## 💡 Décisions techniques

> Documenter ici les choix techniques importants pour s'en souvenir.

| Date | Décision | Raison |
|------|----------|--------|
| 07/03 | Environnement en `t_env` (liste chaînée) plutôt que `char **` | Plus flexible pour export/unset, pas de realloc |
| 07/03 | Variable globale = `volatile sig_atomic_t g_signal_received` | Seule variable globale autorisée, type safe pour les signaux |
| | | |
