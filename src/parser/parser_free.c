/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:35 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/23 16:13:37 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_cmd_redirs(t_cmd *cmd)
{
	t_redir	*tmp_redir;

	while (cmd->redir_cmd)
	{
		tmp_redir = cmd->redir_cmd->next;
		if (cmd->redir_cmd->int_fd >= 0)
			close(cmd->redir_cmd->int_fd);
		free(cmd->redir_cmd->file);
		free(cmd->redir_cmd);
		cmd->redir_cmd = tmp_redir;
	}
}

void	free_cmd(t_cmd *cmd)
{
	t_cmd	*tmp_cmd;
	int		i;

	while (cmd)
	{
		tmp_cmd = cmd->next;
		if (cmd->cmd_args)
		{
			i = 0;
			while (cmd->cmd_args[i])
			{
				free(cmd->cmd_args[i]);
				i++;
			}
			free(cmd->cmd_args);
		}
		free_cmd_redirs(cmd);
		free(cmd);
		cmd = tmp_cmd;
	}
}
