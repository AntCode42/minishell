# Cours Détaillé : Les Redirections en Bash

Ce document explore en détail le fonctionnement des redirections dans le shell Bash, indépendamment de toute implémentation spécifique (comme Minishell). Il couvre les concepts fondamentaux, les types de redirections, l'ordre d'évaluation, ainsi que les pièges classiques et cas limites.

---

## 1. Les Fondations : Les Descripteurs de Fichiers (File Descriptors - FD)

Sous Linux/Unix, tout (ou presque) est un fichier, y compris les entrées/sorties du terminal. Lorsqu'un processus est lancé, le shell lui ouvre par défaut trois descripteurs de fichiers standard :

- **`0` (Standard Input - `stdin`)** : L'entrée standard. Par défaut, c'est le clavier.
- **`1` (Standard Output - `stdout`)** : La sortie standard. Par défaut, c'est l'écran (le terminal).
- **`2` (Standard Error - `stderr`)** : La sortie d'erreur standard. Par défaut, c'est aussi l'écran.

Les redirections consistent à dire au shell de modifier la destination ou l'origine de ces descripteurs avant d'exécuter la commande.

---

## 2. Les Redirections Classiques

### 2.1. Redirection de la sortie standard (`>`)
Écrit la sortie d'une commande dans un fichier (FD `1`).
- **Si le fichier n'existe pas**, il est créé.
- **Si le fichier existe**, son contenu est **écrasé** (tronqué à zéro octet) avant même que la commande ne commence à s'exécuter.

```bash
echo "Bonjour" > fichier.txt
ls -l > liste.txt
```

### 2.2. Ajout à la sortie standard (`>>`)
Similaire à `>`, mais **ajoute** à la fin du fichier sans écraser le contenu existant.

```bash
echo "Ligne 1" > fichier.txt
echo "Ligne 2" >> fichier.txt
```

### 2.3. Redirection de l'entrée standard (`<`)
Fait en sorte qu'une commande lise son entrée depuis un fichier (FD `0`) au lieu du clavier.

```bash
cat < fichier.txt
grep "erreur" < log.txt
```

### 2.4. Le Here-Document (`<<`)
Permet de fournir un bloc de texte multiligne à l'entrée standard d'une commande, jusqu'à ce qu'un délimiteur spécifique soit lu.

```bash
cat << EOF
Ceci est un texte.
Sur plusieurs lignes.
EOF
```
*Note : Si le délimiteur est quoté (ex: `<< 'EOF'`), les variables (`$VAR`) à l'intérieur du bloc ne sont pas expansées.*

---

## 3. Redirections Avancées et Combinaisons

### 3.1. Rediriger la sortie d'erreur (`2>`)
Par défaut, `>` ne redirige que `stdout` (FD 1). Si une commande échoue, l'erreur s'affiche toujours à l'écran. Pour rediriger l'erreur, on cible explicitement le FD `2`.

```bash
ls fichier_inexistant 2> erreurs.log
```

### 3.2. Rediriger stdout ET stderr
Souvent, on veut capter tout ce que produit un programme (logs).

**Méthode classique (portable PosiX) : `> fichier 2>&1`**
Cela signifie : 
1. Redirige `1` (stdout) vers `fichier`.
2. Redirige `2` (stderr) vers l'endroit où pointe actuellement `1` (donc vers `fichier`).

```bash
make > build.log 2>&1
```

**Méthode "Bashisme" (raccourci non PosiX) : `&>` ou `>&`**
```bash
make &> build.log
```

### 3.3. Annuler l'affichage (le trou noir `/dev/null`)
Pour ignorer des sorties ou des erreurs, on les redirige vers `/dev/null`.

```bash
# Ignorer les erreurs
find / -name "secret.txt" 2> /dev/null

# Tout ignorer (silence total)
commande_bruyante > /dev/null 2>&1
```

---

## 4. Pièges Classiques et Cas Limites

### Piège n°1 : L'ordre pèse lourdement (`> file 2>&1` vs `2>&1 > file`)
Le shell évalue les redirections de **gauche à droite**.

✅ **Correct** : `ls > out 2>&1`
1. `stdout` pointe vers `out`.
2. `stderr` pointe vers où pointe `stdout` (soit `out`).
-> *Les deux vont dans `out`.*

❌ **Piège** : `ls 2>&1 > out`
1. `stderr` pointe vers où pointe `stdout` (c'est-à-dire l'écran à cet instant).
2. `stdout` est modifié pour pointer vers `out`.
-> *Les erreurs s'affichent à l'écran, seule la sortie standard va dans `out`.*

### Piège n°2 : La place de la redirection
En Bash, une redirection peut apparaître **n'importe où** dans la commande (sauf au milieu d'un mot).

```bash
# Toutes ces commandes sont strictement équivalentes en Bash :
grep "foo" < in > out
< in grep "foo" > out
> out < in grep "foo"
grep > out < in "foo"
```
C'est souvent déroutant pour les débutants (et pour les développeurs de Minishell !). Le shell extrait toutes les redirections avant d'exécuter la commande ou d'évaluer ses arguments.

### Piège n°3 : L'écrasement prématuré
Le shell gère les redirections *avant* de lancer la commande.

```bash
# TRÈS MAUVAISE IDÉE
cat fichier.txt | grep "mot" > fichier.txt
```
**Ce qui se passe :** Avant de lancer la commande, Bash voit `> fichier.txt` et tronque le fichier à zéro octet. Quand `cat` essaie de lire `fichier.txt`, il est déjà vide ! La commande ne renverra rien et supprime vos données.

### Piège n°4 : Multiples redirections du même descripteur
Que se passe-t-il si on redirige plusieurs fois la même chose ?

```bash
echo "Salut" > f1.txt > f2.txt > f3.txt
```
**Résultat :** 
- Bash crée (ou vide) `f1.txt`, puis `f2.txt`, puis `f3.txt`.
- Ensuite, il attache la sortie standard de `echo` à `f3.txt` (la dernière redirection écrase les processus des précédentes).
- Au final : `f1.txt` et `f2.txt` existent mais sont vides, `f3.txt` contient "Salut".

---

## 5. Les Pipes (`|`) et Redirections

Un pipe modifie implicitement les FD : il relie le `stdout` de la commande de gauche au `stdin` de celle de droite.

Les redirections explicites sont appliquées **après** la configuration du pipe.

```bash
# Exemple complexe : cmd1 | >out cmd2
echo "test" | >out grep "t"
```
1. Pipe créé : stdout de `echo` -> stdin de `grep`.
2. Avant de lancer `grep`, Bash applique ses redirections spécifiques : la sortie standard de `grep` est redirigée vers le fichier `out`.
3. Résultat : "test" entre dans grep. grep trouve "test", veut le printer sur `stdout`, mais `stdout` a été redirigé vers `out`. Le fichier `out` contiendra "test".

Si vous redirigez explicitement la sortie d'une commande avant un pipe, le pipe ne recevra rien !
```bash
echo "hello" > fichier.txt | cat
# 'cat' écrira un flux vide à l'écran, car le "hello" est parti dans fichier.txt au lieu de passer dans le pipe.
```
