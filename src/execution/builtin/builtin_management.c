/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_management.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agard <agard@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 12:15:33 by agard             #+#    #+#             */
/*   Updated: 2026/04/30 17:51:52 by agard            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_sort(t_mini *mini)
{
	char	**cmd;

	cmd = mini->cmd->cmd_args;
	if (ft_strcmp(cmd[0], "echo") == 0)
		mini->exit_status = ft_echo(mini);
	else if (ft_strcmp(cmd[0], "cd") == 0)
		mini->exit_status = ft_cd(mini);
	else if (ft_strcmp(cmd[0], "pwd") == 0)
		mini->exit_status = ft_pwd(mini);
	else if (ft_strcmp(cmd[0], "export") == 0)
		mini->exit_status = ft_export(mini);
	else if (ft_strcmp(cmd[0], "unset") == 0)
		mini->exit_status = ft_unset(mini);
	else if (ft_strcmp(cmd[0], "env") == 0)
		mini->exit_status = ft_env(mini);
	else if (ft_strcmp(cmd[0], "exit") == 0)
		mini->exit_status = ft_exit(mini);
	return (mini->exit_status);
}
