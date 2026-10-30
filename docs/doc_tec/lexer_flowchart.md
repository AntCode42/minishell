# Lexer Flowchart

Ce document détaille le fonctionnement du lexer du minishell. Le lexer est responsable de la transformation de la chaîne de caractères brute (l'input de l'utilisateur) en une liste structurée de tokens (`t_token`).

## Algorithme principal

```mermaid
flowchart TD
    Start(["Début : lexer(char *input)"]) --> CheckInput{"input == NULL ?"}
    CheckInput -- Oui --> ReturnNull(["Retourne NULL"])
    CheckInput -- Non --> Init["Initialise tokens = NULL <br> i = 0"]
    
    Init --> LoopStart{"input[i] != '\0' ?"}
    
    LoopStart -- Non --> ReturnTokens(["Retourne la liste de tokens"])
    LoopStart -- Oui --> SkipSpaces["skip_spaces() : ignore ' ' et '\t'"]
    
    SkipSpaces --> CheckEOF{"input[i] == '\0' ?"}
    CheckEOF -- Oui --> ReturnTokens
    CheckEOF -- Non --> CheckType{"Est-ce un opérateur ? <br/> '|', '<', '>'"}
    
    CheckType -- Oui --> ReadOperator["read_operator() : lit l'opérateur"]
    CheckType -- Non --> ReadWord["read_word() : lit un mot jusqu'au prochain espace ou opérateur"]
    
    ReadOperator --> CheckNewToken{"Nouveau token créé ?"}
    ReadWord --> CheckNewToken
    
    CheckNewToken -- Erreur / NULL --> FreeTokens["free_tokens() : libère la mémoire"]
    FreeTokens --> ReturnNull
    
    CheckNewToken -- Succès --> AddToken["add_token() : ajoute le token à la fin de la liste"]
    AddToken --> LoopStart
```

## Détail : `read_word`

Cette fonction extrait un mot complet, en gérant le cas où le mot est entre guillemets simples ou doubles.

```mermaid
flowchart TD
    StartWord(["Début : read_word() "]) --> SaveStart["start = i"]
    SaveStart --> LoopWord{"Caractère courant valide ? <br/> Ni espace, ni tab, ni opérateur"}
    
    LoopWord -- Non --> Substr["ft_substr() : extrait la chaîne du token"]
    LoopWord -- Oui --> CheckQuote{"Est-ce une quote ? <br/> '\'' ou '\"'"}
    
    CheckQuote -- Non --> Incri1["i++"]
    CheckQuote -- Oui --> SkipQuotes["Sauvegarde quote, i++, avance i jusqu'à la quote fermante, puis i++"]
    
    Incri1 --> LoopWord
    SkipQuotes --> LoopWord
    
    Substr --> CheckSubstr{"Sous-chaîne OK ?"}
    CheckSubstr -- Non --> ReturnNullWord(["Retourne NULL"])
    CheckSubstr -- Oui --> CreateTokenWord["create_token(value, TOKEN_WORD)"]
    CreateTokenWord --> ReturnWord(["Retourne le token WORD"])
```

## Détail : `read_operator`

Gère la détection des différents tokens spéciaux (`|`, `<`, `>`, `<<`, `>>`).

```mermaid
flowchart LR
    StartOp(["Début : read_operator() "]) --> CheckPipe{"Est-ce '|' ?"}
    CheckPipe -- Oui --> TokenPipe["Crée TOKEN_PIPE"]
    CheckPipe -- Non --> CheckAppend{"Est-ce '>>' ?"}
    
    CheckAppend -- Oui --> TokenAppend["Crée TOKEN_APPEND"]
    CheckAppend -- Non --> CheckRedirOut{"Est-ce '>' ?"}
    
    CheckRedirOut -- Oui --> TokenRedirOut["Crée TOKEN_REDIR_OUT"]
    CheckRedirOut -- Non --> CheckHeredoc{"Est-ce '<<' ?"}
    
    CheckHeredoc -- Oui --> TokenHeredoc["Crée TOKEN_HEREDOC"]
    CheckHeredoc -- Non --> CheckRedirIn{"Est-ce '<' ?"}
    
    CheckRedirIn -- Oui --> TokenRedirIn["Crée TOKEN_REDIR_IN"]
    CheckRedirIn -- Non --> ReturnNullOp(["Retourne NULL"])
    
    TokenPipe --> ReturnToken(["Retourne token"])
    TokenAppend --> ReturnToken
    TokenRedirOut --> ReturnToken
    TokenHeredoc --> ReturnToken
    TokenRedirIn --> ReturnToken
```
