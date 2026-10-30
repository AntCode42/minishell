/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:15 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:52:01 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
compte le nombre de mots (arguments) d'une commande jusqu'au pipe (décidément ce mot...)
*/
int	count_cmd_words(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens && tokens->type != TOKEN_PIPE)
	{
		if (tokens->type == TOKEN_WORD)
		{
			count++;
			tokens = tokens->next;
		}
		else if (is_redir_type(tokens->type))
			tokens = tokens->next->next;
		else
			tokens = tokens->next;
	}
	return (count);
}

/*
ajout de la redirection a la fin un peu comme ft_lstadd_back
*/
static void	redir_add_back(t_cmd *cmd, t_redir *new_redir)
{
	t_redir	*tmp;

	if (!cmd->redir_cmd)
		cmd->redir_cmd = new_redir;
	else
	{
		tmp = cmd->redir_cmd;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_redir;
	}
}

/*
crée et ajoute une nouvelle redirection (a la fin avec le add_back)
*/
void	add_redir(t_cmd *cmd, t_token_type type, char *file)
{
	t_redir	*new_redir;

	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return ;
	new_redir->file = ft_strdup(file);
	new_redir->int_fd = -1;
	new_redir->type = type;
	new_redir->next = NULL;
	redir_add_back(cmd, new_redir);
}

/*
création de la structure de t_cmd
*/
t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->cmd_args = NULL;
	cmd->redir_cmd = NULL;
	cmd->next = NULL;
	cmd->prev = NULL;
	return (cmd);
}
