# Expander Flowchart

Ce document détaille le fonctionnement de l'expander du minishell. L'expander est chargé de parcourir les tokens générés par le lexer pour remplacer les variables d'environnement (`$VAR` ou `$?`) par leur valeur, et de gérer la suppression ou l'interprétation des guillemets (quotes simples `'` et doubles `"`).

## 1. Boucle principale (`expand_tokens`)

L'expander parcourt la liste des tokens via `expand_tokens` et n'applique l'expansion que sur les tokens de type `TOKEN_WORD`.

```mermaid
flowchart TD
    Start(["Début : expand_tokens(tokens, mini)"]) --> Init["tmp = tokens"]
    
    Init --> LoopStart{"tmp != NULL ?"}
    LoopStart -- Non --> End(["Fin de l'expansion"])
    
    LoopStart -- Oui --> CheckType{"tmp->type == TOKEN_WORD ?"}
    
    CheckType -- Non --> NextToken["tmp = tmp->next"]
    
    CheckType -- Oui --> RunExpand["new_val = expand_word(tmp->value, mini)"]
    
    RunExpand --> FreeOld["free(tmp->value)"]
    FreeOld --> AssignNew["tmp->value = new_val"]
    AssignNew --> NextToken
    
    NextToken --> LoopStart
```

## 2. Expansion d'un mot (`expand_word`)

La fonction lit le mot caractère par caractère et agit différemment selon le caractère rencontré (guillemets simples, guillemets doubles, ou dollar).

```mermaid
flowchart TD
    StartWord(["Début : expand_word(word, mini)"]) --> InitRes["res = &quot;&quot; <br> i = 0"]
    
    InitRes --> LoopWord{"word[i] != '\0' ?"}
    LoopWord -- Non --> ReturnRes(["Retourne res"])
    
    LoopWord -- Oui --> CheckChar{"Caractère word[i] ?"}
    
    CheckChar -- " '\'' (simple quote) " --> HandleSQuote["Avance manuellement jsq quote fermante, <br> ajoute les caractères bruts dans res sans expanser"]
    HandleSQuote --> LoopWord
    
    CheckChar -- " '&quot;' (double quote) " --> HandleDQuote["res = handle_dquote(word, &i, res, mini)"]
    HandleDQuote --> LoopWord
    
    CheckChar -- " '$' (dollar) " --> HandleDollar["res = expand_dollar(word, &i, res, mini)"]
    HandleDollar --> LoopWord
    
    CheckChar -- " Autre caractère " --> AppendChar["res = append_char(res, word[i++])"]
    AppendChar --> LoopWord
```

## 3. Gestion des Double Quotes (`handle_dquote`)

À l'intérieur des doubles quotes, les variables (`$`) sont expansées, tandis que les autres caractères sont conservés fidèlement.

```mermaid
flowchart TD
    StartDQ(["Début : handle_dquote(str, &i, res, mini)"]) --> IncI["Avance après le '&quot;' (i++)"]
    
    IncI --> LoopDQ{"str[i] existe et != '&quot;' ?"}
    LoopDQ -- Non --> SkipCloseQuote["Si str[i] == '&quot;', avance (i++)"]
    SkipCloseQuote --> ReturnResDQ(["Retourne res"])
    
    LoopDQ -- Oui --> CheckDQChar{"str[i] == '$' ?"}
    
    CheckDQChar -- Oui --> ExpandDQDollar["res = expand_dollar(str, &i, res, mini)"]
    ExpandDQDollar --> LoopDQ
    
    CheckDQChar -- Non --> AppendDQChar["res = append_char(res, str[i]) <br> i++"]
    AppendDQChar --> LoopDQ
```

## 4. Expansion des Variables (`expand_dollar`)

Remplace la variable (ex: `$?` ou `$VAR`) par sa valeur extraite de l'environnement (ou par `""` si elle est introuvable).

```mermaid
flowchart TD
    StartDollar(["Début : expand_dollar(str, &i, res, mini)"]) --> IncIDollar["Avance après le '$' (i++)"]
    
    IncIDollar --> CheckQuestion{"str[i] == '?' ?"}
    
    CheckQuestion -- Oui --> ExpandStatus["val = ft_itoa(mini->exit_status) <br> i++"]
    ExpandStatus --> JoinStatus["join_and_free(res, val)"]
    JoinStatus --> ReturnStatus(["Retourne nouvelle chaîne expansée"])
    
    CheckQuestion -- Non --> CheckAlnum{"str[i] est alphanumérique <br> ou '_' ?"}
    
    CheckAlnum -- Non --> AppendDollarChar["append_char(res, '$')"]
    AppendDollarChar --> ReturnLitteralDollar(["Retourne le '$' litéral"])
    
    CheckAlnum -- Oui --> ExtractVar["Extrait le nom de la variable (ft_substr) <br> Avance i jusqu'à la fin du nom"]
    
    ExtractVar --> GetEnvVal["val = get_env_val(mini->env, var_name)"]
    GetEnvVal --> CheckVal{"val existe ?"}
    
    CheckVal -- Oui --> DupVal["tmp = ft_strdup(val)"]
    CheckVal -- Non --> DupEmpty["tmp = ft_strdup(&quot;&quot;)"]
    
    DupVal --> JoinVar["join_and_free(res, tmp)"]
    DupEmpty --> JoinVar
    JoinVar --> FreeVarName["free(var_name)"]
    FreeVarName --> ReturnVar(["Retourne nouvelle chaîne expansée"])
```
