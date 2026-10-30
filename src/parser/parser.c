/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:15 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 21:49:06 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Compte le nombre de mots (arguments) d'une commande jusqu'au pipe
int	count_cmd_words(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens && tokens->type != TOKEN_PIPE)
	{
		if (tokens->type == TOKEN_WORD)
		{
			if (!(tokens->value[0] == '\0' && tokens->quoted == 0))
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

// Ajoute une redirection à la fin de la liste des redirections de la commande
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

// Crée et ajoute une nouvelle redirection
int	add_redir(t_cmd *cmd, t_token_type type, char *file, int quoted)
{
	t_redir	*new_redir;

	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return (0);
	new_redir->file = ft_strdup(file);
	if (!new_redir->file)
	{
		free(new_redir);
		return (0);
	}
	new_redir->int_fd = -1;
	new_redir->type = type;
	new_redir->quoted = quoted;
	new_redir->next = NULL;
	redir_add_back(cmd, new_redir);
	return (1);
}

// Alloue et initialise une nouvelle structure de commande (t_cmd)
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
