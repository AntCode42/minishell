/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 10:13:40 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:05:46 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Sépare "KEY=VALUE" ou "KEY" en deux chaînes distinctes
static void	extract_key_value(const char *arg, char **key, char **value)
{
	char	*normalized;

	split_key_value(arg, key, value);
	if (*value)
	{
		normalized = normalize_spaces(*value);
		if (normalized)
		{
			free(*value);
			*value = normalized;
		}
	}
}

// Met à jour une variable d'environnement ou la crée si elle n'existe pas
static int	update_or_add_env(t_mini *mini, char *key, char *value)
{
	t_env	*existing;
	t_env	*node;

	existing = find_env_node(mini->env, key);
	if (existing)
	{
		if (value)
		{
			free(existing->value);
			existing->value = value;
		}
		free(key);
	}
	else
	{
		node = new_env_node(key, value);
		free(key);
		if (value)
			free(value);
		if (!node)
			return (1);
		append_env_node(&mini->env, node);
	}
	return (0);
}

// Effectue l'exportation pour une seule clé / variable
static int	export_one_key(t_mini *mini, const char *arg)
{
	char	*key;
	char	*value;

	extract_key_value(arg, &key, &value);
	if (!key)
		return (1);
	if (!is_valid_key(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd((char *)arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		free(key);
		if (value)
			free(value);
		return (1);
	}
	return (update_or_add_env(mini, key, value));
}

// Commande export (affiche les variables si aucun argument)
int	ft_export(t_mini *mini)
{
	char	**args;
	int		i;
	int		ret;

	if (!mini || !mini->cmd || !mini->cmd->cmd_args)
		return (1);
	args = mini->cmd->cmd_args;
	if (!args[1])
	{
		print_export(mini->env);
		return (0);
	}
	ret = 0;
	i = 1;
	while (args[i])
	{
		if (export_one_key(mini, args[i]) != 0)
			ret = 1;
		i++;
	}
	return (ret);
}
