# Flowcharts des Fonctions du Parser

Ce document rassemble les flowcharts d'exécution (logique) pour chaque fonction présente dans le dossier `src/parser/` du projet minishell, générés à l'aide de Mermaid.

---

## Fichier `parser.c`

### `count_cmd_words`

Compte le nombre de mots (arguments) d'une commande jusqu'au pipe.
```mermaid
flowchart TD
    Start([Début de count_cmd_words]) --> Init[count = 0]
    Init --> LoopCond{"tokens != NULL &&<br>tokens->type != TOKEN_PIPE"}
    LoopCond -- Oui --> TypeCheck{"tokens->type == TOKEN_WORD?"}
    TypeCheck -- Oui --> IncCount[count++]
    IncCount --> NextWord[tokens = tokens->next]
    TypeCheck -- Non --> IsRedir{is_redir_type?}
    IsRedir -- Oui --> NextRedir[tokens = tokens->next->next]
    IsRedir -- Non --> NextOther[tokens = tokens->next]
    NextWord --> LoopCond
    NextRedir --> LoopCond
    NextOther --> LoopCond
    LoopCond -- Non --> Return([Retourner count])
```

### `redir_add_back`

Ajoute une redirection à la fin de la liste des redirections d'une commande.
```mermaid
flowchart TD
    Start([Début de redir_add_back]) --> CheckCmd{"cmd->redir_cmd == NULL?"}
    CheckCmd -- Oui --> SetFirst[cmd->redir_cmd = new_redir]
    CheckCmd -- Non --> SetTmp[tmp = cmd->redir_cmd]
    SetTmp --> LoopCond{"tmp->next != NULL?"}
    LoopCond -- Oui --> NextTmp[tmp = tmp->next]
    NextTmp --> LoopCond
    LoopCond -- Non --> SetNext[tmp->next = new_redir]
    SetFirst --> End([Fin])
    SetNext --> End
```

### `add_redir`

Crée et ajoute une nouvelle redirection.
```mermaid
flowchart TD
    Start([Début de add_redir]) --> Alloc[new_redir = malloc]
    Alloc --> CheckAlloc{new_redir == NULL?}
    CheckAlloc -- Oui --> EndReturn([Return])
    CheckAlloc -- Non --> FillFields["new_redir->file = ft_strdup<br>new_redir->int_fd = -1<br>new_redir->type = type<br>new_redir->next = NULL"]
    FillFields --> CallAddBack["redir_add_back(cmd, new_redir)"]
    CallAddBack --> End([Fin])
```

### `init_cmd`

Crée et initialise la structure `t_cmd`.
```mermaid
flowchart TD
    Start([Début de init_cmd]) --> Alloc[cmd = malloc]
    Alloc --> CheckAlloc{cmd == NULL?}
    CheckAlloc -- Oui --> RetNull([Retourner NULL])
    CheckAlloc -- Non --> InitFields["cmd->cmd_args = NULL<br>cmd->redir_cmd = NULL<br>cmd->next = NULL<br>cmd->prev = NULL"]
    InitFields --> RetCmd([Retourner cmd])
```

---

## Fichier `parser_build.c`

### `fill_cmd_args`

Remplit le tableau des commandes et leurs redirections depuis les tokens.
```mermaid
flowchart TD
    Start([Début de fill_cmd_args]) --> Init[i = 0]
    Init --> LoopCond{"*tok != NULL &&<br>(*tok)->type != TOKEN_PIPE"}
    LoopCond -- Oui --> CheckType{"(*tok)->type == TOKEN_WORD?"}
    CheckType -- Oui --> AllocArg["cmd->cmd_args[i] = ft_strdup"]
    AllocArg --> CheckAlloc{"cmd->cmd_args[i] == NULL?"}
    CheckAlloc -- Oui --> RetFail([Retourner 0])
    CheckAlloc -- Non --> IncI[i++]
    IncI --> NextTokOther["*tok = (*tok)->next"]
    CheckType -- Non --> CheckRedir{is_redir_type?}
    CheckRedir -- Oui --> CallAddRedir[add_redir]
    CallAddRedir --> NextTokSkipVal["*tok = (*tok)->next"]
    NextTokSkipVal --> NextTokOther
    CheckRedir -- Non --> NextTokOther
    NextTokOther --> LoopCond
    LoopCond -- Non --> TerminateArray["cmd->cmd_args[i] = NULL"]
    TerminateArray --> RetSuccess([Retourner 1])
```

### `build_cmd`

Crée et remplit entièrement une nouvelle de commande `t_cmd`.
```mermaid
flowchart TD
    Start([Début de build_cmd]) --> CallInit["cmd = init_cmd()"]
    CallInit --> CheckCmd{cmd == NULL?}
    CheckCmd -- Oui --> RetNull([Retourner NULL])
    CheckCmd -- Non --> CountWords["wc = count_cmd_words(*tok)"]
    CountWords --> AllocArgs["cmd->cmd_args = malloc(wc + 1)"]
    AllocArgs --> CheckAlloc{"cmd->cmd_args == NULL <br>OU<br> fill_cmd_args == 0 ?"}
    CheckAlloc -- Oui --> FreeCmd["free(cmd)"]
    FreeCmd --> RetFail([Retourner NULL])
    CheckAlloc -- Non --> RetSuccess([Retourner cmd])
```

### `cmd_add_back`

Ajoute une commande à la fin de la liste de processus.
```mermaid
flowchart TD
    Start([Début de cmd_add_back]) --> CheckHead{*head == NULL?}
    CheckHead -- Oui --> SetHead[*head = cmd]
    CheckHead -- Non --> InitTmp[tmp = *head]
    InitTmp --> LoopCond{"tmp->next != NULL?"}
    LoopCond -- Oui --> NextTmp[tmp = tmp->next]
    NextTmp --> LoopCond
    LoopCond -- Non --> AppendCmd["tmp->next = cmd<br>cmd->prev = tmp"]
    SetHead --> End([Fin])
    AppendCmd --> End
```

### `parse_tokens`

Fonction principale qui assemble les tokens en structure de commandes chainées.
```mermaid
flowchart TD
    Start([Début de parse_tokens]) --> Init[head = NULL]
    Init --> LoopCond{tokens != NULL?}
    LoopCond -- Oui --> CallBuild["cmd = build_cmd(&tokens)"]
    CallBuild --> CheckCmd{cmd == NULL?}
    CheckCmd -- Oui --> FreeAll["free_cmd(head)"]
    FreeAll --> RetNull([Retourner NULL])
    CheckCmd -- Non --> CallAddBack["cmd_add_back(&head, cmd)"]
    CallAddBack --> CheckPipe{"tokens && tokens->type == TOKEN_PIPE?"}
    CheckPipe -- Oui --> SkipPipe[tokens = tokens->next]
    SkipPipe --> LoopCond
    CheckPipe -- Non --> LoopCond
    LoopCond -- Non --> RetHead([Retourner head])
```

---

## Fichier `parser_syntax.c`

### `print_syntax_error`

Affiche le message d'erreur de syntaxe de parsing.
```mermaid
flowchart TD
    Start([Début de print_syntax_error]) --> P1[ft_putstr_fd : minishell: syntax error...]
    P1 --> CheckToken{token_val != NULL?}
    CheckToken -- Oui --> P2[ft_putstr_fd : token_val]
    CheckToken -- Non --> P3[ft_putstr_fd : newline]
    P2 --> P4[ft_putstr_fd : '\n]
    P3 --> P4
    P4 --> End([Fin])
```

### `is_redir_type`

Vérifie si un token est une redirection.
```mermaid
flowchart TD
    Start([Début de is_redir_type]) --> Check[Vérifie IN, OUT, APPEND, HEREDOC]
    Check --> Ret([Retourne true/false])
```

### `handle_redir_error`

Vérifie qu'une redirection est bien suivie d'un argument valide.
```mermaid
flowchart TD
    Start([Début de handle_redir_error]) --> Cond{"curr->next == NULL <br>OU<br> curr->next->type != TOKEN_WORD?"}
    Cond -- Oui --> CheckNext{"curr->next != NULL?"}
    CheckNext -- Oui --> PrintTok["print_syntax_error(curr->next->value)"]
    CheckNext -- Non --> PrintNull["print_syntax_error(NULL)"]
    PrintTok --> RetFail([Retourner 0])
    PrintNull --> RetFail
    Cond -- Non --> RetSuccess([Retourner 1])
```

### `check_token_syntax`

Vérifie unitairement la syntaxe avec les conditions locales d'un token.
```mermaid
flowchart TD
    Start([Début de check_token_syntax]) --> IsPipe{"curr->type == TOKEN_PIPE?"}
    IsPipe -- Oui --> CheckPrev{"prev == NULL <br>OU<br> prev->type == TOKEN_PIPE?"}
    CheckPrev -- Oui --> PrintErrPipe[print_syntax_errorPipe]
    PrintErrPipe --> RetFail([Retourner 0])
    CheckPrev -- Non --> RetSuccess([Retourner 1])
    IsPipe -- Non --> IsRedir{is_redir_type?}
    IsRedir -- Oui --> CallHandleRedir["handle_redir_error(curr)"]
    CallHandleRedir --> RetCallHandle([Retourner status])
    IsRedir -- Non --> RetSuccess
```

### `check_syntax`

Fonction principale pour s'assurer de la validité d'une liste entière de tokens.
```mermaid
flowchart TD
    Start([Début de check_syntax]) --> Init["curr = tokens<br>prev = NULL"]
    Init --> LoopCond{curr != NULL?}
    LoopCond -- Oui --> CallCheckTok["check_token_syntax(curr, prev)"]
    CallCheckTok --> CheckTokRes{Valide?}
    CheckTokRes -- Non --> RetFailToken([Retourner 0])
    CheckTokRes -- Oui --> Advance["prev = curr<br>curr = curr->next"]
    Advance --> LoopCond
    LoopCond -- Non --> CheckTrailPipe{"prev != NULL &&<br>prev->type == TOKEN_PIPE?"}
    CheckTrailPipe -- Oui --> PrintErrTrailPipe[print_syntax_errorPipe]
    PrintErrTrailPipe --> RetFailTrail([Retourner 0])
    CheckTrailPipe -- Non --> RetOk([Retourner 1])
```

---

## Fichier `parser_free.c`

### `free_cmd_redirs`

Libère les redirections rattachées à une commande.
```mermaid
flowchart TD
    Start([Début de free_cmd_redirs]) --> LoopCond{"cmd->redir_cmd != NULL?"}
    LoopCond -- Oui --> SaveNext[tmp_redir = cmd->redir_cmd->next]
    SaveNext --> CheckFile{"cmd->redir_cmd->file != NULL?"}
    CheckFile -- Oui --> FreeFile["free(file)"]
    FreeFile --> FreeNode["free(redir_cmd)"]
    CheckFile -- Non --> FreeNode
    FreeNode --> Next[cmd->redir_cmd = tmp_redir]
    Next --> LoopCond
    LoopCond -- Non --> End([Fin])
```

### `free_cmd`

Parcourt des commandes chaînées pour tout libérer.
```mermaid
flowchart TD
    Start([Début de free_cmd]) --> LoopCond{cmd != NULL?}
    LoopCond -- Oui --> SaveTmp[tmp_cmd = cmd->next]
    SaveTmp --> CheckArgs{"cmd->cmd_args != NULL?"}
    CheckArgs -- Oui --> LoopArgs{"cmd->cmd_args[i] != NULL?"}
    LoopArgs -- Oui --> FreeArg["free arg i<br>i++"]
    FreeArg --> LoopArgs
    LoopArgs -- Non --> FreeArrArgs["free(cmd->cmd_args)"]
    FreeArrArgs --> CallFreeRedirs["free_cmd_redirs(cmd)"]
    CheckArgs -- Non --> CallFreeRedirs
    CallFreeRedirs --> FreeCmd["free(cmd)"]
    FreeCmd --> Adv[cmd = tmp_cmd]
    Adv --> LoopCond
    LoopCond -- Non --> End([Fin])
```