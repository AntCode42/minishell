/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 09:48:54 by agard             #+#    #+#             */
/*   Updated: 2026/05/05 16:48:51 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ext_func_finder(char **list_cmd)
{
	int		i;
	char	*inter[8];

	inter[0] = "echo";
	inter[1] = "cd";
	inter[2] = "pwd";
	inter[3] = "export";
	inter[4] = "unset";
	inter[5] = "env";
	inter[6] = "exit";
	inter[7] = NULL;
	if (!list_cmd || !list_cmd[0])
		return (2);
	if (!list_cmd[0])
		return (0);
	i = 0;
	while (inter[i] != NULL)
	{
		if (ft_strcmp(list_cmd[0], inter[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

static int	single(t_mini *mini)
{
	int	ret;

	if (ft_heredoc(mini->cmd->redir_cmd, mini) < 0)
		return (mini->exit_status);
	ret = ext_func_finder(mini->cmd->cmd_args);
	if (ret == 0)
	{
		create_single_fork(mini);
	}
	else if (ret == 1)
	{
		builtin_sort(mini);
	}
	else
	{
		mini->exit_status = 0;
	}
	return (mini->exit_status);
}

int	ck_pipe(t_mini *mini)
{
	if (!mini->cmd)
		return (1);
	if (mini->cmd->next != NULL)
		pipeline(mini);
	else
		single(mini);
	return (mini->exit_status);
}
