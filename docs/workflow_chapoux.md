# Workflow Git - Branche `ChapouX`

Ce document explique le flux de travail (workflow) pour développer sur la branche `ChapouX` entre deux ordinateurs (PC portable à la maison et PC fixe à l'école) sans impacter la branche `main`.

## Prérequis

Assurez-vous d'être toujours sur la branche `ChapouX` avant de commencer à travailler :
```bash
git checkout ChapouX
```

---

## Scénario 1 : Vous commencez à travailler (ex: Arrivée à l'école ou à la maison)

Avant de modifier quoi que ce soit, **récupérez toujours le travail le plus récent** que vous aviez potentiellement poussé depuis l'autre ordinateur.

1. Vérifier qu'on est sur la bonne branche :
   ```bash
   git status
   ```
2. Récupérer les dernières modifications du serveur :
   ```bash
   git pull origin ChapouX
   ```

---

## Scénario 2 : Vous avez terminé votre session de travail (ex: Départ de l'école ou fin de journée)

Une fois que vos modifications sont prêtes à être sauvegardées et partagées avec votre autre ordinateur.

1. Ajouter les fichiers modifiés :
   ```bash
   git add .
   # ou git add <nom_du_fichier> pour des fichiers spécifiques
   ```
2. Créer un commit avec un message descriptif :
   ```bash
   git commit -m "Description de ce que j'ai fait aujourd'hui"
   ```
3. Envoyer le code sur le serveur (uniquement sur votre branche) :
   ```bash
   git push origin ChapouX
   ```

---

## Scénario 3 : Intégrer le travail de votre collègue (branche `agard`)

Si votre collègue a poussé du nouveau code sur sa branche `agard` et que vous en avez besoin sur votre branche `ChapouX` :

1. Mettre à jour la liste des branches et les modifications depuis le serveur :

   ```bash
   git fetch origin
   ```

2. Vous assurer que vous êtes sur votre branche :

   ```bash
   git checkout ChapouX
   ```

3. Fusionner (merge) le code de votre collègue dans votre branche :

   ```bash
   git merge origin/agard
   ```

   *(Si des conflits apparaissent, il faudra les résoudre dans les fichiers concernés, puis faire un `git add .` et `git commit`)*

4. Une fois fusionné (et testé), vous pouvez l'envoyer sur votre propre branche distante pour que vos deux PC l'aient :

   ```bash
   git push origin ChapouX
   ```

---

## Scénario 4 : Créer une branche de développement (Recommandé)

Pour éviter de "casser" votre branche `ChapouX` lorsqu'une modification est complexe ou longue, il est recommandé de créer une sous-branche de fonctionnalité (feature branch).

1. Créer et se placer sur une nouvelle branche à partir de `ChapouX` :

   ```bash
   git checkout -b feat/nom-de-la-fonctionnalite
   ```

2. Travailler dessus normalement, la sauvegarder et la pousser sur le serveur pour pouvoir y accéder sur l'autre PC :

   ```bash
   git add .
   git commit -m "Travail sur la fonctionnalité"
   git push -u origin feat/nom-de-la-fonctionnalite
   ```

3. Une fois la fonctionnalité terminée et fonctionnelle, la rapatrier (fusionner) dans votre branche `ChapouX` propre :

   ```bash
   git checkout ChapouX
   git merge feat/nom-de-la-fonctionnalite
   git push origin ChapouX
   ```

*(Vous pouvez ensuite supprimer la branche de fonctionnalité sur votre PC avec `git branch -d feat/nom-de-la-fonctionnalite` si vous n'en avez plus besoin).*

---

## Scénario 5 : Résolution de problèmes (Troubleshooting)

### Message "Your branch is ahead of 'origin/ChapouX' by X commits" avec "diverged"

Vous avez oublié de `push` sur un PC, puis vous avez travaillé et `push` depuis l'autre. Les deux historiques sont différents.

#### Option 1 : Fusionner le travail des deux PC (Recommandé)

1. Téléchargez les infos du serveur :

   ```bash
   git fetch origin
   ```

2. Fusionnez le code de l'autre PC (le serveur) avec votre PC actuel :

   ```bash
   git pull origin ChapouX
   ```

   *(Un éditeur de texte (Vim/Nano) peut s'ouvrir pour demander de valider le message de fusion : sauvegardez et quittez. S'il y a des conflits, gérez-les dans les fichiers en conflit, puis `git add .` et `git commit`).*

3. Envoyez le résultat propre sur le serveur :

   ```bash
   git push origin ChapouX
   ```

#### Option 2 : Écraser "l'avance" locale pour copier le serveur

⚠️ *Attention : cela supprime définitivement les commits non envoyés de ce PC. Utilisez cette option uniquement si vous voulez remplacer le travail de ce PC par celui du serveur.*

```bash
git fetch origin
git reset --hard origin/ChapouX
```

---

## Scénario 6 : Comparer son travail avec la branche principale (`main`)

Si vous voulez savoir ce qui a été modifié sur votre branche `ChapouX` par rapport à la branche `main` :

1. Voir la liste des fichiers modifiés (résumé) :

   ```bash
   git diff main..ChapouX --stat
   ```

2. Voir le détail du code modifié ligne par ligne :

   ```bash
   git diff main..ChapouX
   ```

   *(Utilisez les flèches haut/bas pour naviguer et appuyez sur la touche `q` pour quitter l'affichage).*

3. Voir l'historique des commits qui sont sur `ChapouX` mais pas sur `main` :

   ```bash
   git log main..ChapouX --oneline
   ```

---

## Scénario 7 : Fusionner son travail dans la branche principale (`main`)

Une fois qu'une grande fonctionnalité est terminée, testée et fonctionnelle sur `ChapouX`, il est temps de l'intégrer dans la version officielle du projet (`main`).

1. Placez-vous sur la branche `main` :

   ```bash
   git checkout main
   ```

2. Assurez-vous que `main` est à jour avec le serveur :

   ```bash
   git pull origin main
   ```

3. Fusionnez votre travail (`ChapouX`) dans `main` :

   ```bash
   git merge ChapouX
   ```

4. Envoyez la branche `main` mise à jour sur le serveur :

   ```bash
   git push origin main
   ```

5. (Optionnel) Repartez sur votre branche pour continuer à coder :

   ```bash
   git checkout ChapouX
   ```

*(Note : Si vous utilisez GitHub de manière collaborative, il est souvent recommandé de créer une **Pull Request** directement depuis le site web de GitHub plutôt que de taper ces commandes. Cela permet à votre collègue de relire le code avant de l'accepter sur `main`).*

---

## Résumé du cycle quotidien

1. **Pull** (Récupérer) : `git pull origin ChapouX`
2. **Code** (Travailler) : Faites vos modifications.
3. **Commit** (Sauvegarder) : `git add .` puis `git commit -m "..."`
4. **Push** (Envoyer) : `git push origin ChapouX`

*Note : Gardez ce cycle, et vous aurez toujours votre code synchronisé entre votre PC portable et votre PC de l'école sans jamais toucher à la branche `main` !*
