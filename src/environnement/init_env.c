/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 17:45:43 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:07:46 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Crée un nœud d'environnement à partir d'une chaîne "CLE=VALEUR"
static t_env	*create_env_node(char *env_str)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	split_key_value(env_str, &node->key, &node->value);
	if (!node->key && env_str)
	{
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
}

// Incrémente le niveau du shell (SHLVL) à l'initialisation de l'environnement
static void	update_shlvl(t_env **env)
{
	t_env	*shlvl;
	int		val;

	shlvl = find_env_node(*env, "SHLVL");
	if (shlvl && shlvl->value)
	{
		val = ft_atoi(shlvl->value) + 1;
		free(shlvl->value);
		shlvl->value = ft_itoa(val);
	}
	else if (shlvl && !shlvl->value)
		shlvl->value = ft_strdup("1");
	else
		append_env_node(env, new_env_node("SHLVL", "1"));
}

// Initialise la structure liste chaînée d'environnement à partir de envp
t_env	*init_env(char **envp)
{
	t_env	*env;
	t_env	*new;
	int		i;
	char	cwd[1024];

	env = NULL;
	if (!envp || !envp[0])
	{
		if (getcwd(cwd, sizeof(cwd)))
			append_env_node(&env, new_env_node("PWD", cwd));
	}
	else
	{
		i = 0;
		while (envp[i])
		{
			new = create_env_node(envp[i]);
			if (new)
				append_env_node(&env, new);
			i++;
		}
	}
	update_shlvl(&env);
	return (env);
}

// Libère entièrement la mémoire allouée à la liste d'environnement
void	free_env(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env->next;
		free(env->key);
		if (env->value)
			free(env->value);
		free(env);
		env = tmp;
	}
}
