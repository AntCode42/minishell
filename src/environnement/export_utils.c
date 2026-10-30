/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 12:22:43 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:02:46 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Sépare "KEY=VALUE" ou "KEY" en deux chaînes distinctes
void	split_key_value(const char *str, char **key, char **value)
{
	char	*egal_pos;

	if (!key || !value)
		return ;
	*key = NULL;
	*value = NULL;
	if (!str)
		return ;
	egal_pos = ft_strchr(str, '=');
	if (egal_pos)
	{
		*key = ft_substr(str, 0, egal_pos - str);
		*value = ft_strdup(egal_pos + 1);
	}
	else
	{
		*key = ft_strdup(str);
		*value = NULL;
	}
}

// Vérifie si le nom de clé fourni est valide pour une variable d'environnement
int	is_valid_key(const char *key)
{
	int	i;

	if (!key || (!ft_isalpha(key[0]) && key[0] != '_'))
		return (0);
	i = 1;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

// Trouve un noeud par sa clé dans la liste d'environnement
t_env	*find_env_node(t_env *env, const char *key)
{
	while (env)
	{
		if (ft_strncmp(env->key, key, ft_strlen(key) + 1) == 0)
			return (env);
		env = env->next;
	}
	return (NULL);
}

// Ajoute un nouveau noeud à la fin de la liste d'environnement
void	append_env_node(t_env **env, t_env *new_node)
{
	t_env	*tmp;

	if (!*env)
	{
		*env = new_node;
		return ;
	}
	tmp = *env;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_node;
}

// Crée un nouveau noeud pour la liste d'environnement (clé et valeur)
t_env	*new_env_node(const char *key, const char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (!node->key)
	{
		free(node);
		return (NULL);
	}
	if (value)
		node->value = ft_strdup(value);
	else
		node->value = NULL;
	node->next = NULL;
	return (node);
}
