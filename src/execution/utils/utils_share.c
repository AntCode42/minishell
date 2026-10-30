/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_share.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 15:04:00 by antoine-pro       #+#    #+#             */
/*   Updated: 2026/05/05 16:52:10 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while ((s1[i] != '\0') || (s2[i] != '\0'))
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (s1[i] - s2[i]);
}

char	*ft_strcat(char *dest, char *src)
{
	char	*res;

	if (!dest)
		return (ft_strdup(src));
	if (!src)
		return (dest);
	res = ft_strjoin(dest, src);
	free(dest);
	return (res);
}

void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

int	count_node(t_env *env)
{
	t_env	*runner;
	int		i;

	if (!env)
		return (0);
	runner = env;
	i = 0;
	while (runner)
	{
		runner = runner->next;
		i++;
	}
	return (i);
}

int	external(t_mini *mini, char **env_arr, char *cmd_path)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(mini->cmd->cmd_args[0], 2);
	ft_putstr_fd(": est un dossier\n", 2);
	free(cmd_path);
	free_array(env_arr);
	free_env(mini->env);
	free_cmd(mini->cmd);
	exit(126);
}
