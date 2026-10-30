/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_build.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:05 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:52 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
remplit le tableau des commandes et rempli le type de redir
*/
static int	fill_cmd_args(t_cmd *cmd, t_token **tok)
{
	int	i;

	i = 0;
	while (*tok && (*tok)->type != TOKEN_PIPE) //gros gros doute sur la formulation du (*) - a tester fort fort
	{
		if ((*tok)->type == TOKEN_WORD)
		{
			cmd->cmd_args[i] = ft_strdup((*tok)->value);
			if (!cmd->cmd_args[i])
				return (0);
			i++;
		}
		else if (is_redir_type((*tok)->type))
		{
			add_redir(cmd, (*tok)->type, (*tok)->next->value);
			*tok = (*tok)->next;
		}
		*tok = (*tok)->next;
	}
	cmd->cmd_args[i] = NULL;
	return (1);
}

/*
cree et rempli une commande (ouais le !fill... c'est moche mais c'est court, cmb)
*/
static t_cmd	*build_cmd(t_token **tok)
{
	t_cmd	*cmd;
	int		wc;

	cmd = init_cmd();
	if (!cmd)
		return (NULL);
	wc = count_cmd_words(*tok);
	cmd->cmd_args = malloc(sizeof(char *) * (wc + 1));
	if (!cmd->cmd_args || !fill_cmd_args(cmd, tok))
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

/*
ajoute une commande a la fin
*/
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

/*
fonction principale de convertion des tokens en liste 
*/
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
