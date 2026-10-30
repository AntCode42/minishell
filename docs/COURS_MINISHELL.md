# 🐚 Minishell — Cours Complet

> **Auteur** : Support d'apprentissage pour le projet minishell (42)
> **Objectif** : Comprendre TOUTES les notions nécessaires, de zéro jusqu'à l'implémentation
> **Règle** : Aucun code de minishell ici — uniquement des exemples pédagogiques et des explications

---

## 📋 Table des matières

### Partie I — Comprendre le shell
1. [Qu'est-ce qu'un shell ?](#1--quest-ce-quun-shell-)
2. [Comment bash traite une commande](#2--comment-bash-traite-une-commande)
3. [Les processus Unix](#3--les-processus-unix)

### Partie II — Les fonctions autorisées
4. [Fonctions de gestion de processus](#4--fonctions-de-gestion-de-processus)
5. [Fonctions de fichiers et file descriptors](#5--fonctions-de-fichiers-et-file-descriptors)
6. [Fonctions de pipes](#6--fonctions-de-pipes)
7. [Fonctions de signaux](#7--fonctions-de-signaux)
8. [Fonctions de terminal](#8--fonctions-de-terminal)
9. [Fonctions d'environnement et chemins](#9--fonctions-denvironnement-et-chemins)
10. [Fonctions readline](#10--fonctions-readline)
11. [Autres fonctions utiles](#11--autres-fonctions-utiles)

### Partie III — Le pipeline de traitement
12. [Le Lexer (Tokenisation)](#12--le-lexer-tokenisation)
13. [Le Parser (Analyse syntaxique)](#13--le-parser-analyse-syntaxique)
14. [L'Expander (Expansion des variables)](#14--lexpander-expansion-des-variables)
15. [Les Quotes (Guillemets)](#15--les-quotes-guillemets)

### Partie IV — L'exécution
16. [Exécution d'une commande simple](#16--exécution-dune-commande-simple)
17. [Les pipes en détail](#17--les-pipes-en-détail)
18. [Les redirections](#18--les-redirections)
19. [Le heredoc](#19--le-heredoc)

### Partie V — Les builtins
20. [Les builtins — Vue d'ensemble](#20--les-builtins--vue-densemble)
21. [echo](#21--echo)
22. [cd](#22--cd)
23. [pwd](#23--pwd)
24. [export](#24--export)
25. [unset](#25--unset)
26. [env](#26--env)
27. [exit](#27--exit)

### Partie VI — Les signaux
28. [Les signaux Unix](#28--les-signaux-unix)
29. [Signaux dans minishell](#29--signaux-dans-minishell)

### Partie VII — Gestion mémoire et erreurs
30. [Stratégie de gestion mémoire](#30--stratégie-de-gestion-mémoire)
31. [Gestion des erreurs](#31--gestion-des-erreurs)
32. [Exit status ($?)](#32--exit-status-)

### Partie VIII — Cas limites et pièges
33. [Cas limites exhaustifs](#33--cas-limites-exhaustifs)
34. [Différences entre bash et minishell](#34--différences-entre-bash-et-minishell)

---

# PARTIE I — COMPRENDRE LE SHELL

---

## 1. 🖥️ Qu'est-ce qu'un shell ?

### L'analogie du traducteur

Imagine que tu es dans un pays étranger. Tu ne parles pas la langue locale (le langage machine), mais tu as un **traducteur** avec toi. Tu lui parles en français (des commandes comme `ls`, `echo hello`), et lui traduit tes demandes pour que les locaux (le système d'exploitation) les comprennent et les exécutent.

**Le shell, c'est ce traducteur.**

```
Toi (utilisateur)
    │
    │  "ls -la"
    ▼
┌──────────┐
│  SHELL   │  ← le traducteur
└────┬─────┘
     │  appels système (fork, execve...)
     ▼
┌──────────┐
│   OS     │  ← le système d'exploitation
│ (kernel) │
└──────────┘
```

### Shell vs Terminal

- Le **terminal** (ou émulateur de terminal) = la fenêtre noire dans laquelle tu tapes. C'est le **téléphone**.
- Le **shell** = le programme qui interprète tes commandes. C'est la **personne au bout du fil**.

Tu peux avoir le même terminal avec différents shells (`bash`, `zsh`, `fish`...), comme tu peux appeler différentes personnes avec le même téléphone.

### Ce que fait un shell, étape par étape

Quand tu tapes `echo hello | cat -e > out` et appuies sur Entrée, le shell fait ceci :

1. **Lire** l'input (la ligne que tu as tapée)
2. **Découper** en mots/tokens : `echo`, `hello`, `|`, `cat`, `-e`, `>`, `out`
3. **Analyser** la structure : il y a un pipe, une redirection
4. **Étendre** : remplacer `$VAR` par sa valeur (pas ici, mais si tu avais tapé `echo $USER`)
5. **Exécuter** : lancer les programmes, connecter les pipes, ouvrir les fichiers
6. **Attendre** que tout se termine
7. **Recommencer** (afficher le prompt)

> **Minishell** = recréer ce cycle complet en C.

---

## 2. 🔄 Comment bash traite une commande

### Le pipeline de traitement

Bash traite chaque ligne en **4 étapes séquentielles** que tu dois reproduire :

```
"echo $USER | cat -e > out"
         │
         ▼
    ┌─────────┐
    │  LEXER  │   Découpage en tokens
    └────┬────┘
         │
         ▼
    ┌─────────┐
    │ PARSER  │   Construction de la structure de commandes
    └────┬────┘
         │
         ▼
    ┌──────────┐
    │ EXPANDER │   Remplacement des $VARIABLES
    └────┬─────┘
         │
         ▼
    ┌──────────┐
    │ EXECUTOR │   Exécution réelle (fork, exec, pipes)
    └──────────┘
```

### Analogie de la cuisine

Pense à un restaurant :
- **Lexer** = le serveur qui prend la commande et la note sur un ticket (découpage en éléments)
- **Parser** = le chef qui lit le ticket et organise les plats par étape (structuration)
- **Expander** = le sous-chef qui remplace "viande du jour" par "bœuf" (substitution)
- **Executor** = les cuisiniers qui préparent réellement les plats (exécution)

Chaque étape ne peut fonctionner que si la précédente a été faite correctement.

---

## 3. ⚙️ Les processus Unix

### Qu'est-ce qu'un processus ?

Un **processus** = un programme en cours d'exécution. Quand tu tapes `ls`, le shell crée un **nouveau processus** pour exécuter le programme `/bin/ls`.

### Analogie de l'entreprise

- Le **processus parent** = le manager (ton shell)
- Le **processus enfant** = l'employé (la commande `ls`)

Le manager reçoit une tâche ("liste les fichiers"), il la délègue à un employé (il crée un processus enfant), l'employé fait le travail, puis revient dire au manager "c'est fait, voici le résultat" (code de retour).

### PID (Process ID)

Chaque processus a un numéro unique : son **PID**.

```bash
$ echo $$        # Affiche le PID du shell actuel
42               # Exemple

$ ps             # Liste les processus
  PID TTY          TIME CMD
   42 pts/0    00:00:00 bash
  123 pts/0    00:00:00 ps
```

### Processus parent et enfant

Quand un processus en crée un autre :
- Le créateur = **parent**
- Le nouveau = **enfant** (child)
- L'enfant est une **copie exacte** du parent au moment de la création

```
Parent (PID 42, le shell)
    │
    │ fork()
    │
    ├── Enfant (PID 123, copie du shell)
    │       │
    │       │ execve("/bin/ls", ...)
    │       │    → l'enfant DEVIENT ls
    │       │
    │       └── ls s'exécute, affiche les fichiers, termine (exit 0)
    │
    │ waitpid(123, &status, 0)
    │    → le parent attend que l'enfant finisse
    │    → récupère le code de retour (0 = succès)
    │
    ▼
Parent continue (affiche le prompt)
```

### Les 3 étapes clés de l'exécution

1. **`fork()`** — Créer une copie du processus actuel
2. **`execve()`** — Dans l'enfant, remplacer le programme par celui qu'on veut exécuter
3. **`waitpid()`** — Dans le parent, attendre que l'enfant termine

> C'est le schéma fondamental. TOUT dans minishell repose sur ce triplet `fork` → `execve` → `waitpid`.

---

# PARTIE II — LES FONCTIONS AUTORISÉES

---

## 4. 🔧 Fonctions de gestion de processus

### `fork()` — Créer un processus enfant

```
#include <unistd.h>
pid_t fork(void);
```

**Ce qu'elle fait** : Crée une copie **exacte** du processus actuel. Après le `fork`, il y a **deux** processus identiques qui continuent au même endroit dans le code.

**Retour** :
- Dans le **parent** : retourne le PID de l'enfant (un nombre > 0)
- Dans l'**enfant** : retourne `0`
- En cas d'**erreur** : retourne `-1`

**Analogie** : Imagine que tu photographies un livre ouvert à la page 50. Maintenant il y a deux exemplaires du livre, les deux ouverts à la page 50. L'un est l'original (parent), l'autre la copie (enfant). Ils peuvent ensuite tourner des pages différentes.

**Exemple** :
```c
pid_t pid = fork();

if (pid == -1)
    // Erreur : fork a échoué (plus de mémoire, trop de processus)
if (pid == 0)
    // On est dans l'ENFANT
    // pid vaut 0 ici
if (pid > 0)
    // On est dans le PARENT
    // pid contient le PID de l'enfant (ex: 12345)
```

**Points importants** :
- Après `fork`, **les deux processus ont des copies séparées de toute la mémoire**. Si l'enfant modifie une variable, le parent ne le voit pas (et vice versa).
- Les **file descriptors** (fd) sont **partagés** entre parent et enfant au moment du fork. Si le parent avait le fd 3 ouvert, l'enfant l'a aussi.
- Le `fork` peut échouer si le système n'a plus de ressources. Il faut **toujours vérifier le retour**.

---

### `execve()` — Remplacer le programme actuel

```
#include <unistd.h>
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

**Ce qu'elle fait** : Remplace le programme du processus actuel par un autre programme. Le processus garde le même PID, mais le code, les données, la pile — tout est remplacé.

**Paramètres** :
- `pathname` : chemin **complet** du programme (`"/bin/ls"`, pas juste `"ls"`)
- `argv` : tableau d'arguments. `argv[0]` = nom du programme, terminé par `NULL`
- `envp` : tableau des variables d'environnement, terminé par `NULL`

**Retour** :
- **Ne retourne JAMAIS en cas de succès** (le programme a été remplacé, il n'y a plus de code pour "retourner")
- Retourne `-1` en cas d'erreur

**Analogie** : Un acteur sur scène. `fork` = l'acteur se dédouble (il y en a deux). `execve` = le double enlève son costume et en met un autre — il **devient** un personnage totalement différent. L'acteur original reste lui-même.

**Exemple** :
```c
// Dans le processus enfant, après fork :
char *args[] = {"/bin/ls", "-la", NULL};
char *env[] = {"PATH=/usr/bin", "HOME=/home/user", NULL};

execve("/bin/ls", args, env);
// Si on arrive ici, c'est que execve a échoué !
perror("execve");
exit(127);  // Convention : 127 = commande non trouvée
```

**Points importants** :
- Toujours appeler `execve` dans le **processus enfant** (après `fork`). Si tu l'appelles dans le parent, ton shell est remplacé — il disparaît !
- `argv[0]` est conventionnellement le nom du programme (mais le programme peut l'ignorer)
- Le dernier élément de `argv` et `envp` **DOIT** être `NULL`
- Si `execve` échoue (fichier n'existe pas, pas les droits), le **processus enfant continue**. Il faut donc faire `exit()` juste après pour ne pas avoir un processus zombie.

---

### `wait()` et `waitpid()` — Attendre un processus enfant

```
#include <sys/wait.h>
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

**Ce qu'elles font** : Le parent attend qu'un processus enfant se termine. Récupère le code de retour.

**Différence** :
- `wait()` : attend **n'importe lequel** des enfants
- `waitpid(pid, ...)` : attend un enfant **spécifique** (par son PID)

**Paramètres de waitpid** :
- `pid` : le PID de l'enfant à attendre. `-1` = n'importe lequel
- `status` : un pointeur vers un int où sera stocké le code de retour
- `options` : `0` = bloquant (attend jusqu'à la fin), `WNOHANG` = non-bloquant (revient immédiatement)

**Décoder le status** :
```c
int status;
waitpid(pid, &status, 0);

if (WIFEXITED(status))           // L'enfant s'est terminé normalement ?
    int exit_code = WEXITSTATUS(status);  // → le code de retour (0-255)

if (WIFSIGNALED(status))         // L'enfant a été tué par un signal ?
    int signal = WTERMSIG(status);        // → quel signal l'a tué
```

**Analogie** : Le manager (parent) envoie un employé (enfant) faire une tâche. `waitpid` = le manager s'assoit et attend que l'employé revienne avec le résultat. `WIFEXITED` = "l'employé a-t-il fini normalement ?". `WEXITSTATUS` = "quel est le résultat ?" (0 = succès, autre = problème).

**Points importants** :
- Si tu ne fais pas `wait`/`waitpid` pour un enfant terminé, il devient un **zombie** (il reste dans la table des processus, prend des ressources).
- Avec les pipes, tu as **plusieurs enfants**. Tu dois attendre **tous** les enfants.
- Le `$?` de minishell doit refléter le code du **dernier processus de la pipeline** (le plus à droite).

---

## 5. 📂 Fonctions de fichiers et file descriptors

### Qu'est-ce qu'un file descriptor (fd) ?

Un **file descriptor** est un numéro entier que le système utilise pour identifier un fichier ouvert, un pipe, un socket, etc.

**Analogie** : Pense aux fd comme des **numéros de vestiaire**. Quand tu déposes ton manteau (ouvres un fichier), on te donne un numéro (le fd). Quand tu veux récupérer ton manteau (lire/écrire dans le fichier), tu donnes le numéro. Quand tu pars (fermes le fichier), tu rends le numéro.

**Les 3 fd standard** (toujours ouverts par défaut) :

| fd | Nom | Constante | Description |
|----|-----|-----------|-------------|
| 0 | stdin | `STDIN_FILENO` | Entrée standard (clavier) |
| 1 | stdout | `STDOUT_FILENO` | Sortie standard (écran) |
| 2 | stderr | `STDERR_FILENO` | Sortie erreur (écran aussi, mais séparée) |

```
          ┌────────────────────┐
Clavier → │ fd 0 (stdin)       │
          │                    │
          │    Processus       │ → fd 1 (stdout) → Écran
          │                    │
          │                    │ → fd 2 (stderr) → Écran
          └────────────────────┘
```

Les redirections, c'est simplement **changer où pointent ces fd**. `> fichier` = faire pointer fd 1 vers un fichier au lieu de l'écran.

---

### `open()` — Ouvrir un fichier

```
#include <fcntl.h>
int open(const char *pathname, int flags, ... /* mode_t mode */);
```

**Ce qu'elle fait** : Ouvre un fichier et retourne un **fd** (le plus petit numéro disponible).

**Flags courants** (combinables avec `|`) :

| Flag | Effet |
|------|-------|
| `O_RDONLY` | Ouvrir en lecture seule |
| `O_WRONLY` | Ouvrir en écriture seule |
| `O_RDWR` | Ouvrir en lecture et écriture |
| `O_CREAT` | Créer le fichier s'il n'existe pas |
| `O_TRUNC` | Vider le fichier s'il existe (= `>`) |
| `O_APPEND` | Écrire à la fin du fichier (= `>>`) |

**mode** (3ème argument, seulement avec `O_CREAT`) : les permissions du fichier. Typiquement `0644` (lecture/écriture pour le propriétaire, lecture pour les autres).

**Correspondance avec les redirections shell** :

| Redirection | Équivalent open() |
|-------------|-------------------|
| `< fichier` | `open("fichier", O_RDONLY)` |
| `> fichier` | `open("fichier", O_WRONLY \| O_CREAT \| O_TRUNC, 0644)` |
| `>> fichier` | `open("fichier", O_WRONLY \| O_CREAT \| O_APPEND, 0644)` |

**Retour** :
- Succès : un fd (nombre ≥ 0, typiquement ≥ 3 car 0, 1, 2 sont pris)
- Erreur : `-1` (fichier n'existe pas, pas les droits, etc.)

---

### `close()` — Fermer un file descriptor

```
#include <unistd.h>
int close(int fd);
```

**Ce qu'elle fait** : Ferme un fd. Le numéro redevient disponible pour un futur `open` ou `pipe`.

**Point important** : Si tu ne fermes pas les fd, tu as une **fuite de file descriptors**. C'est comme une fuite mémoire mais pour les fd. Le système a une limite (~1024 fd par processus). Avec les pipes, si tu ne fermes pas les fd inutilisés, les processus peuvent se **bloquer** (attendre indéfiniment des données qui ne viendront jamais).

---

### `read()` et `write()` — Lire et écrire

```
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

**Ce qu'elles font** :
- `read` : lit jusqu'à `count` octets depuis `fd` dans le buffer `buf`
- `write` : écrit `count` octets de `buf` vers `fd`

**Retour** : le nombre d'octets effectivement lus/écrits, 0 (fin de fichier pour read), ou -1 (erreur).

---

### `dup2()` — Dupliquer un fd ★★★

```
#include <unistd.h>
int dup2(int oldfd, int newfd);
```

**Ce qu'elle fait** : Fait en sorte que `newfd` pointe vers la même chose que `oldfd`. Si `newfd` était déjà ouvert, il est d'abord fermé.

**C'est LA fonction centrale des redirections et des pipes.**

**Analogie** : Imagine que le fd est une étiquette sur un tuyau. `dup2(5, 1)` signifie : "prends l'étiquette 1 (stdout), enlève-la du tuyau où elle est (l'écran), et colle-la sur le même tuyau que l'étiquette 5 (un fichier)". Maintenant, tout ce qui va dans stdout (fd 1) va dans le fichier.

**Exemples concrets** :

```c
// Redirection > fichier
int fd = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);   // stdout pointe maintenant vers outfile
close(fd);                   // on n'a plus besoin du fd original
// À partir d'ici, printf/write(1,...) écrit dans outfile au lieu de l'écran

// Redirection < fichier
int fd = open("infile", O_RDONLY);
dup2(fd, STDIN_FILENO);    // stdin pointe maintenant vers infile
close(fd);
// À partir d'ici, read(0,...)/scanf lit depuis infile au lieu du clavier
```

**Visualisation** :

Avant `dup2(fd, 1)` :
```
fd 0 ──→ clavier
fd 1 ──→ écran
fd 2 ──→ écran
fd 5 ──→ outfile
```

Après `dup2(5, 1)` puis `close(5)` :
```
fd 0 ──→ clavier
fd 1 ──→ outfile    ← stdout redirigé !
fd 2 ──→ écran
fd 5    (fermé)
```

**Points importants** :
- Toujours **fermer** le fd original après `dup2` (sinon on gaspille un fd)
- `dup2` est **atomique** (ferme newfd et duplique en une seule opération)
- Si `oldfd == newfd`, `dup2` ne fait rien (pas d'erreur)

---

### `access()` — Vérifier les droits d'accès

```
#include <unistd.h>
int access(const char *pathname, int mode);
```

**Ce qu'elle fait** : Vérifie si le processus actuel a les droits d'accès au fichier.

**Modes** :
- `F_OK` : le fichier existe-t-il ?
- `R_OK` : peut-on le lire ?
- `W_OK` : peut-on y écrire ?
- `X_OK` : peut-on l'exécuter ?

**Retour** : `0` si OK, `-1` si non.

Utile pour : vérifier qu'une commande existe et est exécutable avant d'essayer `execve`.

---

### `stat()` — Obtenir des informations sur un fichier

```
#include <sys/stat.h>
int stat(const char *pathname, struct stat *statbuf);
```

**Ce qu'elle fait** : Remplit une structure `stat` avec des infos sur le fichier (taille, permissions, type, dates...).

Utile pour : savoir si un chemin est un fichier régulier ou un répertoire (important pour `cd` et les commandes).

---

### `unlink()` — Supprimer un fichier

```
#include <unistd.h>
int unlink(const char *pathname);
```

**Ce qu'elle fait** : Supprime le fichier du système de fichiers.

Utile pour : supprimer les fichiers temporaires de heredoc si tu utilises des fichiers plutôt que des pipes.

---

## 6. 🔗 Fonctions de pipes

### `pipe()` — Créer un tube de communication

```
#include <unistd.h>
int pipe(int pipefd[2]);
```

**Ce qu'elle fait** : Crée un **tube** (pipe) avec deux extrémités :
- `pipefd[0]` = l'extrémité de **lecture** (on lit les données qui sortent du tube)
- `pipefd[1]` = l'extrémité d'**écriture** (on écrit des données dans le tube)

**Analogie complète** : Un pipe Unix, c'est comme un **tube pneumatique** dans un vieux bureau de poste :

```
Processus A                         Processus B
(écrit)                              (lit)

  "hello"                            
    │                                 
    ▼                                 
┌────────┐     ┌─────────────┐     ┌────────┐
│ fd[1]  │────▶│   TUBE      │────▶│ fd[0]  │
│(écrire)│     │ (mémoire    │     │ (lire) │
│        │     │  kernel)    │     │        │
└────────┘     └─────────────┘     └────────┘
                                      │
                                      ▼
                                   "hello"
```

- Ce qu'on écrit dans `fd[1]` ressort par `fd[0]`
- C'est **unidirectionnel** : on ne peut lire que par `fd[0]` et écrire que par `fd[1]`
- Le tube a une **capacité limitée** (~65536 octets). Si le tube est plein, `write` bloque. Si le tube est vide, `read` bloque.

**Retour** : `0` si OK, `-1` si erreur.

### Comment un pipe connecte deux commandes

Quand tu tapes `ls | grep .c`, voici ce qui se passe :

```
1. Le shell crée un pipe : pipe(pipefd)
   pipefd[0] = 3 (lecture)
   pipefd[1] = 4 (écriture)

2. Le shell fork() → enfant 1 (ls)
   Enfant 1 :
     dup2(pipefd[1], STDOUT)   // sa sortie → le tube
     close(pipefd[0])          // ne lit pas le tube
     close(pipefd[1])          // ferme la copie originale
     execve("/bin/ls", ...)

3. Le shell fork() → enfant 2 (grep)
   Enfant 2 :
     dup2(pipefd[0], STDIN)    // son entrée ← le tube  
     close(pipefd[0])          // ferme la copie originale
     close(pipefd[1])          // n'écrit pas dans le tube
     execve("/bin/grep", ...)

4. Le parent ferme les deux bouts du pipe :
     close(pipefd[0])
     close(pipefd[1])
   
5. Le parent attend les deux enfants :
     waitpid(pid1, ...)
     waitpid(pid2, ...)
```

**Visualisation** :
```
                    pipe
           ┌──────────────────┐
ls         │                  │         grep
┌─────┐    │   pipefd[0]=3    │    ┌─────────┐
│stdout├───▶   pipefd[1]=4    ├───▶│stdin    │
│(fd 1)│    │                  │    │(fd 0)   │
└─────┘    └──────────────────┘    └─────────┘
```

### Règle d'or des pipes : FERMER ce qu'on n'utilise pas

C'est la source de **90% des bugs** de pipes dans minishell.

**Pourquoi ?** Si `pipefd[1]` (écriture) n'est pas fermé dans le processus qui lit (grep), alors `read` ne recevra **jamais** EOF. Le processus lecteur bloquera **indéfiniment**, car le système voit qu'il y a encore un fd ouvert en écriture sur le pipe, donc des données **pourraient** encore arriver.

| Processus | Doit fermer |
|-----------|-------------|
| Enfant 1 (ls, le producteur) | `pipefd[0]` (il n'en a pas besoin — il ne lit pas) |
| Enfant 2 (grep, le consommateur) | `pipefd[1]` (il n'en a pas besoin — il n'écrit pas) |
| Parent (le shell) | `pipefd[0]` ET `pipefd[1]` (il ne les utilise pas) |

### Pipes multiples : `ls | grep .c | wc -l`

Pour N commandes, il faut N-1 pipes :

```
3 commandes → 2 pipes

pipe1                pipe2
┌──────┐   ┌──────┐   ┌──────┐
│  ls  │──▶│ grep │──▶│  wc  │
│      │   │  .c  │   │  -l  │
└──────┘   └──────┘   └──────┘

pipe1 connecte ls → grep
pipe2 connecte grep → wc
```

Le processus du milieu (grep) lit depuis pipe1 et écrit dans pipe2. C'est le seul qui a besoin d'interagir avec deux pipes.

---

## 7. 📡 Fonctions de signaux

### `signal()` — Associer un handler à un signal (simple)

```
#include <signal.h>
sighandler_t signal(int signum, sighandler_t handler);
```

**Ce qu'elle fait** : Associe une fonction (handler) à un signal. Quand le signal est reçu, la fonction est appelée.

**Valeurs spéciales pour handler** :
- `SIG_DFL` : comportement **par défaut** du signal
- `SIG_IGN` : **ignorer** le signal

Exemple :
```c
signal(SIGINT, SIG_IGN);     // ctrl-C est ignoré
signal(SIGQUIT, SIG_DFL);    // ctrl-\ a le comportement par défaut
signal(SIGINT, my_handler);  // ctrl-C appelle my_handler
```

---

### `sigaction()` — Associer un handler (avancé, recommandé)

```
#include <signal.h>
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

**Ce qu'elle fait** : Comme `signal()`, mais plus fiable et avec plus d'options.

**Pourquoi préférer sigaction** :
- `signal()` peut avoir des comportements différents entre les systèmes
- `sigaction()` est standardisé POSIX et fiable
- Permet de spécifier des flags (comme `SA_RESTART` pour relancer les appels système interrompus)

**Exemple** :
```c
struct sigaction sa;

sa.sa_handler = my_handler;      // la fonction à appeler
sigemptyset(&sa.sa_mask);         // pas de signaux bloqués pendant le handler
sa.sa_flags = SA_RESTART;         // relancer les appels système interrompus

sigaction(SIGINT, &sa, NULL);     // appliquer pour ctrl-C
```

---

### `kill()` — Envoyer un signal à un processus

```
#include <signal.h>
int kill(pid_t pid, int sig);
```

**Ce qu'elle fait** : Envoie un signal à un processus (ou groupe de processus).

Utile pour : envoyer des signaux aux processus enfants (par exemple pour les interrompre).

---

## 8. 🖥️ Fonctions de terminal

### `isatty()` — Vérifier si un fd est un terminal

```
#include <unistd.h>
int isatty(int fd);
```

**Retour** : `1` si `fd` est connecté à un terminal, `0` sinon.

Utile pour : savoir si minishell est lancé en mode interactif (terminal) ou en mode pipe/redirection.

---

### `ttyname()` — Obtenir le nom du terminal

```
#include <unistd.h>
char *ttyname(int fd);
```

**Retour** : le chemin du terminal (ex: `"/dev/pts/0"`) ou `NULL`.

---

### `tcgetattr()` et `tcsetattr()` — Configurer le terminal

```
#include <termios.h>
int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
```

**Ce qu'elles font** : Lire et modifier les paramètres du terminal (mode canonique, echo, signaux...).

**Quand les utiliser dans minishell** : Pour restaurer les attributs du terminal quand minishell quitte, surtout après que `readline` les a potentiellement modifiés. Si tu ne le fais pas, le terminal peut se retrouver dans un état bizarre après un crash.

---

### `ioctl()` — Contrôle d'entrées/sorties

```
#include <sys/ioctl.h>
int ioctl(int fd, unsigned long request, ...);
```

Rarement nécessaire dans minishell, mais autorisée. Permet des opérations de contrôle avancées sur les fd.

---

## 9. 🌍 Fonctions d'environnement et chemins

### `getenv()` — Lire une variable d'environnement

```
#include <stdlib.h>
char *getenv(const char *name);
```

**Ce qu'elle fait** : Retourne la **valeur** de la variable d'environnement `name`, ou `NULL` si elle n'existe pas.

**Exemple** :
```c
char *home = getenv("HOME");    // → "/home/user"
char *path = getenv("PATH");    // → "/usr/bin:/bin:/usr/local/bin"
char *nope = getenv("NEXISTE"); // → NULL
```

**Attention** : Dans minishell, tu gères ta **propre copie** de l'environnement. Tu n'utiliseras probablement pas `getenv` directement — tu fouilleras dans ta propre structure `t_env` à la place.

---

### `getcwd()` — Obtenir le répertoire courant

```
#include <unistd.h>
char *getcwd(char *buf, size_t size);
```

**Ce qu'elle fait** : Copie le chemin absolu du répertoire courant dans `buf`.

Si `buf` est `NULL` (sur Linux), elle alloue elle-même la mémoire (il faudra `free`).

Utile pour : le builtin `pwd` et pour mettre à jour `PWD` après un `cd`.

---

### `chdir()` — Changer de répertoire

```
#include <unistd.h>
int chdir(const char *path);
```

**Ce qu'elle fait** : Change le répertoire courant du processus.

**Retour** : `0` si OK, `-1` si erreur (répertoire n'existe pas, pas les droits, etc.)

**Point important** : `chdir` modifie le répertoire du **processus courant**. C'est pour ça que `cd` doit être un **builtin** (exécuté dans le processus du shell) et non une commande externe. Si tu faisais `fork` puis `chdir` dans l'enfant, seul l'enfant changerait de répertoire — le parent (le shell) resterait au même endroit.

---

### Résolution du PATH

Quand tu tapes `ls`, le shell ne sait pas que c'est `/bin/ls`. Il doit le chercher.

**L'algorithme** :
1. Si la commande contient un `/` (ex: `./script` ou `/bin/ls`), c'est un **chemin direct** → utiliser tel quel
2. Sinon, chercher dans les répertoires listés dans `$PATH`

**$PATH** est une liste de répertoires séparés par `:` :
```
/usr/local/bin:/usr/bin:/bin:/usr/sbin
```

**Le processus de recherche** :
```
Commande : "ls"
PATH = "/usr/local/bin:/usr/bin:/bin"

Essayer : /usr/local/bin/ls → existe ? → NON
Essayer : /usr/bin/ls       → existe ? → NON
Essayer : /bin/ls           → existe et exécutable ? → OUI → utiliser celui-ci

// Pour vérifier : access("/bin/ls", X_OK) == 0
```

**Cas spéciaux** :
- Si `$PATH` n'est pas défini → ne chercher que le répertoire courant (ou erreur)
- Si la commande commence par `./` → chemin relatif, ne pas chercher dans PATH
- Si la commande commence par `/` → chemin absolu, ne pas chercher dans PATH

---

## 10. 📖 Fonctions readline

### `readline()` — Lire une ligne avec édition

```
#include <readline/readline.h>
char *readline(const char *prompt);
```

**Ce qu'elle fait** :
1. Affiche le `prompt` (ex: `"minishell$ "`)
2. Lit une ligne depuis l'entrée standard avec édition (flèches, backspace, etc.)
3. Retourne la ligne **sans le `\n` final**, allouée avec `malloc`

**Retour** :
- La ligne saisie (à `free` après usage !)
- `NULL` si EOF (l'utilisateur a fait `ctrl-D`)

**Points importants** :
- `readline` **alloue** la mémoire. Tu dois la **libérer** avec `free()`.
- `readline` a ses propres fuites mémoire internes (tolérées par le sujet)
- `readline` modifie les attributs du terminal (il les restaure normalement, mais un crash peut laisser le terminal dans un état bizarre)
- Si l'utilisateur entre une ligne vide, `readline` retourne une chaîne vide `""` (pas `NULL`)
- `NULL` = l'utilisateur a fait `ctrl-D` (EOF), ce qui signifie "exit" pour le shell

---

### `add_history()` — Ajouter une commande à l'historique

```
#include <readline/history.h>
void add_history(const char *string);
```

**Ce qu'elle fait** : Ajoute `string` à l'historique des commandes. L'utilisateur peut ensuite la retrouver avec les flèches haut/bas.

**Conseil** : Ne pas ajouter les lignes vides à l'historique.

```c
char *line = readline("minishell$ ");
if (line && *line)          // si non-NULL et non-vide
    add_history(line);
```

---

### `rl_clear_history()` — Vider l'historique

```
void rl_clear_history(void);
```

Utile pour nettoyer à la fin du programme.

---

### `rl_on_new_line()` — Informer readline d'un saut de ligne

```
int rl_on_new_line(void);
```

**Quand l'utiliser** : Après avoir affiché un `\n` manuellement (par exemple après `ctrl-C`), pour dire à readline "on est sur une nouvelle ligne".

---

### `rl_replace_line()` — Remplacer le contenu de la ligne

```
void rl_replace_line(const char *text, int clear_undo);
```

**Quand l'utiliser** : Après `ctrl-C`, pour vider la ligne en cours :
```c
rl_replace_line("", 0);  // la ligne est maintenant vide
```

---

### `rl_redisplay()` — Réafficher le prompt

```
void rl_redisplay(void);
```

**Quand l'utiliser** : Après avoir modifié la ligne (avec `rl_replace_line`), pour mettre à jour l'affichage.

**Le combo pour ctrl-C dans le prompt** :
```c
// Dans le handler de SIGINT :
write(1, "\n", 1);          // Nouvelle ligne
rl_on_new_line();            // Dire à readline
rl_replace_line("", 0);     // Vider la ligne
rl_redisplay();              // Réafficher le prompt
```

---

## 11. 🧰 Autres fonctions utiles

### `perror()` — Afficher un message d'erreur

```
#include <stdio.h>
void perror(const char *s);
```

**Ce qu'elle fait** : Affiche `s`, un `:`, puis la description de la dernière erreur (basée sur `errno`).

Exemple : Si `open("fichier", O_RDONLY)` échoue avec "No such file or directory" :
```c
perror("minishell");  // Affiche : "minishell: No such file or directory"
```

---

### `strerror()` — Obtenir la description d'une erreur

```
#include <string.h>
char *strerror(int errnum);
```

**Ce qu'elle fait** : Retourne la description textuelle de l'erreur numéro `errnum`.

```c
#include <errno.h>
char *msg = strerror(errno);  // → "No such file or directory" (par ex.)
```

Utile pour construire des messages d'erreur personnalisés :
```c
fprintf(stderr, "minishell: %s: %s\n", filename, strerror(errno));
```

---

### `errno` — La variable d'erreur globale

```
#include <errno.h>
```

`errno` est une variable globale qui contient le code d'erreur de la **dernière** fonction système qui a échoué.

**Important** : `errno` n'est fiable que **juste après** un échec. Il peut être modifié par n'importe quel appel système suivant.

Valeurs courantes :
| errno | Constante | Signification |
|-------|-----------|---------------|
| 2 | `ENOENT` | No such file or directory |
| 13 | `EACCES` | Permission denied |
| 12 | `ENOMEM` | Out of memory |

---

### `exit()` — Terminer le processus

```
#include <stdlib.h>
void exit(int status);
```

**Ce qu'elle fait** : Termine le processus avec le code de retour `status`.

**Codes conventionnels** :
- `0` = succès
- `1` = erreur générale
- `2` = erreur d'utilisation/syntaxe
- `126` = commande trouvée mais pas exécutable
- `127` = commande non trouvée
- `128 + N` = terminé par le signal N (ex: `128 + 2 = 130` pour SIGINT)

---

### `malloc()`, `free()` — Gestion mémoire dynamique

Tu connais déjà, mais un rappel important pour minishell :

- **Chaque `malloc` a besoin d'un `free`**
- Si un `malloc` échoue (retourne `NULL`), il faut **tout nettoyer et quitter proprement** (ou au minimum gérer l'erreur)
- Les **fuites de readline** sont tolérées. Toutes les autres fuites = 0 au projet.

---

# PARTIE III — LE PIPELINE DE TRAITEMENT

---

## 12. 🔪 Le Lexer (Tokenisation)

### Qu'est-ce que le lexer ?

Le **lexer** (ou tokeniser) est la première étape du traitement. Il prend une **chaîne de caractères brute** et la découpe en **tokens** (jetons).

**Analogie** : Pense à un caissier de supermarché. Les produits arrivent en vrac sur le tapis (la chaîne de caractères). Le caissier les identifie un par un et scanne chacun (les tokens) : "pomme", "lait", "beurre", "sac plastique". Il ne réfléchit pas à la recette qu'on va cuisiner — il identifie juste chaque élément.

### Types de tokens dans minishell

| Type | Symbole | Exemple dans la commande |
|------|---------|--------------------------|
| `WORD` | n'importe quel mot | `ls`, `-la`, `hello`, `file.txt` |
| `PIPE` | `\|` | `ls \| grep` |
| `REDIR_IN` | `<` | `< infile` |
| `REDIR_OUT` | `>` | `> outfile` |
| `APPEND` | `>>` | `>> outfile` |
| `HEREDOC` | `<<` | `<< EOF` |

### Exemple de tokenisation

```
Input : echo "hello world" | cat -e > out

Tokens :
[WORD:"echo"] → [WORD:"hello world"] → [PIPE:"|"] → [WORD:"cat"] → [WORD:"-e"] → [REDIR_OUT:">"] → [WORD:"out"]
```

Note que `"hello world"` est **un seul token** WORD (les quotes le protègent). Le lexer ne retire pas encore les quotes — il les utilise juste pour savoir où le token s'arrête.

### Comment fonctionne le lexer

Le lexer parcourt la chaîne caractère par caractère :

```
e c h o   " h e l l o   w o r l d "   |   c a t   - e   >   o u t
^
on commence ici
```

**Règles de découpage** :

1. **Espaces/tabs** : délimitent les tokens, mais ne sont pas des tokens eux-mêmes. On les saute.

2. **Opérateurs** (`|`, `<`, `>`, `>>`, `<<`) : ce sont des tokens à part entière. Dès qu'on voit un `|`, on crée un token `PIPE`.

3. **`>`** vs **`>>`** : Si on voit `>`, on regarde le caractère suivant. Si c'est aussi `>`, c'est un `APPEND` (token `>>`). Sinon c'est un `REDIR_OUT` (token `>`). Pareil pour `<` et `<<`.

4. **Mots (WORD)** : tout ce qui n'est pas un espace ou un opérateur. On avance jusqu'au prochain espace ou opérateur.

5. **Quotes** : quand on rencontre une quote (`'` ou `"`), on avance jusqu'à la quote fermante correspondante. Tout ce qui est à l'intérieur fait partie du même token, **y compris les espaces et les opérateurs**.

### Le piège des quotes dans le lexer

```
echo "hello|world" 'test > file'
```

Les `|` et `>` à l'intérieur des quotes ne sont **PAS** des opérateurs ! Ce sont des caractères ordinaires. Le token `"hello|world"` est un seul WORD qui contient un `|` littéral.

**Règle** : Quand tu es à l'intérieur de quotes, les seuls caractères spéciaux sont :
- Dans `'...'` (single quotes) : **rien n'est spécial** sauf la `'` fermante
- Dans `"..."` (double quotes) : **seuls `$` et `"` fermante** sont spéciaux

### Quotes non fermées

```
echo "hello
```

Bash attend la suite sur plusieurs lignes (affiche `> ` comme prompt secondaire). Dans **minishell**, le sujet dit généralement de NE PAS gérer les quotes non fermées (afficher une erreur de syntaxe).

### Construction de la liste chaînée de tokens

Le résultat du lexer est une **liste chaînée** de tokens :

```
HEAD → [WORD:"echo"] → [WORD:"hello world"] → [PIPE:"|"] → [WORD:"cat"] → NULL
```

Chaque maillon contient :
- `value` : la chaîne de caractères du token (ex: `"echo"`)
- `type` : le type du token (ex: `TOKEN_WORD`)
- `next` : pointeur vers le token suivant

---

## 13. 🌳 Le Parser (Analyse syntaxique)

### Qu'est-ce que le parser ?

Le **parser** prend la liste de tokens du lexer et la transforme en une **structure de commandes** que l'executor pourra utiliser.

**Analogie** : Si le lexer est le caissier qui identifie les produits, le parser est le chef cuisinier qui les organise en plats. Il les regroupe logiquement : "ces ingrédients vont ensemble pour faire une salade", "ceux-là pour le plat principal". Il détecte aussi les erreurs : "il manque un ingrédient" ou "on ne peut pas mettre de la viande dans le dessert".

### Ce que fait le parser concrètement

Il transforme :
```
[WORD:"echo"] → [WORD:"hello"] → [PIPE:"|"] → [WORD:"cat"] → [REDIR_OUT:">"] → [WORD:"out"]
```

En :
```
Commande 1 :
  args = ["echo", "hello"]
  redirs = (aucune)
  next → Commande 2

Commande 2 :
  args = ["cat"]
  redirs = [> "out"]
  next → NULL
```

### Ce que le parser détecte

**Erreurs de syntaxe** :

| Input | Erreur | Message bash |
|-------|--------|-------------|
| `\| ls` | Pipe en début | `syntax error near unexpected token '\|'` |
| `ls \|` | Pipe en fin (rien après) | `syntax error near unexpected token '\|'` |
| `ls \| \| grep` | Double pipe | `syntax error near unexpected token '\|'` |
| `>` | Redirection sans fichier | `syntax error near unexpected token 'newline'` |
| `ls > > file` | Deux redirections collées | `syntax error near unexpected token '>'` |
| `<< \|` | Heredoc suivi d'un pipe au lieu d'un delimiter | `syntax error near unexpected token '\|'` |

**Règle générale** : après un opérateur (`|`, `>`, `<`, `>>`, `<<`), on doit trouver un `WORD`. Après un `|`, on doit aussi trouver une commande (au moins un WORD ou une redirection).

### La structure de commandes

Le parser produit une **liste chaînée de commandes** :

```
echo hello | grep h | wc -l > count

┌─────────────────────┐    ┌──────────────────────┐    ┌─────────────────────────┐
│ Cmd 1               │    │ Cmd 2                │    │ Cmd 3                   │
│ args: ["echo","hi"] │───▶│ args: ["grep","h"]   │───▶│ args: ["wc","-l"]       │
│ redirs: NULL        │    │ redirs: NULL          │    │ redirs: [> "count"]     │
└─────────────────────┘    └──────────────────────┘    └─────────────────────────┘
```

Chaque commande contient :
- **args** : le tableau d'arguments (`args[0]` = la commande elle-même)
- **redirs** : la liste des redirections attachées à cette commande
- **next** : la commande suivante (séparée par un pipe)

### Les redirections dans le parser

Les redirections peuvent apparaître **n'importe où** dans la commande :

```
> out echo hello < in      # C'est valide !
```

Équivaut à :
```
echo hello < in > out
```

Le parser doit les **extraire** de la liste des tokens et les attacher à la commande, tout en ne les incluant **pas** dans les args :

```
Tokens : [REDIR_OUT] [WORD:"out"] [WORD:"echo"] [WORD:"hello"] [REDIR_IN] [WORD:"in"]

Résultat :
  args = ["echo", "hello"]          ← les WORD qui ne suivent pas une redir
  redirs = [> "out"] → [< "in"]    ← les redirections
```

---

## 14. 💲 L'Expander (Expansion des variables)

### Qu'est-ce que l'expansion ?

L'**expansion** remplace les variables (`$VAR`) par leur valeur dans l'environnement.

**Analogie** : Imagine un formulaire administratif avec des champs à remplir : "Nom : `$NOM`", "Ville : `$VILLE`". L'expander, c'est la personne qui prend le formulaire et remplace `$NOM` par "Dupont" et `$VILLE` par "Paris".

### Ce qui est étendu

```
$HOME       → /home/user
$USER       → gchaillo
$?          → 0   (code de retour de la dernière commande)
$PATH       → /usr/bin:/bin
$VIDE       → ""  (variable qui vaut une chaîne vide)
$NEXISTE    → ""  (variable non définie = chaîne vide)
```

### Règles d'expansion

#### 1. Dans quel contexte ?

| Contexte | Expansion ? | Exemple |
|----------|-------------|---------|
| Sans quotes | ✅ Oui | `echo $USER` → `echo gchaillo` |
| Double quotes `"..."` | ✅ Oui | `echo "$USER"` → `echo "gchaillo"` |
| Single quotes `'...'` | ❌ Non | `echo '$USER'` → `echo $USER` (littéral) |

#### 2. Qu'est-ce qu'un nom de variable valide ?

Un nom de variable commence par une lettre ou `_`, suivi de lettres, chiffres ou `_`.

```
$USER       → valide (lettres)
$HOME123    → valide (lettres + chiffres)
$_private   → valide (underscore)
$123        → $123 : le shell interprète $1 (paramètre positionnel, pas dans minishell) puis "23"
$           → $ tout seul reste un $ littéral
$?          → cas spécial : le code de retour
```

**Comment déterminer où s'arrête le nom** :
- On commence après le `$`
- On avance tant que le caractère est alphanumérique ou `_`
- On s'arrête au premier caractère qui ne l'est pas

```
echo $USER_name.txt
      ^^^^^^^^^
      nom de variable = "USER_name"
                       ^
                       le . n'est pas valide → on s'arrête
```

#### 3. `$?` — Le cas spécial

`$?` est le **code de retour** de la dernière commande exécutée. Ce n'est pas une variable d'environnement — c'est stocké dans ta structure `t_mini`.

```bash
ls fichier_qui_existe
echo $?    # → 0 (succès)

ls fichier_qui_nexiste_pas
echo $?    # → 2 (erreur)
```

#### 4. `$` tout seul

Un `$` qui n'est suivi ni d'un nom de variable valide ni de `?` reste un `$` littéral.

```bash
echo $        # → affiche $
echo $ hello  # → affiche $ hello
echo "$"      # → affiche $
```

#### 5. Expansion et concaténation

```bash
echo "$USER est connecté"     # → "gchaillo est connecté"
echo "salut $USER!"           # → "salut gchaillo!" (le ! n'est pas dans le nom)
echo $USER$HOME               # → gchaillo/home/gchaillo (deux expansions concaténées)
echo "$USER"'$HOME'           # → gchaillo$HOME (une expandée, l'autre pas)
```

#### 6. Quand la variable n'existe pas ou est vide

```bash
unset NOTSET
echo "[$NOTSET]"   # → "[]" (remplacé par chaîne vide)
echo $NOTSET       # → (ligne vide, car le mot disparaît)
```

---

## 15. 🪡 Les Quotes (Guillemets)

### Les 3 modes de quoting

Les quotes modifient l'interprétation des caractères spéciaux.

**Analogie** : Imagine trois types d'enveloppes pour le courrier :
- **Pas d'enveloppe** (sans quotes) : tout le monde peut lire et modifier le message
- **Enveloppe transparente** (double quotes `"..."`) : on peut lire mais pas tout modifier — seul `$` est interprété
- **Enveloppe opaque** (single quotes `'...'`) : personne ne touche au message — tout est littéral

### Sans quotes

Les caractères suivants sont **spéciaux** :
- Espaces/tabs : séparent les mots
- `|` : pipe
- `<`, `>` : redirections
- `$` : expansion de variable
- `'` et `"` : début de quote

```bash
echo hello world      # 2 arguments : "hello" et "world"
echo hello|world      # erreur de syntaxe ou commande "world" inexistante
echo $USER            # → gchaillo (expansion)
```

### Single quotes `'...'`

**RIEN** n'est spécial à l'intérieur. Absolument rien.

```bash
echo 'hello world'    # 1 argument : "hello world"
echo '$USER'          # → $USER (littéral, pas d'expansion)
echo 'hello | world'  # 1 argument : "hello | world" (le | est littéral)
echo 'it'\''s'        # Pour inclure un ' dans des single quotes :
                       # fermer les quotes, ajouter \', rouvrir les quotes
                       # → it's
```

**Limitation** : On ne peut PAS mettre une single quote à l'intérieur de single quotes. C'est impossible. La seule solution est de fermer, échapper, rouvrir (comme ci-dessus — mais ce cas n'est probablement pas requis dans minishell).

### Double quotes `"..."`

Seuls `$` et `"` sont spéciaux à l'intérieur.

(Note : dans bash, `\` et `` ` `` sont aussi spéciaux dans les double quotes, mais **pas dans minishell** — le sujet n'exige que `$`)

```bash
echo "hello world"        # 1 argument : "hello world"
echo "hello | world"      # 1 argument : "hello | world" (| est littéral)
echo "$USER"              # → gchaillo (expansion fonctionne !)
echo "hello $USER"        # → hello gchaillo
echo "it's easy"          # → it's easy (single quote est littérale dans "")
```

### Concaténation de quotes

Différents types de quotes peuvent se **coller** pour former un seul mot :

```bash
echo "hello"'world'       # → helloworld (1 seul argument)
echo "hello "'world'" more"  # attention aux imbrications
echo "val=$USER"'$LITT'   # → val=gchaillo$LITT
```

Le lexer doit comprendre que les morceaux adjacents sans espace entre eux forment un **seul** token WORD.

### Le quote removal

Après l'expansion, les quotes sont **retirées** du résultat. Si tu tapes `echo "hello"`, la commande `echo` reçoit l'argument `hello` (sans les guillemets).

```
Input :           echo "hello" '$USER' world
Après expansion : echo "hello" '$USER' world    (pas de $ à expandre dans les '')
Après removal :   echo  hello   $USER  world    (3 arguments)
```

### Tableau récapitulatif

| Caractère | Sans quotes | Single quotes `'` | Double quotes `"` |
|-----------|-------------|-------------------|-------------------|
| Espace | Séparateur | Littéral | Littéral |
| `$` | Expansion | Littéral | Expansion |
| `\|` | Pipe | Littéral | Littéral |
| `<`, `>` | Redirection | Littéral | Littéral |
| `'` | Début de single quote | FIN de single quote | Littéral |
| `"` | Début de double quote | Littéral | FIN de double quote |

---

# PARTIE IV — L'EXÉCUTION

---

## 16. 🚀 Exécution d'une commande simple

### Le cas le plus basique : une seule commande

```bash
ls -la /home
```

**Étapes** :

```
1. Le parser a produit :
   cmd = { args: ["ls", "-la", "/home"], redirs: NULL, next: NULL }

2. Est-ce un builtin ? ("ls" n'est pas echo/cd/pwd/export/unset/env/exit)
   → Non, c'est une commande externe

3. fork() → création d'un processus enfant

4. Dans l'enfant :
   a. Chercher le chemin complet : /bin/ls (via PATH)
   b. Appliquer les redirections (pas de redirs ici)
   c. execve("/bin/ls", ["ls", "-la", "/home"], envp)
   d. Si execve échoue → perror + exit(127)

5. Dans le parent :
   a. waitpid(pid_enfant, &status, 0)
   b. Récupérer le code de retour → exit_status
```

### Exécution d'un builtin (sans pipe)

```bash
cd /home
```

Quand la commande est un builtin **et qu'il n'y a pas de pipe**, on l'exécute **directement dans le processus parent**, sans `fork`.

**Pourquoi ?** Parce que les builtins comme `cd`, `export`, `unset` modifient l'état du shell lui-même (répertoire courant, variables d'environnement). Si on les exécutait dans un enfant, les modifications seraient perdues quand l'enfant terminerait.

```
Est-ce un builtin ?
├── OUI et pas de pipe → exécuter dans le PARENT (pas de fork)
├── OUI et dans un pipe → exécuter dans un ENFANT (fork nécessaire)
└── NON → exécuter dans un ENFANT (fork + execve)
```

### Exécution avec redirections

```bash
echo hello > outfile
```

Les redirections sont appliquées **dans le processus enfant**, **avant** l'appel à `execve` :

```
1. fork()

2. Dans l'enfant :
   a. Parcourir la liste des redirections :
      - Type = REDIR_OUT, fichier = "outfile"
      - fd = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0644)
      - dup2(fd, STDOUT_FILENO)  → stdout pointe vers outfile
      - close(fd)
   b. execve("echo", ...)  → la sortie de echo ira dans outfile

3. Dans le parent :
   waitpid(...)
```

**Ordre des redirections** : Les redirections sont appliquées **de gauche à droite**. Si il y a plusieurs redirections du même type, c'est la **dernière** qui gagne pour la lecture/écriture, mais les fichiers intermédiaires sont quand même ouverts/créés.

```bash
echo hello > file1 > file2 > file3
# file1 est créé (vide), file2 est créé (vide), "hello" est écrit dans file3
```

---

## 17. 🔗 Les pipes en détail

### Pipe simple : `cmd1 | cmd2`

**Ce qui se passe dans le shell** :

```
1. Le parser produit :
   cmd1 = { args: [...], next: cmd2 }
   cmd2 = { args: [...], next: NULL }

2. Compter les commandes : 2 → il faut 1 pipe

3. pipe(pipefd)  →  pipefd[0] = lecture, pipefd[1] = écriture

4. fork() → Enfant 1 (cmd1)
   Dans enfant 1 :
     dup2(pipefd[1], STDOUT)   // sortie → pipe
     close(pipefd[0])
     close(pipefd[1])
     execve(cmd1)

5. fork() → Enfant 2 (cmd2)
   Dans enfant 2 :
     dup2(pipefd[0], STDIN)    // entrée ← pipe
     close(pipefd[0])
     close(pipefd[1])
     execve(cmd2)

6. Dans le parent :
     close(pipefd[0])
     close(pipefd[1])
     waitpid(enfant1)
     waitpid(enfant2)
     exit_status = status de enfant2  ← le dernier !
```

### Pipes multiples : `cmd1 | cmd2 | cmd3`

```
2 pipes nécessaires :
pipe1 : cmd1 ──▶ cmd2
pipe2 : cmd2 ──▶ cmd3

Le processus cmd2 lit depuis pipe1[0] et écrit dans pipe2[1].
```

**Algorithme général pour N commandes** :

```
pour chaque commande i (de 0 à N-1) :
    si ce n'est PAS la dernière commande :
        créer un nouveau pipe
    
    fork() → enfant i
    
    dans l'enfant :
        si i > 0 (pas la première commande) :
            dup2(pipe_précédent[0], STDIN)     // lire depuis le pipe précédent
        si i < N-1 (pas la dernière commande) :
            dup2(pipe_courant[1], STDOUT)      // écrire dans le pipe courant
        fermer tous les fd de pipes
        appliquer les redirections propres à cette commande
        execve(...)
    
    dans le parent :
        fermer les bouts de pipe inutilisés
        si i > 0 :
            fermer pipe_précédent[0] et pipe_précédent[1]
        pipe_précédent = pipe_courant

attendre tous les enfants
exit_status = code de retour du DERNIER enfant
```

### Visualisation pour 3 commandes

```
    ┌──────────────────────────────────────────────────┐
    │                    PARENT                         │
    │                                                   │
    │  pipe1[0,1]           pipe2[0,1]                 │
    │      │                    │                       │
    │  ┌───┴────┐   ┌──────────┴──┐   ┌────────────┐  │
    │  │Enfant 1│   │  Enfant 2   │   │  Enfant 3  │  │
    │  │ cmd1   │   │   cmd2      │   │   cmd3     │  │
    │  │        │   │             │   │            │  │
    │  │ out=   │   │ in=pipe1[0] │   │ in=pipe2[0]│  │
    │  │pipe1[1]│   │ out=pipe2[1]│   │ out=stdout │  │
    │  └────────┘   └─────────────┘   └────────────┘  │
    └──────────────────────────────────────────────────┘
```

### Pourquoi le parent DOIT fermer les pipes

Le parent crée les pipes mais ne les utilise pas. S'il ne les ferme pas :
- Les enfants qui lisent (`read`) ne recevront jamais EOF
- Ils bloqueront indéfiniment
- Ton shell semble "gelé"

**Règle** : Dès que les enfants sont lancés, le parent ferme **tous** les fd de pipes.

---

## 18. ↔️ Les redirections

### Les 4 types de redirections

| Symbole | Nom | Direction | Effet |
|---------|-----|-----------|-------|
| `<` | Input | Entrée | Lire depuis un fichier au lieu du clavier |
| `>` | Output | Sortie | Écrire dans un fichier au lieu de l'écran (crée/écrase) |
| `>>` | Append | Sortie | Écrire à la fin d'un fichier (crée/ajoute) |
| `<<` | Heredoc | Entrée | Lire depuis l'input jusqu'à un délimiteur |

### `< fichier` — Redirection d'entrée

```bash
cat < mon_fichier.txt
# Au lieu de lire depuis le clavier, cat lit depuis mon_fichier.txt
```

**Implémentation** :
```c
int fd = open("mon_fichier.txt", O_RDONLY);
if (fd == -1)
    // erreur : fichier n'existe pas
dup2(fd, STDIN_FILENO);
close(fd);
```

**Erreur courante** : si le fichier n'existe pas, il faut afficher une erreur et NE PAS exécuter la commande (retourner exit status 1).

### `> fichier` — Redirection de sortie

```bash
echo hello > outfile
# Le texte "hello" est écrit dans outfile au lieu de l'écran
# Si outfile existe, il est VIDÉ puis écrit
# Si outfile n'existe pas, il est CRÉÉ
```

**Implémentation** :
```c
int fd = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

### `>> fichier` — Append (ajout)

```bash
echo hello >> outfile
# Comme > mais AJOUTE à la fin du fichier au lieu de le vider
```

**Implémentation** :
```c
int fd = open("outfile", O_WRONLY | O_CREAT | O_APPEND, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

### Redirections multiples

```bash
cmd < in1 < in2 > out1 > out2
```

Les redirections sont appliquées **de gauche à droite** :
1. stdin ← in1 (ouvert, dup2)
2. stdin ← in2 (ré-ouvert, dup2 → in2 remplace in1 pour la lecture)
3. stdout → out1 (créé/vidé, dup2)
4. stdout → out2 (créé/vidé, dup2 → out2 remplace out1 pour l'écriture)

Résultat : cmd lit depuis **in2** et écrit dans **out2**. Mais **out1 est quand même créé** (et vidé).

### Redirections + Pipes

```bash
echo hello > out | cat
# L'output de echo va dans "out", PAS dans le pipe
# cat ne reçoit rien (son stdin est vide)
```

Les redirections ont la **priorité** sur les pipes.

```bash
cat < infile | grep hello > outfile
# cat lit depuis "infile" (pas depuis stdin)
# grep écrit dans "outfile" (pas vers stdout)
# Le pipe connecte la sortie de cat à l'entrée de grep
# Mais la redir "< infile" override l'entrée de cat
# Et la redir "> outfile" override la sortie de grep
```

**Ordre d'application** : D'abord les pipes (dup2 pour les pipes), puis les redirections (dup2 pour les redirs). Les redirections écrasent ce que les pipes ont configuré.

### Erreur de redirection

Si une redirection échoue (fichier inaccessible, droits insuffisants), la commande **n'est pas exécutée** :

```bash
cat < fichier_inexistant
# bash: fichier_inexistant: No such file or directory
# $? = 1 (exit status)
# cat n'est jamais lancé
```

---

## 19. 📝 Le heredoc

### Qu'est-ce qu'un heredoc ?

Le **heredoc** (`<<`) est une redirection d'entrée spéciale. Au lieu de lire depuis un fichier, il lit l'input de l'utilisateur **ligne par ligne** jusqu'à rencontrer un **délimiteur** (un mot que tu choisis).

**Analogie** : Imagine que tu dictes une lettre à un secrétaire. Tu lui dis : "Écris tout ce que je dis jusqu'à ce que je dise le mot STOP". Le secrétaire écrit tout, et quand tu dis "STOP", il arrête et te donne la lettre. Le mot "STOP", c'est le délimiteur.

### Syntaxe

```bash
cat << EOF
Bonjour
Je suis un texte
sur plusieurs lignes
EOF
```

Output :
```
Bonjour
Je suis un texte
sur plusieurs lignes
```

Le mot `EOF` (ou n'importe quel mot choisi) sert de **délimiteur**. Quand le shell le rencontre **seul sur une ligne**, il arrête de lire.

### Expansion dans le heredoc

**Si le délimiteur n'est PAS entre quotes** : l'expansion `$VAR` fonctionne à l'intérieur du heredoc.

```bash
cat << EOF
Hello $USER
EOF
# → Hello gchaillo
```

**Si le délimiteur EST entre quotes** (n'importe quel type) : **aucune** expansion.

```bash
cat << 'EOF'
Hello $USER
EOF
# → Hello $USER (littéral)

cat << "EOF"
Hello $USER
EOF
# → Hello $USER (littéral aussi — les quotes désactivent l'expansion du contenu)
```

Note : les quotes sont retirées du délimiteur lui-même. Il faut chercher `EOF` (sans quotes) pour la ligne de fin.

### Comment implémenter un heredoc

**Méthode recommandée** : utiliser un **pipe**.

```
1. Créer un pipe : pipe(pipefd)
2. Lire l'input ligne par ligne (readline ou get_next_line depuis stdin)
3. Comparer chaque ligne au délimiteur
4. Si pas le délimiteur → écrire la ligne dans pipefd[1]
   (avec expansion de $VAR si le délimiteur n'est pas quoté)
5. Si c'est le délimiteur → arrêter
6. Fermer pipefd[1]
7. Utiliser pipefd[0] comme stdin pour la commande (dup2)
```

### Quand traiter les heredocs

**POINT CRITIQUE** : Les heredocs doivent être lus pendant le **parsing**, AVANT l'exécution.

Pourquoi ? Parce que si tu as :
```bash
cat << EOF | grep hello
hello world
bonjour
EOF
```

Le shell doit d'abord lire TOUT le heredoc, puis lancer la pipeline `cat | grep`. Si tu essaies de lire le heredoc pendant l'exécution dans un processus enfant, tu auras des problèmes de stdin (le parent et l'enfant se battent pour lire l'input).

### Heredocs multiples

```bash
cat << A << B
contenu A
A
contenu B
B
```

Les heredocs sont lus dans l'ordre (de gauche à droite). Le **dernier** heredoc est celui qui est effectivement connecté à stdin.

### Signaux et heredoc

- `ctrl-C` pendant un heredoc : interrompt le heredoc, retourne au prompt. `$?` = 130.
- `ctrl-D` pendant un heredoc (EOF sans trouver le délimiteur) : 
  ```
  bash: warning: here-document delimited by end-of-file (wanted `EOF')
  ```
  Le heredoc est fermé avec ce qui a été saisi.

---

# PARTIE V — LES BUILTINS

---

## 20. 🏠 Les builtins — Vue d'ensemble

### Pourquoi des builtins ?

Certaines commandes **doivent** être exécutées dans le processus du shell lui-même et non dans un enfant. La raison principale : elles modifient l'état interne du shell.

| Commande | Pourquoi builtin ? |
|----------|-------------------|
| `cd` | Doit changer le répertoire du shell (si dans un enfant, seul l'enfant change) |
| `export` | Doit modifier l'environnement du shell |
| `unset` | Doit supprimer des variables de l'environnement du shell |
| `exit` | Doit terminer le shell lui-même |
| `echo` | Pas strictement nécessaire, mais c'est le comportement de bash |
| `pwd` | Pas strictement nécessaire, mais c'est le comportement de bash |
| `env` | Pas strictement nécessaire, mais c'est le comportement de bash |

### Builtins et pipes

**Cas spécial** : quand un builtin est dans un **pipe**, il doit être exécuté dans un **fork** (processus enfant).

```bash
export A=1 | cat     # export est dans un pipe → fork
                     # CONSÉQUENCE : A n'est PAS ajouté à l'env du shell !
```

Seul le builtin **sans pipe** modifie l'état du shell. Les builtins dans un pipe n'ont aucun effet sur le shell parent.

**Règle** :
```
si (nombre de commandes == 1 ET c'est un builtin)
    → exécuter dans le parent (PAS de fork)
sinon
    → exécuter dans un enfant (fork, même si c'est un builtin)
```

---

## 21. 📢 echo

### Comportement

`echo` affiche ses arguments séparés par des espaces, suivis d'un `\n`.

```bash
echo hello world     # → "hello world\n"
echo                 # → "\n" (juste un saut de ligne)
echo -n hello        # → "hello" (PAS de \n à la fin)
```

### L'option `-n`

`-n` supprime le saut de ligne final. C'est la **seule** option que minishell doit gérer.

**Piège** : Plusieurs `-n` sont valides, et tout argument composé uniquement de `n` après le tiret est un flag `-n` :

```bash
echo -n hello         # → "hello" (pas de \n)
echo -n -n hello      # → "hello" (pas de \n)
echo -nnnn hello      # → "hello" (pas de \n)
echo -nnnm hello      # → "-nnnm hello\n" (le 'm' casse le flag)
echo -n -n -n         # → "" (rien, même pas un \n)
echo -n               # → "" (rien)
```

**Algorithme** :
```
flag_n = faux
pour chaque argument en partant du premier :
    si l'argument commence par '-' ET le reste ne contient que des 'n' :
        flag_n = vrai
        continuer au prochain argument
    sinon :
        arrêter de chercher des flags (c'est le début du texte)

afficher les arguments restants séparés par des espaces
si flag_n est faux :
    afficher '\n'
```

### Exit status

`echo` retourne toujours `0` (sauf si erreur d'écriture, ce qui est rare).

---

## 22. 📁 cd

### Comportement

`cd` change le répertoire courant du shell.

```bash
cd /home          # va dans /home
cd ..             # remonte d'un cran
cd                # va dans $HOME
cd -              # retourne au répertoire précédent (OLDPWD) — pas obligatoire dans minishell
```

### Implémentation

```
1. Récupérer le chemin cible :
   - cd sans argument → chemin = valeur de $HOME
   - cd avec argument → chemin = l'argument

2. Sauvegarder le répertoire actuel :
   old_pwd = getcwd(NULL, 0)

3. Appeler chdir(chemin)
   - Si échec → erreur "minishell: cd: <chemin>: No such file or directory"

4. Si succès :
   - Mettre à jour $OLDPWD = old_pwd
   - Mettre à jour $PWD = getcwd(NULL, 0)
```

### Cas limites

```bash
cd                  # Si $HOME n'est pas défini → "minishell: cd: HOME not set"
cd a b              # Trop d'arguments → "minishell: cd: too many arguments" (exit status 1)
cd ""               # Chaîne vide → "minishell: cd: : No such file or directory"
cd dossier_protégé  # Pas les droits → "minishell: cd: <dossier>: Permission denied"
```

### Exit status

- `0` : succès
- `1` : erreur

---

## 23. 📍 pwd

### Comportement

`pwd` affiche le chemin absolu du répertoire courant.

```bash
pwd    # → /home/user/projets/minishell
```

### Implémentation

```
Utiliser getcwd(NULL, 0) pour obtenir le chemin
Afficher le résultat suivi de '\n'
```

C'est le builtin le plus simple.

### Exit status

- `0` : succès (toujours, sauf si getcwd échoue, ce qui est très rare)

---

## 24. 📤 export

### Comportement

`export` ajoute ou modifie des variables d'environnement. Sans argument, il les affiche.

### `export` sans argument

Affiche **toutes** les variables d'environnement, **triées par ordre alphabétique**, dans un format spécial :

```bash
export
# declare -x HOME="/home/user"
# declare -x LANG="en_US.UTF-8"
# declare -x PATH="/usr/bin:/bin"
# declare -x USER="gchaillo"
```

Le format est : `declare -x NOM="valeur"` (avec les guillemets autour de la valeur).

Si une variable a été exportée **sans valeur** (ex: `export A`), elle s'affiche sans le `=` :
```
declare -x A
```

### `export` avec arguments

```bash
export FOO=bar           # crée/modifie FOO avec valeur "bar"
export FOO="hello world" # crée/modifie FOO avec valeur "hello world"
export A=1 B=2 C=3       # peut créer/modifier plusieurs variables
export FOO               # crée FOO sans valeur (si elle n'existe pas déjà)
```

### Validation du nom

Un nom de variable valide commence par une lettre ou `_`, suivi de lettres, chiffres ou `_`.

```bash
export A=1        # OK
export _A=1       # OK
export A1=1       # OK
export 1A=1       # ERREUR : "minishell: export: `1A=1': not a valid identifier"
export A-B=1      # ERREUR : tiret invalide
export ""=1       # ERREUR
export =value     # ERREUR
```

### Exit status

- `0` : toutes les variables sont exportées avec succès
- `1` : au moins une erreur d'identifiant (mais les variables valides sont quand même ajoutées)

---

## 25. 🗑️ unset

### Comportement

`unset` supprime des variables d'environnement.

```bash
export FOO=bar
echo $FOO         # → bar
unset FOO
echo $FOO         # → (rien — chaîne vide)
```

### Implémentation

```
Pour chaque argument :
    Chercher la variable dans la liste d'environnement
    Si trouvée → la supprimer de la liste chaînée (free le nœud)
    Si pas trouvée → ne rien faire (ce n'est PAS une erreur)
```

### Cas limites

```bash
unset              # Sans argument → ne fait rien (pas d'erreur)
unset PATH         # Supprime PATH — les commandes externes ne fonctionnent plus !
unset HOME         # Supprime HOME — cd sans argument échoue
unset INEXISTANT   # Pas d'erreur
```

### Exit status

- `0` : toujours (même si la variable n'existe pas)

---

## 26. 🌿 env

### Comportement

`env` affiche toutes les variables d'environnement (celles avec une valeur).

```bash
env
# HOME=/home/user
# PATH=/usr/bin:/bin
# USER=gchaillo
# FOO=bar
```

**Différence avec `export` sans argument** :
- `env` affiche `NOM=valeur` (format simple)
- `export` affiche `declare -x NOM="valeur"` (format avec declare -x et guillemets)
- `env` n'affiche PAS les variables sans valeur
- `export` affiche les variables sans valeur (`declare -x A`)

### Exit status

- `0` : succès

---

## 27. 🚪 exit

### Comportement

`exit` termine le shell.

```bash
exit         # Quitte avec le code de retour de la dernière commande ($?)
exit 0       # Quitte avec le code 0
exit 42      # Quitte avec le code 42
exit 256     # Quitte avec 0 (256 % 256 = 0)
exit -1      # Quitte avec 255 (255 = -1 en unsigned 8 bits)
```

### Cas limites (très testés en correction !)

```bash
exit abc
# minishell: exit: abc: numeric argument required
# → QUITTE avec code 2 (ou 255 selon les implémentations)

exit 1 2
# minishell: exit: too many arguments
# → NE QUITTE PAS ! Retourne seulement exit status 1.

exit 1 abc
# minishell: exit: too many arguments  
# → NE QUITTE PAS !

exit abc 1
# minishell: exit: abc: numeric argument required
# → QUITTE avec code 2 (l'erreur numérique prend le dessus)
```

**Algorithme** :
```
1. Afficher "exit\n" sur stderr (ou stdout — les deux sont acceptés)

2. Si pas d'argument :
   → quitter avec $? (exit_status actuel)

3. Premier argument :
   Si ce n'est PAS un nombre valide :
     → "numeric argument required"
     → quitter avec 2

4. Si plus d'un argument :
   → "too many arguments"
   → NE PAS quitter, exit_status = 1

5. Sinon (un seul argument numérique) :
   → quitter avec (argument % 256)
     (pour avoir un code entre 0 et 255)
```

### Le code de retour est sur 8 bits

Le code de retour est un **unsigned 8 bits** (0-255). Tout nombre est pris modulo 256 :
- `exit 256` → code 0
- `exit 257` → code 1
- `exit -1` → code 255
- `exit 9999` → code 9999 % 256 = 15

---

# PARTIE VI — LES SIGNAUX

---

## 28. 📡 Les signaux Unix

### Qu'est-ce qu'un signal ?

Un **signal** est une notification envoyée à un processus pour lui dire que quelque chose s'est passé.

**Analogie** : Les signaux sont comme des **SMS d'urgence** envoyés à un employé. L'employé (processus) est en train de travailler, et il reçoit soudainement un message :
- `SIGINT` : "Arrête ce que tu fais" (ctrl-C)
- `SIGQUIT` : "Arrête et fais un rapport de crash" (ctrl-\)
- `SIGTERM` : "Tu es viré" (kill)
- `SIGKILL` : "Les gardes de sécurité t'escortent dehors" (kill -9, impossible à ignorer)

### Les signaux importants pour minishell

| Signal | Numéro | Raccourci | Comportement par défaut |
|--------|--------|-----------|------------------------|
| `SIGINT` | 2 | `ctrl-C` | Terminer le processus |
| `SIGQUIT` | 3 | `ctrl-\` | Terminer + core dump |
| `SIGTERM` | 15 | `kill pid` | Terminer le processus |
| `SIGKILL` | 9 | `kill -9 pid` | Terminer (impossible à capter) |
| `SIGPIPE` | 13 | — | Écriture dans un pipe fermé |
| `SIGTSTP` | 20 | `ctrl-Z` | Suspendre (pas dans minishell) |

### Le handler de signal

Un **handler** est une fonction que TU écris et qui est appelée quand le signal est reçu, à la place du comportement par défaut.

```c
void handler(int signum)
{
    // Cette fonction est appelée quand le signal est reçu
    // signum = le numéro du signal
}
```

### Contraintes dans un handler de signal

**ATTENTION** : Un handler de signal est appelé de manière **asynchrone** (n'importe quand, au milieu de n'importe quoi). C'est très dangereux.

**Fonctions sûres dans un handler** (dites "async-signal-safe") :
- `write()` (PAS `printf`, PAS `ft_putstr`)
- modifier une variable `volatile sig_atomic_t`
- `exit()` (mais pas `free` ou `malloc`)

**Fonctions INTERDITES dans un handler** :
- `malloc`, `free` (peuvent corrompre le heap)
- `printf`, `puts` (utilisent des buffers internes)
- `readline` (pas async-safe)

**La solution** :
```c
// Variable globale (la seule autorisée !)
volatile sig_atomic_t g_signal_received = 0;

void handler(int signum)
{
    g_signal_received = signum;   // On note juste quel signal a été reçu
}

// Puis dans la boucle principale, on vérifie :
if (g_signal_received == SIGINT)
{
    // Traiter ctrl-C ici, en toute sécurité
    g_signal_received = 0;
}
```

### `volatile sig_atomic_t` — Pourquoi ce type ?

- `volatile` : dit au compilateur "ne cache pas cette variable dans un registre, elle peut changer à tout moment" (parce qu'un signal peut l'interrompre)
- `sig_atomic_t` : un type qui peut être lu/écrit de manière **atomique** (en une seule instruction processeur, pas interruptible)

Sans `volatile`, le compilateur pourrait optimiser la variable et ne jamais la relire. Sans `sig_atomic_t`, la lecture pourrait être interrompue au milieu et donner une valeur corrompue.

---

## 29. 🎛️ Signaux dans minishell

### Les 3 contextes de signaux

Le comportement des signaux change selon **ce que fait minishell** à cet instant :

| Contexte | `ctrl-C` (SIGINT) | `ctrl-D` (EOF) | `ctrl-\` (SIGQUIT) |
|----------|-------------------|----------------|---------------------|
| **Prompt** (en train d'attendre l'input) | Nouvelle ligne, nouveau prompt, `$?=130` | Exit le shell | Rien (ignoré) |
| **Exécution** (une commande tourne) | Tue le processus enfant, `$?=130` | Rien (c'est un EOF, pas un signal) | Tue le processus enfant + "Quit (core dumped)", `$?=131` |
| **Heredoc** (en train de lire un heredoc) | Interrompt le heredoc, retour au prompt, `$?=130` | Ferme le heredoc avec un warning | Rien (ignoré) |

### Contexte 1 : Prompt (en attente d'input)

```
minishell$ _           ← curseur ici, on attend l'input
          [ctrl-C]
                       ← nouvelle ligne
minishell$ _           ← nouveau prompt, ligne vide
```

**Implémentation** :
```
SIGINT :
  - g_signal_received = SIGINT
  - Dans le main loop, après readline :
    → write "\n"
    → rl_on_new_line()
    → rl_replace_line("", 0)
    → rl_redisplay()
    → exit_status = 130

SIGQUIT :
  - signal(SIGQUIT, SIG_IGN)  // igonré au prompt

ctrl-D :
  - Ce n'est PAS un signal. C'est un EOF.
  - readline() retourne NULL
  - → afficher "exit" + quitter le shell
```

### Contexte 2 : Exécution (un processus enfant tourne)

```
minishell$ cat         ← cat est lancé, attend l'input
          [ctrl-C]     ← le signal est envoyé au processus cat
^C                     ← cat est tué
minishell$ _           ← retour au prompt, $? = 130
```

```
minishell$ cat
          [ctrl-\]     ← le signal SIGQUIT est envoyé à cat
^\Quit (core dumped)   ← cat est tué avec core dump
minishell$ _           ← $? = 131
```

**Implémentation** :
```
Pendant l'exécution :
  Parent :
    - SIGINT : le signal va au foreground process group (l'enfant le reçoit aussi)
    - Le parent doit IGNORER SIGINT dans son handler pour ne pas être tué lui-même
    - Après waitpid, si WIFSIGNALED → exit_status = 128 + WTERMSIG(status)
    
  Enfant :
    - Remettre SIGINT et SIGQUIT au comportement par défaut (SIG_DFL)
    - L'enfant sera tué normalement par le signal
```

### Contexte 3 : Heredoc

```
minishell$ cat << EOF
> hello                ← en train de saisir le heredoc
> [ctrl-C]             ← le heredoc est interrompu
minishell$ _           ← retour au prompt, $? = 130
```

Le heredoc est une phase délicate pour les signaux :
- `ctrl-C` doit interrompre la saisie du heredoc et revenir au prompt
- `ctrl-\` est ignoré
- `ctrl-D` ferme le heredoc (comme un EOF) avec un warning

### Comment changer de contexte

Tu dois changer les handlers de signaux selon la phase :

```
┌─────────────────────────────┐
│         PROMPT              │
│  SIGINT = handler_prompt    │
│  SIGQUIT = SIG_IGN          │
└────────────┬────────────────┘
             │ l'utilisateur tape une commande
             ▼
┌─────────────────────────────┐
│       EXÉCUTION             │
│  SIGINT = SIG_IGN (parent)  │
│  SIGQUIT = SIG_IGN (parent) │
│  ─────────────────────────  │
│  SIGINT = SIG_DFL (enfant)  │
│  SIGQUIT = SIG_DFL (enfant) │
└────────────┬────────────────┘
             │ la commande est finie
             ▼
┌─────────────────────────────┐
│         PROMPT              │ on revient au prompt
│  (on remet les handlers)    │
└─────────────────────────────┘
```

---

# PARTIE VII — GESTION MÉMOIRE ET ERREURS

---

## 30. 🧠 Stratégie de gestion mémoire

### Le principe du "free tout à chaque boucle"

Dans minishell, la boucle principale est :
```
tant que vrai :
    input = readline()
    tokens = lexer(input)
    cmds = parser(tokens)
    expander(cmds)
    executor(cmds)
    free(input)
    free_tokens(tokens)
    free_cmds(cmds)
```

À chaque itération, tu alloues de la mémoire pour l'input, les tokens, les commandes. Tu dois **tout libérer** avant la prochaine itération.

### Ce qui doit être alloué/libéré

| Donnée | Alloué quand | Libéré quand |
|--------|-------------|--------------|
| `input` (ligne de readline) | À chaque readline | Après traitement de la ligne |
| `t_token` (liste de tokens) | Par le lexer | Après le parsing (les tokens ne sont plus nécessaires) |
| `t_cmd` (liste de commandes) | Par le parser | Après l'exécution |
| `t_cmd->args` (tableaux d'args) | Par le parser | Avec la commande |
| `t_redir` (redirections) | Par le parser | Avec la commande |
| `t_env` (environnement) | Au début du programme | À la fin du programme |
| Heredoc pipes | Pendant le parsing | Après l'exécution (close des fd) |

### Erreur de malloc

Si `malloc` échoue :
1. Libérer tout ce qui a été alloué jusqu'ici
2. Afficher un message d'erreur
3. Quitter proprement (ou simplement revenir au prompt)

En pratique, un `malloc` échoue rarement dans un shell, mais le code doit gérer ce cas.

### Fonctions de nettoyage

Prévoir des fonctions dédiées :
- `free_tokens(t_token *tokens)` — parcourt et libère toute la liste
- `free_cmds(t_cmd *cmds)` — parcourt et libère les commandes, args, redirections
- `free_env(t_env *env)` — libère toute la liste d'environnement
- `free_tab(char **tab)` — libère un tableau de strings + le tableau lui-même

### Fuites tolérées

Les fuites venant de **readline** et **add_history** sont **tolérées** (elles sont internes à la bibliothèque et tu ne peux pas les éviter). Utilise le fichier `readline.supp` avec Valgrind pour les supprimer des rapports.

---

## 31. ⚠️ Gestion des erreurs

### Format des messages d'erreur

Bash a un format standard. **Les messages d'erreur vont sur stderr (fd 2)** :

```
minishell: <contexte>: <description de l'erreur>
```

Exemples :
```
minishell: syntax error near unexpected token `|'
minishell: cd: /nonexistent: No such file or directory
minishell: export: `1abc': not a valid identifier
minishell: exit: abc: numeric argument required
minishell: fichier_inexistant: command not found
```

### Types d'erreurs et exit status

| Erreur | Message | Exit status |
|--------|---------|-------------|
| Commande non trouvée | `minishell: cmd: command not found` | 127 |
| Permission denied (commande) | `minishell: cmd: Permission denied` | 126 |
| Erreur de syntaxe | `minishell: syntax error...` | 2 |
| Fichier de redirection inexistant | `minishell: file: No such file or directory` | 1 |
| Builtin : argument invalide | Dépend du builtin | 1 ou 2 |
| Signal SIGINT (ctrl-C) | — | 130 (128+2) |
| Signal SIGQUIT (ctrl-\) | — | 131 (128+3) |

### Quand NE PAS quitter

La plupart des erreurs **n'arrêtent pas** le shell. On affiche l'erreur, on met le bon exit status, et on revient au prompt.

Les cas qui terminent le shell :
- `exit` (builtin)
- `ctrl-D` sur un prompt vide
- Erreur critique (malloc échoue, ce qui est rare)

---

## 32. 🔢 Exit status ($?)

### Qu'est-ce que `$?` ?

`$?` contient le **code de retour** de la **dernière commande exécutée**.

```bash
ls fichier_existant
echo $?    # → 0 (succès)

ls fichier_inexistant
echo $?    # → 2 (erreur)

echo $?    # → 0 (car echo a réussi !)
```

**Attention** : `echo $?` lui-même retourne 0, donc `$?` change à chaque commande.

### Valeurs courantes

| Situation | $? |
|-----------|----|
| Succès | 0 |
| Erreur générale | 1 |
| Erreur de syntaxe (builtin mal utilisé) | 2 |
| Erreur de syntaxe (shell) | 2 |
| Permission denied (commande) | 126 |
| Commande non trouvée | 127 |
| Tué par SIGINT (ctrl-C) | 130 |
| Tué par SIGQUIT (ctrl-\) | 131 |

### $? et les pipes

Dans un pipe, `$?` reflète le code de retour du **dernier** processus :

```bash
false | true
echo $?     # → 0 (true est la dernière commande, elle réussit)

true | false
echo $?     # → 1 (false est la dernière commande, elle échoue)
```

### Quand mettre à jour $?

- Après chaque commande exécutée (externe ou builtin)
- Après un signal
- Après une erreur de syntaxe
- Après une erreur de redirection

---

# PARTIE VIII — CAS LIMITES ET PIÈGES

---

## 33. 🎯 Cas limites exhaustifs

### Lexer / Quotes

```bash
# Concaténation de quotes
echo "hello"'world'           # → helloworld (1 seul argument)
echo "'"                      # → '
echo '"'                      # → "
echo "'"'"'                   # → '"  (enchaînement complexe)

# Quotes vides
echo ""                       # → (chaîne vide, mais un argument !)
echo ''                       # → (idem)
echo "" ""                    # → (deux arguments vides, séparés par un espace)
echo ""a""b                   # → ab

# $ dans les quotes
echo "$USER"                  # → gchaillo
echo '$USER'                  # → $USER
echo "'$USER'"                # → 'gchaillo'
echo '"$USER"'                # → "$USER"
```

### Expansion

```bash
# $ tout seul
echo $                        # → $
echo " $ "                    # → " $ "
echo $?                       # → 0 (ou le dernier exit status)
echo $?$?                     # → 00 (deux expansions collées)

# Variable suivie de texte
echo $USERx                   # → (vide, car variable $USERx n'existe pas)
echo ${USER}x                 # → gchaillo (bash syntax, PAS dans minishell)
                              # Minishell n'a pas besoin de gérer ${}

# Variable inexistante
echo $ZZZZZZZ                 # → (chaîne vide, pas d'erreur)
echo "hello $ZZZZZZZ world"  # → "hello  world" (deux espaces)
```

### Redirections

```bash
# Redirection avant la commande
> outfile echo hello          # → "hello" est écrit dans outfile ✓

# Multiples redirections
echo hello > f1 > f2 > f3    # f1 et f2 sont créés vides, "hello" dans f3

# Fichier qui n'existe pas (input)
cat < nexiste_pas             # → erreur, cat n'est PAS exécuté

# Permissions
cat < /etc/shadow              # → Permission denied (sur la plupart des systèmes)

# Redirection combinée avec pipe
echo hello > out | cat        # cat ne reçoit rien (echo écrit dans out)
cat < infile | grep hello > outfile  # cat lit infile, grep écrit dans outfile
```

### Pipes

```bash
# Commande inexistante dans un pipe
aaaa | ls                     # "aaaa: command not found" (stderr), ls s'exécute quand même
ls | aaaa                     # ls s'exécute, "aaaa: command not found", $? = 127

# Pipe vers un builtin
echo hello | cd /tmp          # cd reçoit hello sur stdin mais ne le lit pas
                              # cd /tmp s'exécute dans un fork → ne change PAS le répertoire !

# Commande vide dans un pipe
| ls                          # erreur de syntaxe
ls |                          # erreur de syntaxe
ls | | cat                    # erreur de syntaxe
```

### Builtins

```bash
# echo edge cases
echo -n                       # → (rien, même pas un \n)
echo -nnnnnn                  # → (rien)
echo -nnn hello               # → hello (sans \n)
echo -n -n -n hello           # → hello (sans \n)
echo -na hello                # → -na hello\n (le 'a' invalide le flag)

# cd edge cases
cd / && pwd                   # → /
cd ~ 2>/dev/null && pwd       # ~ devrait être interprété comme $HOME
cd ""                         # → erreur
cd "   "                      # → erreur (dossier n'existe pas)

# export edge cases
export ""                     # → erreur : not a valid identifier
export "A=hello world"        # → A vaut "hello world"
export A=1 B=2 2C=3           # A=1 OK, B=2 OK, 2C=3 ERREUR (mais A et B sont quand même set)

# exit edge cases
exit 9999999999999999999999   # → numeric argument required (overflow)
exit +42                      # → quitte avec 42 (le + est accepté)
exit -42                      # → quitte avec 214 (-42 + 256 = 214)
exit " 42 "                   # → quitte avec 42 (bash accepte les espaces autour)
exit 42 abc                   # → "too many arguments", NE quitte PAS

# unset edge cases
unset PATH                    # → plus rien ne fonctionne (commandes externes)
unset HOME && cd              # → "minishell: cd: HOME not set"
```

### Commande vide

```bash
""                            # → commande vide : "minishell: : command not found"
''                            # → idem
$UNDEFINED_VAR                # → ne fait rien (pas d'erreur) — le mot disparaît après expansion
"$UNDEFINED_VAR"              # → commande vide : erreur (les quotes préservent l'argument vide)
```

### Signaux

```bash
# ctrl-C sur un prompt vide
minishell$ [ctrl-C]
minishell$ [ctrl-C]
minishell$ [ctrl-C]
# → 3 nouveaux prompts, $? = 130 à chaque fois

# ctrl-C pendant cat
minishell$ cat
[ctrl-C]
^C
minishell$                     # $? = 130

# ctrl-\ pendant cat
minishell$ cat
[ctrl-\]
^\Quit (core dumped)
minishell$                     # $? = 131

# ctrl-D
minishell$ [ctrl-D]
exit                           # le shell affiche "exit" et se termine
```

---

## 34. ⚖️ Différences entre bash et minishell

### Ce que minishell NE FAIT PAS (non requis par le sujet)

| Feature bash | Minishell |
|-------------|-----------|
| `&&` et `\|\|` (opérateurs logiques) | ❌ |
| Sous-shells `(cmd)` | ❌ |
| Wildcards `*`, `?` | ❌ |
| `$()` (command substitution) | ❌ |
| `` `cmd` `` (backticks) | ❌ |
| `${VAR}` (accolades) | ❌ |
| `$0`, `$1`, ... (paramètres positionnels) | ❌ |
| `~` (tilde expansion, sauf si tu veux en bonus) | ❌ |
| `\` (backslash escaping) | ❌ |
| `;` (séparateur de commandes) | ❌ |
| Scripts (lecture depuis un fichier) | ❌ |
| Fonctions shell | ❌ |
| Arrays | ❌ |
| Alias | ❌ |
| `history` builtin | ❌ |
| Job control (`bg`, `fg`, `jobs`) | ❌ |

### Ce que tu DOIS reproduire exactement

- Le comportement de `bash` pour les features listées dans le sujet
- Les messages d'erreur (format similaire à bash)
- Les codes de retour (`$?`)
- Le comportement des signaux dans chaque contexte

### Comment tester : toujours comparer avec bash

La règle d'or : **en cas de doute, teste dans bash et reproduis le comportement**.

```bash
# Ouvre deux terminaux :
# Terminal 1 : bash
# Terminal 2 : ./minishell

# Tape la même commande dans les deux
# Compare les sorties, les exit status, les messages d'erreur
```

---

# ANNEXES

---

## A. Résumé des codes de retour

| Code | Signification |
|------|---------------|
| 0 | Succès |
| 1 | Erreur générale / erreur de builtin |
| 2 | Erreur de syntaxe / mauvais usage |
| 126 | Commande trouvée mais non exécutable |
| 127 | Commande non trouvée |
| 128+N | Terminé par le signal N |
| 130 | Terminé par SIGINT (ctrl-C) : 128+2 |
| 131 | Terminé par SIGQUIT (ctrl-\) : 128+3 |

## B. Résumé des redirections

| Symbole | Nom | open() flags | dup2 target |
|---------|-----|-------------|-------------|
| `<` | Input | `O_RDONLY` | `STDIN_FILENO` |
| `>` | Output | `O_WRONLY \| O_CREAT \| O_TRUNC` | `STDOUT_FILENO` |
| `>>` | Append | `O_WRONLY \| O_CREAT \| O_APPEND` | `STDOUT_FILENO` |
| `<<` | Heredoc | (pipe, pas open) | `STDIN_FILENO` |

## C. Résumé des signaux par contexte

| Contexte | SIGINT (ctrl-C) | SIGQUIT (ctrl-\) | EOF (ctrl-D) |
|----------|-----------------|-------------------|--------------|
| Prompt | Nouvelle ligne + prompt, $?=130 | Ignoré | Exit le shell |
| Exécution | Tue l'enfant, $?=130 | Tue l'enfant + core dump, $?=131 | Rien |
| Heredoc | Interrompt le heredoc, $?=130 | Ignoré | Warning + ferme heredoc |

## D. Récapitulatif des fonctions autorisées

| Fonction | Header | Usage dans minishell |
|----------|--------|---------------------|
| `readline` | `<readline/readline.h>` | Lire l'input avec prompt |
| `rl_clear_history` | `<readline/history.h>` | Nettoyer l'historique |
| `rl_on_new_line` | `<readline/readline.h>` | Signaler une nouvelle ligne |
| `rl_replace_line` | `<readline/readline.h>` | Remplacer le contenu de la ligne |
| `rl_redisplay` | `<readline/readline.h>` | Réafficher le prompt |
| `add_history` | `<readline/history.h>` | Ajouter à l'historique |
| `printf` | `<stdio.h>` | Affichage (préférer write/ft_putstr) |
| `malloc` | `<stdlib.h>` | Allocation mémoire |
| `free` | `<stdlib.h>` | Libération mémoire |
| `write` | `<unistd.h>` | Écriture bas niveau |
| `access` | `<unistd.h>` | Vérifier les droits d'un fichier |
| `open` | `<fcntl.h>` | Ouvrir un fichier |
| `read` | `<unistd.h>` | Lire depuis un fd |
| `close` | `<unistd.h>` | Fermer un fd |
| `fork` | `<unistd.h>` | Créer un processus enfant |
| `wait` | `<sys/wait.h>` | Attendre un enfant |
| `waitpid` | `<sys/wait.h>` | Attendre un enfant spécifique |
| `wait3` | `<sys/wait.h>` | wait avec infos de ressources |
| `wait4` | `<sys/wait.h>` | waitpid avec infos de ressources |
| `signal` | `<signal.h>` | Associer un handler à un signal |
| `sigaction` | `<signal.h>` | Handler avancé (recommandé) |
| `sigemptyset` | `<signal.h>` | Initialiser un set de signaux vide |
| `sigaddset` | `<signal.h>` | Ajouter un signal à un set |
| `kill` | `<signal.h>` | Envoyer un signal à un processus |
| `exit` | `<stdlib.h>` | Terminer le processus |
| `getcwd` | `<unistd.h>` | Obtenir le répertoire courant |
| `chdir` | `<unistd.h>` | Changer de répertoire |
| `stat` | `<sys/stat.h>` | Infos sur un fichier |
| `lstat` | `<sys/stat.h>` | Infos (sans suivre les symlinks) |
| `fstat` | `<sys/stat.h>` | Infos depuis un fd |
| `unlink` | `<unistd.h>` | Supprimer un fichier |
| `execve` | `<unistd.h>` | Remplacer le programme courant |
| `dup` | `<unistd.h>` | Dupliquer un fd |
| `dup2` | `<unistd.h>` | Dupliquer un fd vers un fd cible |
| `pipe` | `<unistd.h>` | Créer un tube |
| `opendir` | `<dirent.h>` | Ouvrir un répertoire |
| `readdir` | `<dirent.h>` | Lire une entrée de répertoire |
| `closedir` | `<dirent.h>` | Fermer un répertoire |
| `strerror` | `<string.h>` | Description d'une erreur |
| `perror` | `<stdio.h>` | Afficher une erreur |
| `isatty` | `<unistd.h>` | Vérifier si fd est un terminal |
| `ttyname` | `<unistd.h>` | Nom du terminal |
| `ttyslot` | `<unistd.h>` | Slot du terminal |
| `ioctl` | `<sys/ioctl.h>` | Contrôle d'I/O |
| `getenv` | `<stdlib.h>` | Lire une variable d'env |
| `tcsetattr` | `<termios.h>` | Configurer le terminal |
| `tcgetattr` | `<termios.h>` | Lire la config du terminal |

## E. Schéma mental complet

```
┌──────────────────────────────────────────────────────────────────┐
│                     BOUCLE PRINCIPALE                            │
│                                                                  │
│  1. setup_signals(PROMPT_MODE)                                   │
│  2. input = readline("minishell$ ")                              │
│  3. si input == NULL → afficher "exit", quitter                  │
│  4. si input non vide → add_history(input)                       │
│                                                                  │
│  5. tokens = lexer(input)                                        │
│     └─ Découpe en WORD, PIPE, REDIR_IN, REDIR_OUT, APPEND,     │
│        HEREDOC. Gère les quotes.                                 │
│                                                                  │
│  6. si erreur de syntaxe → afficher erreur, $?=2, goto 1         │
│                                                                  │
│  7. cmds = parser(tokens)                                        │
│     └─ Construit la liste chaînée de t_cmd.                     │
│        Extrait les redirections. Ouvre les heredocs.              │
│                                                                  │
│  8. expander(cmds, env)                                          │
│     └─ Remplace $VAR par sa valeur dans les args.               │
│        Retire les quotes.                                        │
│                                                                  │
│  9. setup_signals(EXEC_MODE)                                     │
│ 10. executor(cmds, env)                                          │
│     └─ Si 1 cmd et builtin → exec dans parent                  │
│        Sinon → fork + exec pour chaque commande                 │
│        Gère pipes, redirections, waitpid                        │
│        Met à jour $?                                            │
│                                                                  │
│ 11. free(input), free_tokens(tokens), free_cmds(cmds)           │
│ 12. goto 1                                                       │
└──────────────────────────────────────────────────────────────────┘
```

---

> **Ce cours couvre l'intégralité des notions nécessaires pour minishell.**
> En cas de doute sur un comportement, ouvre un terminal bash et teste.
> Le comportement de **bash** est ta référence ultime.
