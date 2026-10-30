/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tilde_expander.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 15:12:59 by chaillo           #+#    #+#             */
/*   Updated: 2026/04/23 10:44:22 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Vérifie si le tilde (~) doit être développé
static int	should_expand_tilde(char *str, int i)
{
	if (!str || str[i] != '~')
		return (0);
	if (i != 0)
		return (0);
	if (str[i + 1] && str[i + 1] != '/')
		return (0);
	return (1);
}

// Remplace le tilde (~) par la valeur de HOME si possible
char	*expand_tilde(char *str, int *i, char *res, t_mini *mini)
{
	char	*home;
	char	*tmp;

	if (!str || !i || !mini)
		return (res);
	if (!should_expand_tilde(str, *i))
	{
		res = append_char(res, str[*i]);
		(*i)++;
		return (res);
	}
	home = get_env_val(mini->env, "HOME");
	if (!home)
	{
		res = append_char(res, str[*i]);
		(*i)++;
		return (res);
	}
	tmp = ft_strdup(home);
	(*i)++;
	return (join_and_free(res, tmp));
}
