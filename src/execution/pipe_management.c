/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_management.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 16:49:42 by agard             #+#    #+#             */
/*   Updated: 2026/05/06 22:08:05 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_child_builtin(t_mini *mini, t_cmd *node, t_cmd *head)
{
	int	exit_status;

	mini->cmd = node;
	builtin_sort(mini);
	exit_status = mini->exit_status;
	free_env(mini->env);
	free_cmd(head);
	exit(exit_status);
}

static void	child_exec_error(t_mini *mini, char *cmd_path,
	char	**env_arr, t_cmd *head)
{
	int			err;
	struct stat	st;

	err = errno;
	if (stat(cmd_path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(mini->cmd->cmd_args[0], 2);
		ft_putstr_fd(": est un dossier\n", 2);
		free(cmd_path);
		free_array(env_arr);
		free_env(mini->env);
		free_cmd(head);
		exit(126);
	}
	ft_putstr_fd("minishell: ", 2);
	perror(mini->cmd->cmd_args[0]);
	free(cmd_path);
	free_array(env_arr);
	free_env(mini->env);
	free_cmd(head);
	if (err == EACCES)
		exit(126);
	exit(127);
}

static void	handle_child_exec(t_mini *mini, t_cmd *node, t_cmd *head)
{
	char	*cmd_path;
	char	**env_arr;

	if (node->cmd_args[0][0] == '\0')
		cmd_path = NULL;
	else if (node->cmd_args[0][0] == '/' || node->cmd_args[0][0] == '.')
		cmd_path = ft_strdup(node->cmd_args[0]);
	else
		cmd_path = path(mini->env, node->cmd_args[0], 1);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(node->cmd_args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		free_env(mini->env);
		free_cmd(head);
		exit(127);
	}
	env_arr = env_to_array(mini->env);
	execve(cmd_path, node->cmd_args, env_arr);
	child_exec_error(mini, cmd_path, env_arr, head);
}

static void	run_child(t_mini *mini, t_cmd *node, int *pipefd, int pipe_status)
{
	int		saved_fd;
	t_cmd	*head;

	set_signal_mode(SIG_MODE_DEFAULT);
	dir_pipe(node, pipefd, pipe_status);
	ft_redir(node->redir_cmd);
	saved_fd = dup(1);
	dup2(saved_fd, 1);
	close(saved_fd);
	head = mini->cmd;
	if (!node->cmd_args || !node->cmd_args[0])
	{
		free_env(mini->env);
		free_cmd(head);
		exit(0);
	}
	if (ext_func_finder(node->cmd_args) == 1)
		handle_child_builtin(mini, node, head);
	handle_child_exec(mini, node, head);
}

int	go_through_pipe(t_mini *mini, t_cmd *node, int *pipefd, int pipe_status)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
		run_child(mini, node, pipefd, pipe_status);
	return (pid);
}
