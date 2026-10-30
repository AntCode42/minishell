/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_fork.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agard <agard@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 11:50:21 by agard             #+#    #+#             */
/*   Updated: 2026/04/23 12:34:10 by agard            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*find_path(t_env *env, int flag)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (flag == 1)
			if (ft_strncmp(tmp->key, "PATH", ft_strlen("PATH")) == 0)
				return (tmp);
		if (flag == 2)
			if (ft_strncmp(tmp->key, "PWD", ft_strlen("PWD")) == 0)
				return (tmp);
		if (flag == 3)
			if (ft_strncmp(tmp->key, "OLDPWD", ft_strlen("OLDPWD")) == 0)
				return (tmp);
		tmp = tmp->next;
	}
	return (tmp);
}

static char	**join_cmd(char **path_array, char *cmd)
{
	int		i;
	char	*tmp;

	i = 0;
	while (path_array[i])
	{
		tmp = path_array[i];
		path_array[i] = ft_strjoin(tmp, cmd);
		free(tmp);
		i++;
	}
	return (path_array);
}

char	**env_to_array(t_env *env)
{
	t_env	*node;
	char	*tmp;
	char	**array;
	int		nbr_node;
	int		i;

	node = env;
	nbr_node = count_node(env);
	array = ft_calloc(nbr_node + 1, sizeof(char *));
	if (!array)
		return (NULL);
	i = 0;
	while (i < nbr_node)
	{
		tmp = ft_strjoin(node->key, "=");
		array[i] = ft_strjoin(tmp, node->value);
		free(tmp);
		node = node->next;
		i++;
	}
	return (array);
}

static char	*find_exec_path(char **path_array)
{
	int		i;
	char	*res;

	i = 0;
	res = NULL;
	while (path_array && path_array[i])
	{
		if (access(path_array[i], X_OK) == 0)
		{
			res = ft_strdup(path_array[i]);
			break ;
		}
		i++;
	}
	return (res);
}

char	*path(t_env *env, char *cmd, int flag)
{
	t_env	*path_node;
	char	**path_array;
	char	*tmp;
	char	*res;

	path_node = find_path(env, flag);
	if (!path_node || !path_node->value)
		return (ft_strdup(cmd));
	path_array = ft_split(path_node->value, ':');
	tmp = ft_strjoin("/", cmd);
	path_array = join_cmd(path_array, tmp);
	free(tmp);
	res = find_exec_path(path_array);
	free_array(path_array);
	return (res);
}
