/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 16:49:42 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 15:11:41 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pipeline(t_mini *mini)
{
	int	ret;

	ret = ext_func_finder(mini->cmd->cmd_args);
	if (ret == 0)
	{
		execute_pipeline(mini);
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

int execute_pipeline(t_mini *mini)
{
	int pids_count;
	pid_t last_pid;
	int status;

	pids_count = fork_all_commands(mini, &last_pid);
	status = wait_all_children(pids_count, last_pid);
	mini->exit_status = status;
	return (status);
}

int wait_all_children(int count, pid_t last_pid)
{
	int status;
	int i;

	i = 0;
	while (i < count - 1)
	{
		wait(NULL);
		i++;
	}
	waitpid(last_pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (1);
}

int	go_through_pipe(t_cmd *node, t_env *env, int *pipefd, int pipe_status)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
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
		execve(path(env, node->cmd_args[0]), node->cmd_args,
			env_to_array(env));
	}
	return(pid);
}

int ck_pid(int *pipefd, int pipe_status, t_cmd *node)
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

