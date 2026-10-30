/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:23:30 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 15:12:18 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int create_single_fork(t_mini *mini)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid < 0) {
		perror("fork");
	return (1);
	}
	if (pid == 0)
	{
		execve(path(mini->env, mini->cmd->cmd_args[0]), mini->cmd->cmd_args,
			env_to_array(mini->env));
	perror("execve");
	exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
	mini->exit_status = WEXITSTATUS(status);
	return (mini->exit_status);
}

int fork_all_commands(t_mini *mini, pid_t *last_pid)
{
	t_cmd *node;
	pid_t pid;
	int pipe_status; //
	int pipefd[2];
	int count;

	node = mini->cmd;
	pipe_status = -1;
	count = 0;
	while (node != NULL)
	{
		if (node->next != NULL)
			pipe(pipefd);
		pid = go_through_pipe(node, mini->env, pipefd, pipe_status);
		*last_pid = pid;
		count++;
		pipe_status = ck_pid(pipefd, pipe_status, node);
		node = node->next;
	}
	return (count);
}
