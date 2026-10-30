/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 09:48:54 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 15:23:43 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ck_pipe(t_mini *mini)
{
	if (!mini->cmd)
		return(1);
	if (mini->cmd->next != NULL)
		pipeline(mini); // to_build
	else
		single(mini);
	return(mini->exit_status);
}

int	ext_func_finder(char **list_cmd)
{
	int		i;
	char	*inter[] = {"echo", "cd", "pwd", "export", "unset", "env", "exit",
						NULL};

	if (!list_cmd || !list_cmd[0])
		return (2);
	i = 0;
	while (inter[i] != NULL)
	{
		if (ft_strncmp(list_cmd[0], inter[i], ft_strlen(list_cmd[0]) + 1) == 0)
			return (1);
		i++;
	}
		return (0);
}
int	single(t_mini *mini)
{
	int	ret;

	ret = ext_func_finder(mini->cmd->cmd_args);
	if (ret == 0)
	{
		create_single_fork(mini);
	}
/******************************** */
/* for built in function*/
	// else if (ret == 1)
	// {
	// 	builtin_sort(mini);
	// }
	else
	{
		return(ret);
	}
	return (mini->exit_status);
}
