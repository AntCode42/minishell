/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_pipe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 11:47:46 by agard             #+#    #+#             */
/*   Updated: 2026/05/05 21:23:16 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	dir_pipe(t_cmd *node, int *pipefd, int pipe_status)
{
	if (pipe_status != -1)
	{
		dup2(pipe_status, 0);
		close(pipe_status);
	}
	if (node->next != NULL)
	{
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		close(pipefd[0]);
	}
}

int	wait_all_children(int count, pid_t last_pid)
{
	int	status;
	int	exit_status;
	int	i;
	int	other_signaled;
	int	last_signaled;

	other_signaled = 0;
	last_signaled = 0;
	waitpid(last_pid, &status, 0);
	if (WIFSIGNALED(status))
		last_signaled = WTERMSIG(status);
	exit_status = wait_status_to_exit_code(status);
	i = 0;
	while (i < count - 1)
	{
		wait(&status);
		if (WIFSIGNALED(status) && !other_signaled)
			other_signaled = WTERMSIG(status);
		i++;
	}
	if (!last_signaled && other_signaled == SIGINT)
		write(1, "\n", 1);
	else if (!last_signaled && other_signaled == SIGQUIT)
		write(1, "Quit (core dumped)\n", 19);
	return (exit_status);
}

int	ck_pid(int *pipefd, int pipe_status, t_cmd *node)
{
	if (pipe_status != -1)
		close(pipe_status);
	if (node->next != NULL)
	{
		close(pipefd[1]);
		pipe_status = pipefd[0];
	}
	return (pipe_status);
}
