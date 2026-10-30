/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:00:33 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:45 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
gère si commence par $ ou $?
retourne la valeur ou remplace par vide si introuvable
*/
char	*expand_dollar(char *str, int *i, char *res, t_mini *mini)
{
	int		start;
	char	*var_name;
	char	*val;
	char	*tmp;

	(*i)++; //pour avancer après le $
	if (str[*i] == '?')
	{
		val = ft_itoa(mini->exit_status);
		(*i)++;
		return (join_and_free(res, val));
	}
	if (!ft_isalnum(str[*i]) && str[*i] != '_')
		return (append_char(res, '$'));
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	var_name = ft_substr(str, start, *i - start);
	val = get_env_val(mini->env, var_name);
	free(var_name);
	if (val)
		tmp = ft_strdup(val);
	else
		tmp = ft_strdup("");
	return (join_and_free(res, tmp));
}

/*
gère l'intérieur d'une double quote " " :
permet l'expansion avec $ et garde les autres caractères intacts
*/
char	*handle_dquote(char *str, int *i, char *res, t_mini *mini)
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

/*
parcourt un mot et gère les quotes simples, doubles, ou les $ à l'extérieur
retourne le mot entièrement expansé (ce mot existe ?!) sans les quotes
*/
char	*expand_word(char *word, t_mini *mini)
{
	char	*res;
	int		i;

	res = ft_strdup("");
	i = 0;
	while (word && word[i])
	{
		if (word[i] == '\'')
		{
			i++;
			while (word[i] && word[i] != '\'')
				res = append_char(res, word[i++]);
			if (word[i] == '\'')
				i++;
		}
		else if (word[i] == '"')
			res = handle_dquote(word, &i, res, mini);
		else if (word[i] == '$')
			res = expand_dollar(word, &i, res, mini);
		else
			res = append_char(res, word[i++]);
	}
	return (res);
}

/*
boucle de l'expander, tourne sur chaque tokens
il fonctionne que sur les WORDS
*/
void	expand_tokens(t_token *tokens, t_mini *mini)
{
	t_token	*tmp;
	char	*new_val;

	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == TOKEN_WORD)
		{
			new_val = expand_word(tmp->value, mini);
			free(tmp->value);
			tmp->value = new_val;
		}
		tmp = tmp->next;
	}
}
