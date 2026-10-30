/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_management.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoine-pro <antoine-pro@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 19:08:44 by antoine-pro       #+#    #+#             */
/*   Updated: 2026/05/10 11:17:14 by antoine-pro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	restore_stdin(void)
{
	int	fd;

	fd = open("/dev/tty", O_RDONLY);
	if (fd < 0)
		return ;
	if (fd != STDIN_FILENO)
	{
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
}

static int	handle_heredoc_eof(t_redir *redir)
{
	if (g_signal_received != SIGINT)
	{
		ft_putstr_fd("minishell: warning: here-document ", 2);
		ft_putstr_fd("delimited by end-of-file (wanted `", 2);
		ft_putstr_fd(redir->file, 2);
		ft_putstr_fd("`)\n", 2);
		return (0);
	}
	return (-1);
}

static int	read_heredoc(int pipefd[2], t_redir *redir, t_mini *mini)
{
	char	*line;
	char	*expanded;

	line = readline("> ");
	if (line == NULL)
		return (handle_heredoc_eof(redir));
	if (ft_strcmp(line, redir->file) == 0)
	{
		free(line);
		return (0);
	}
	if (!redir->quoted)
	{
		expanded = expand_heredoc_line(line, mini);
		free(line);
		line = expanded;
	}
	write(pipefd[1], line, ft_strlen(line));
	write(pipefd[1], "\n", 1);
	free(line);
	return (1);
}

static int	process_heredoc_loop(int pipefd[2], t_redir *redir, t_mini *mini)
{
	int	check;

	check = 1;
	while (check > 0)
	{
		check = read_heredoc(pipefd, redir, mini);
		if (check < 0)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			restore_stdin();
			if (mini)
				mini->exit_status = 130;
			set_signal_mode(SIG_MODE_INTERACTIVE);
			return (-1);
		}
	}
	return (0);
}

int	ft_heredoc(t_redir *redir, t_mini *mini)
{
	int		pipefd[2];

	if (!redir)
		return (1);
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			set_signal_mode(SIG_MODE_HEREDOC);
			pipe(pipefd);
			if (process_heredoc_loop(pipefd, redir, mini) == -1)
				return (-1);
			close(pipefd[1]);
			redir->int_fd = pipefd[0];
			set_signal_mode(SIG_MODE_EXECUTION);
		}
		redir = redir->next;
	}
	return (0);
}
