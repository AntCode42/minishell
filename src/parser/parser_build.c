/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_build.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:05 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:02:45 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_redir(t_cmd *cmd, t_token **tok)
{
	int	quoted;

	quoted = 0;
	if ((*tok)->type == TOKEN_HEREDOC)
		quoted = (*tok)->next->quoted;
	if (!add_redir(cmd, (*tok)->type, (*tok)->next->value, quoted))
		return (0);
	*tok = (*tok)->next;
	return (1);
}

// Remplit le tableau d'arguments de la commande et gère les redirections
static int	fill_cmd_args(t_cmd *cmd, t_token **tok)
{
	int	i;

	i = 0;
	while (*tok && (*tok)->type != TOKEN_PIPE)
	{
		if ((*tok)->type == TOKEN_WORD)
		{
			if (!((*tok)->value[0] == '\0' && (*tok)->quoted == 0))
			{
				cmd->cmd_args[i] = ft_strdup((*tok)->value);
				if (!cmd->cmd_args[i])
					return (0);
				i++;
			}
		}
		else if (is_redir_type((*tok)->type))
		{
			if (!handle_redir(cmd, tok))
				return (0);
		}
		*tok = (*tok)->next;
	}
	cmd->cmd_args[i] = NULL;
	return (1);
}

// Crée et initialise une structure de commande à partir des tokens
static t_cmd	*build_cmd(t_token **tok)
{
	t_cmd	*cmd;
	int		wc;

	cmd = init_cmd();
	if (!cmd)
		return (NULL);
	wc = count_cmd_words(*tok);
	cmd->cmd_args = ft_calloc(wc + 1, sizeof(char *));
	if (!cmd->cmd_args || !fill_cmd_args(cmd, tok))
	{
		free_cmd(cmd);
		return (NULL);
	}
	return (cmd);
}

// Ajoute une commande à la fin de la liste chaînée
static void	cmd_add_back(t_cmd **head, t_cmd *cmd)
{
	t_cmd	*tmp;

	if (!*head)
		*head = cmd;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = cmd;
		cmd->prev = tmp;
	}
}

// Fonction principale : convertit les tokens en commandes
t_cmd	*parse_tokens(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*cmd;

	head = NULL;
	while (tokens)
	{
		cmd = build_cmd(&tokens);
		if (!cmd)
		{
			free_cmd(head);
			return (NULL);
		}
		cmd_add_back(&head, cmd);
		if (tokens && tokens->type == TOKEN_PIPE)
			tokens = tokens->next;
	}
	return (head);
}
