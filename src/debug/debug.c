/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 10:22:42 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:17 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
fichier pour debug et afficher si le lexer et parser marche bien
sera supprime apres
*/
char	*get_token_type_str(t_token_type type)
{
	if (type == TOKEN_WORD)
		return ("WORD");
	if (type == TOKEN_PIPE)
		return ("PIPE");
	if (type == TOKEN_REDIR_IN)
		return ("REDIR_IN");
	if (type == TOKEN_REDIR_OUT)
		return ("REDIR_OUT");
	if (type == TOKEN_APPEND)
		return ("APPEND");
	if (type == TOKEN_HEREDOC)
		return ("HEREDOC");
	return ("UNKNOWN");
}

void	print_tokens(t_token *list)
{
	int	i;

	i = 0;
	printf("--- TOKENS ---\n");
	while (list)
	{
		printf("Token[%d]: type=%-10s | value=%s\n",
			i, get_token_type_str(list->type), list->value);
		list = list->next;
		i++;
	}
	printf("--------------\n\n");
}

void	print_cmd(t_cmd *cmd)
{
	int		i;
	int		cmd_id;
	t_redir	*redir;

	cmd_id = 0;
	printf("--- COMMANDS ---\n");
	while (cmd)
	{
		printf("CMD[%d]:\n", cmd_id);
		i = 0;
		if (cmd->cmd_args)
		{
			while (cmd->cmd_args[i])
			{
				printf("  Arg[%d]: %s\n", i, cmd->cmd_args[i]);
				i++;
			}
		}
		redir = cmd->redir_cmd;
		while (redir)
		{
			printf("  Redir: type=%-10s | file=%s\n",
				get_token_type_str(redir->type), redir->file);
			redir = redir->next;
		}
		cmd = cmd->next;
		cmd_id++;
	}
	printf("----------------------\n");
}
