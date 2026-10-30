/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 10:59:39 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:42 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
coucou le ft_joint
*/
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

/*
ajoute un caractère à la fin d'une chaîne en réallouant correctement
retourne la nouvelle chaîne avec le caractère ajouté
*/
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

/*
recherche dans environnement
retourne pointeur vers la valeur ou NULL
*/
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
