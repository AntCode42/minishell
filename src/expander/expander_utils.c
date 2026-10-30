/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 10:59:39 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 21:13:16 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Concatène deux chaînes et libère la mémoire
char	*join_and_free(char *s1, char *s2)
{
	char	*res;

	if (!s1)
		s1 = ft_strdup("");
	if (!s2)
		s2 = ft_strdup("");
	res = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (res);
}

// Ajoute un caractère à la fin de la chaîne et réalloue la mémoire
char	*append_char(char *str, char c)
{
	char	*res;
	int		len;

	len = 0;
	if (str)
		len = ft_strlen(str);
	res = malloc(len + 2);
	if (!res)
		return (NULL);
	if (str)
	{
		ft_strlcpy(res, str, len + 1);
		free(str);
	}
	res[len] = c;
	res[len + 1] = '\0';
	return (res);
}

// Cherche la valeur associée à la clé dans les variables d'environnement
char	*get_env_val(t_env *env, char *key)
{
	int	len;

	len = ft_strlen(key);
	while (env)
	{
		if (!ft_strncmp(env->key, key, len + 1))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

char	*expand_exit_status(char *res, t_mini *mini, int *i)
{
	char	*status;

	status = ft_itoa(mini->exit_status);
	(*i)++;
	return (join_and_free(res, status));
}

char	*expand_env_key(char *line, int *i, char *res, t_mini *mini)
{
	int		start;
	char	*key;
	char	*val;
	char	*tmp;

	start = *i;
	while (line[*i] && (ft_isalnum(line[*i]) || line[*i] == '_'))
		(*i)++;
	key = ft_substr(line, start, *i - start);
	val = get_env_val(mini->env, key);
	free(key);
	if (val)
		tmp = ft_strdup(val);
	else
		tmp = ft_strdup("");
	return (join_and_free(res, tmp));
}
