/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 11:52:23 by agard             #+#    #+#             */
/*   Updated: 2026/05/06 09:42:08 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_heredoc_error(t_cmd *node, t_mini *mini, int pipe_status)
{
	if (ft_heredoc(node->redir_cmd, mini) < 0)
	{
		mini->exit_status = 130;
		while (waitpid(-1, NULL, 0) != -1)
			;
		if (pipe_status != -1)
			close(pipe_status);
		return (-1);
	}
	return (0);
}

static int	execute_pipeline(t_mini *mini)
{
	int		pids_count;
	pid_t	last_pid;
	int		status;

	set_signal_mode(SIG_MODE_EXECUTION);
	last_pid = -1;
	pids_count = fork_all_commands(mini, &last_pid);
	if (pids_count < 0)
	{
		set_signal_mode(SIG_MODE_INTERACTIVE);
		return (mini->exit_status);
	}
	status = wait_all_children(pids_count, last_pid);
	set_signal_mode(SIG_MODE_INTERACTIVE);
	mini->exit_status = status;
	return (status);
}

int	pipeline(t_mini *mini)
{
	execute_pipeline(mini);
	return (mini->exit_status);
}
