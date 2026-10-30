/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoine-pro <antoine-pro@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:00:33 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/10 11:29:30 by antoine-pro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Gère l'expansion des variables d'environnement (ex: $VAR ou $?)
static char	*expand_dollar(char *str, int *i, char *res, t_mini *mini)
{
	(*i)++;
	if (str[*i] == '?')
		return (expand_exit_status(res, mini, i));
	if (!ft_isalnum(str[*i]) && str[*i] != '_')
		return (append_char(res, '$'));
	return (expand_env_key(str, i, res, mini));
}

// Gère l'intérieur des doubles guillemets (permet l'expansion de $)
static char	*handle_dquote(char *str, int *i, char *res, t_mini *mini)
{
	(*i)++;
	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '$')
			res = expand_dollar(str, i, res, mini);
		else
		{
			res = append_char(res, str[*i]);
			(*i)++;
		}
	}
	if (str[*i] == '"')
		(*i)++;
	return (res);
}

// Gère l'intérieur des simples guillemets (pas d'expansion)
static char	*handle_squote(char *str, int *i, char *res)
{
	(*i)++;
	while (str[*i] && str[*i] != '\'')
		res = append_char(res, str[(*i)++]);
	if (str[*i] == '\'')
		(*i)++;
	return (res);
}

// Parcourt un mot (variables, tilde) et enlève les guillemets
static char	*expand_word(char *word, t_mini *mini)
{
	char	*res;
	int		i;

	res = ft_strdup("");
	i = 0;
	while (word && word[i])
	{
		if (word[i] == '\'')
			res = handle_squote(word, &i, res);
		else if (word[i] == '"')
			res = handle_dquote(word, &i, res, mini);
		else if (word[i] == '~')
			res = expand_tilde(word, &i, res, mini);
		else if (word[i] == '$')
			res = expand_dollar(word, &i, res, mini);
		else
			res = append_char(res, word[i++]);
	}
	return (res);
}

// Boucle principale de l'expander : parcourt et développe tous les tokens WORD
void	expand_tokens(t_token *tokens, t_mini *mini)
{
	t_token	*tmp;
	char	*original;
	char	*new_val;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == TOKEN_WORD)
		{
			original = tmp->value;
			new_val = expand_word(tmp->value, mini);
			if (new_val[0] == '\0' && is_unquoted_var(original))
				tmp->type = TOKEN_EMPTY;
			free(original);
			tmp->value = new_val;
		}
		tmp = tmp->next;
	}
}
