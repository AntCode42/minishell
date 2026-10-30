/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 15:20:34 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:11:46 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Supprime une variable d'environnement de la liste chaînée
static void	remove_env_node(t_env **env, const char *key)
{
	t_env	*prev;
	t_env	*curr;

	if (!env || !*env)
		return ;
	prev = NULL;
	curr = *env;
	while (curr)
	{
		if (ft_strncmp(curr->key, key, ft_strlen(key) + 1) == 0)
		{
			if (prev)
				prev->next = curr->next;
			else
				*env = curr->next;
			free(curr->key);
			if (curr->value)
				free(curr->value);
			free(curr);
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

// Implémente la commande unset : supprime chaque variable listée
int	ft_unset(t_mini *mini)
{
	char	**args;
	int		i;
	int		ret;

	if (!mini || !mini->cmd || !mini->cmd->cmd_args)
		return (1);
	args = mini->cmd->cmd_args;
	ret = 0;
	i = 1;
	while (args[i])
	{
		if (!is_valid_key(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", 2);
			ft_putstr_fd(args[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			ret = 1;
		}
		else
			remove_env_node(&mini->env, args[i]);
		i++;
	}
	return (ret);
}
