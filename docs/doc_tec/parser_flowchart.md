# Parser Flowchart

Ce document détaille le fonctionnement du parser du minishell, qui est chargé de transformer la liste de tokens bruts provenant du lexer en une liste de commandes structurées (`t_cmd`) prêtes pour l'exécution.

## 1. Validation de la syntaxe (`check_syntax`)

Avant de construire les arbres de commandes, le parser s'assure que la suite de tokens est syntaxiquement valide (pas de pipe en début/fin, pas de redirections sans cibles, etc.).

```mermaid
flowchart TD
    StartCheck(["Début : check_syntax(tokens)"]) --> LoopSyntax{"Il y a des tokens ?"}
    
    LoopSyntax -- Oui --> CheckTokenSyntax["check_token_syntax()"]
    
    CheckTokenSyntax --> TokenPipe{"Est-ce un TOKEN_PIPE ?"}
    TokenPipe -- Oui --> CheckValidPipe{"Précédent est PIPE ou NULL ?"}
    CheckValidPipe -- Oui --> SyntaxError["Erreur de syntaxe"]
    CheckValidPipe -- Non --> NextToken["Token suivant"]
    
    TokenPipe -- Non --> TokenRedir{"Est-ce une redirection ?"}
    TokenRedir -- Oui --> TargetExists{"Token suivant == TOKEN_WORD ?"}
    TargetExists -- Non --> SyntaxError
    TargetExists -- Oui --> NextToken
    
    TokenRedir -- Non --> NextToken
    
    NextToken --> LoopSyntax
    
    LoopSyntax -- Non --> CheckEndPipe{"Dernier token == PIPE ?"}
    CheckEndPipe -- Oui --> SyntaxError
    CheckEndPipe -- Non --> ReturnValid(["Syntaxe Valide : Retourne 1"])
    
    SyntaxError --> PrintError["Affiche message d'erreur de syntaxe"]
    PrintError --> ReturnInvalid(["Syntaxe Invalide : Retourne 0"])
```

## 2. Construction de la liste des commandes (`parse_tokens`)

Une fois la syntaxe jugée valide, la liste des commandes est générée. Chaque nœud `t_cmd` contiendra une liste d'arguments (`cmd_args`) et une liste de redirections associées (`redir_cmd`).

```mermaid
flowchart TD
    StartParse(["Début : parse_tokens(tokens)"]) --> InitHead["head = NULL"]
    InitHead --> LoopParse{"Tokens != NULL ?"}
    
    LoopParse -- Non --> ReturnHead(["Retourne la liste t_cmd construite"])
    LoopParse -- Oui --> BuildCmd["build_cmd() : création d'une nouvelle t_cmd"]
    
    BuildCmd --> InitCmd["init_cmd() : Alloue et initialise la node t_cmd"]
    InitCmd --> CountWords["count_cmd_words() : compte les tokens WORD avant le pipe"]
    CountWords --> MallocArgs["Alloue tableau cmd->cmd_args"]
    
    MallocArgs --> FillCmdArgs["fill_cmd_args() : Remplit les commandes et les redirections"]
    FillCmdArgs --> LoopFill{"Token actuel et pas de PIPE ?"}
    
    LoopFill -- Oui --> CheckFillType{"Type du token ?"}
    CheckFillType -- WORD --> AddArg["Copie le mot dans cmd_args"]
    CheckFillType -- REDIR --> AddRedir["add_redir() : ajoute un t_redir à la liste cmd->redir_cmd"]
    
    AddArg --> AdvToken1["Avance le pointeur du token *tok"]
    AddRedir --> AdvToken2["Avance la cible du *tok puis le token actuel"]
    
    AdvToken1 --> LoopFill
    AdvToken2 --> LoopFill
    
    LoopFill -- Non --> EndFill("Arguments et redirs stockés")
    
    EndFill --> CheckBuildSucces{"Erreur de malloc/construction ?"}
    CheckBuildSucces -- Oui --> FreeAll["free_cmd() : Libère l'arbre partiel"]
    FreeAll --> ReturnBuildNull(["Retourne NULL"])
    
    CheckBuildSucces -- Non --> CmdAddBack["cmd_add_back() : Attache le t_cmd à la liste doublement chaînée"]
    CmdAddBack --> SkipPipe{"Le token de fin est un TOKEN_PIPE ?"}
    
    SkipPipe -- Oui --> AdvPipe["tokens = tokens->next"]
    SkipPipe -- Non --> ReturnToLoopParse
    
    AdvPipe --> LoopParse
    ReturnToLoopParse --> LoopParse
```
