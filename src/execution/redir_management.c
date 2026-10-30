/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_management.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoine-pro <antoine-pro@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 17:50:35 by antoine-pro       #+#    #+#             */
/*   Updated: 2026/05/06 17:17:16 by antoine-pro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_redir_in(t_redir *redir)
{
	int	fd;

	fd = open(redir->file, O_RDONLY);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(redir->file);
		exit(1);
	}
	dup2(fd, 0);
	close(fd);
	return (0);
}

static int	ft_redir_out(t_redir *redir)
{
	int	fd;

	fd = open(redir->file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(redir->file);
		exit(1);
	}
	dup2(fd, 1);
	close(fd);
	return (0);
}

static int	ft_redir_append(t_redir *redir)
{
	int	fd;

	fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(redir->file);
		exit(1);
	}
	dup2(fd, 1);
	close(fd);
	return (0);
}

int	mk_heredoc(t_redir *redir)
{
	int		fd;

	fd = redir->int_fd;
	dup2(fd, 0);
	close(fd);
	return (0);
}

int	ft_redir(t_redir *redir)
{
	if (!redir)
		return (1);
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
			ft_redir_in(redir);
		else if (redir->type == TOKEN_REDIR_OUT)
			ft_redir_out(redir);
		else if (redir->type == TOKEN_APPEND)
			ft_redir_append(redir);
		else if (redir->type == TOKEN_HEREDOC)
			mk_heredoc(redir);
		redir = redir->next;
	}
	return (0);
}
