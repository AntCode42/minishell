/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 20:48:42 by gchaillo          #+#    #+#             */
/*   Updated: 2026/04/23 10:44:22 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Affiche les variables d'environnement possédant une valeur
int	ft_env(t_mini *mini)
{
	t_env	*tmp;

	if (!mini || !mini->env)
		return (1);
	tmp = mini->env;
	while (tmp)
	{
		if (tmp->value)
		{
			ft_putstr_fd(tmp->key, 1);
			ft_putchar_fd('=', 1);
			ft_putstr_fd(tmp->value, 1);
			ft_putchar_fd('\n', 1);
		}
		tmp = tmp->next;
	}
	return (0);
}
