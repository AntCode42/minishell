/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 16:23:30 by agard             #+#    #+#             */
/*   Updated: 2026/05/06 22:08:05 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_cmd_path(t_mini *mini)
{
	char	*cmd_path;

	if (mini->cmd->cmd_args[0][0] == '\0')
		cmd_path = NULL;
	else if (mini->cmd->cmd_args[0][0] == '/'
		|| mini->cmd->cmd_args[0][0] == '.')
		cmd_path = ft_strdup(mini->cmd->cmd_args[0]);
	else
		cmd_path = path(mini->env, mini->cmd->cmd_args[0], 1);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(mini->cmd->cmd_args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		free_env(mini->env);
		free_cmd(mini->cmd);
		exit(127);
	}
	return (cmd_path);
}

static void	execute_external(t_mini *mini, char *cmd_path)
{
	char			**env_arr;
	int				err;
	struct stat		st;

	env_arr = env_to_array(mini->env);
	execve(cmd_path, mini->cmd->cmd_args, env_arr);
	err = errno;
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
		external(mini, env_arr, cmd_path);
	ft_putstr_fd("minishell: ", 2);
	perror(mini->cmd->cmd_args[0]);
	free(cmd_path);
	free_array(env_arr);
	free_env(mini->env);
	free_cmd(mini->cmd);
	if (err == EACCES)
		exit(126);
	exit(127);
}

static void	ex_fork(t_mini *mini)
{
	char	*cmd_path;

	set_signal_mode(SIG_MODE_DEFAULT);
	ft_redir(mini->cmd->redir_cmd);
	if (!mini->cmd->cmd_args || !mini->cmd->cmd_args[0])
	{
		free_env(mini->env);
		free_cmd(mini->cmd);
		exit(0);
	}
	cmd_path = get_cmd_path(mini);
	execute_external(mini, cmd_path);
}

int	create_single_fork(t_mini *mini)
{
	pid_t	pid;
	int		status;

	set_signal_mode(SIG_MODE_EXECUTION);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		set_signal_mode(SIG_MODE_INTERACTIVE);
		return (1);
	}
	if (pid == 0)
	{
		ex_fork(mini);
	}
	waitpid(pid, &status, 0);
	mini->exit_status = wait_status_to_exit_code(status);
	set_signal_mode(SIG_MODE_INTERACTIVE);
	return (mini->exit_status);
}

int	fork_all_commands(t_mini *mini, pid_t *last_pid)
{
	t_cmd	*node;
	pid_t	pid;
	int		pipe_status;
	int		pipefd[2];
	int		count;

	node = mini->cmd;
	pipe_status = -1;
	count = 0;
	while (node != NULL)
	{
		if (check_heredoc_error(node, mini, pipe_status) == -1)
			return (-1);
		if (node->next != NULL)
			pipe(pipefd);
		pid = go_through_pipe(mini, node, pipefd, pipe_status);
		*last_pid = pid;
		count++;
		pipe_status = ck_pid(pipefd, pipe_status, node);
		node = node->next;
	}
	return (count);
}
