/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:27 by gchaillo          #+#    #+#             */
/*   Updated: 2026/04/23 10:40:20 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Affiche un message d'erreur de syntaxe de parsing
static void	print_syntax_error(char *token_val)
{
	ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
	if (token_val)
		ft_putstr_fd(token_val, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putstr_fd("'\n", 2);
}

// Vérifie si le type de token est une redirection
int	is_redir_type(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

// Vérifie qu'une redirection est bien suivie d'un mot (la cible)
static int	handle_redir_error(t_token *curr)
{
	if (!curr->next || curr->next->type != TOKEN_WORD)
	{
		if (curr->next)
			print_syntax_error(curr->next->value);
		else
			print_syntax_error(NULL);
		return (0);
	}
	return (1);
}

// Vérifie la syntaxe (pipes et redirections)
static int	check_token_syntax(t_token *curr, t_token *prev)
{
	if (curr->type == TOKEN_WORD && curr->value
		&& !ft_strcmp(curr->value, "("))
	{
		print_syntax_error("(");
		return (0);
	}
	if (curr->type == TOKEN_PIPE)
	{
		if (!prev || prev->type == TOKEN_PIPE)
		{
			print_syntax_error("|");
			return (0);
		}
	}
	else if (is_redir_type(curr->type))
	{
		if (!handle_redir_error(curr))
			return (0);
	}
	return (1);
}

// Fonction principale de vérification de la syntaxe des tokens
int	check_syntax(t_token *tokens)
{
	t_token	*curr;
	t_token	*prev;

	curr = tokens;
	prev = NULL;
	while (curr)
	{
		if (!check_token_syntax(curr, prev))
			return (0);
		prev = curr;
		curr = curr->next;
	}
	if (prev && prev->type == TOKEN_PIPE)
	{
		print_syntax_error("|");
		return (0);
	}
	return (1);
}
