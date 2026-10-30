# Workflow Git

1. **Toujours travailler sur une branche `feature/`**, jamais directement sur `main` ou `develop`
2. **Quand une feature est prête** :
   ```bash
   git checkout develop
   git pull origin develop
   git merge feature/ma-feature
   # Résoudre les conflits si nécessaire
   git push origin develop
   ```
3. **Quand `develop` est stable** (fin de semaine, livrable atteint) :
   ```bash
   git checkout main
   git merge develop
   git push origin main
   ```

### Conventions de commit

Utiliser des **prefixes clairs** pour les messages de commit :

| Préfixe | Usage | Exemple |
|---------|-------|---------|
| `feat:` | Nouvelle fonctionnalité | `feat: add pipe execution for multiple commands` |
| `fix:` | Correction de bug | `fix: handle empty input in lexer` |
| `refactor:` | Refactorisation sans changement fonctionnel | `refactor: simplify token list creation` |
| `docs:` | Documentation | `docs: update ORGANISATION.md` |
| `norm:` | Corrections de norminette | `norm: fix line length in parser.c` |
| `test:` | Ajout/modif de tests | `test: add edge cases for quote handling` |
| `wip:` | Work in progress (ne pas merger) | `wip: heredoc parsing in progress` |

### Règles Git

- ❌ **Ne JAMAIS `push --force`** sur `main` ou `develop`
- ✅ **Commit souvent** avec des messages clairs (petits commits atomiques)
- ✅ **Pull `develop`** avant de commencer à travailler chaque jour
- ✅ **Tester avant de merger** dans `develop`
- ✅ Faire un `.gitignore` propre :

```gitignore
# Objets et binaire
*.o
*.a
minishell

# Dossiers
obj/

# Éditeurs
.vscode/
*.swp
*.swo
*~

# macOS
.DS_Store
```

---