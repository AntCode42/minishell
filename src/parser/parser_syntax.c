/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 16:26:27 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:57 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
affiche un message d'erreur de syntaxe de parsing (même que dans le bash mais mon bash est francais c'est relou...)
bref à tester car le token est entouré par << >> chez moi
*/
static void	print_syntax_error(char *token_val)
{
	ft_putstr_fd("minishell: syntax error near unexpected token '", 2);
	if (token_val)
		ft_putstr_fd(token_val, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putstr_fd("'\n", 2);
}

/*
vérifie si le token est une redirection (in/out/append/heredoc)
*/
int	is_redir_type(t_token_type type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_APPEND || type == TOKEN_HEREDOC);
}

/* Verifie qu'une redirection est bien suivie d'un mot (la cible) */
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

/*
vérifie la validité du token en cours
j'ai appris que : echo "hello" | > out cat			et bien ca marche...
bref je ne suis pas sûr d'avoir tout compris avec ces pipes et 
*/
static int	check_token_syntax(t_token *curr, t_token *prev)
{
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

/*
fonction principale du check de la syntaxe
*/
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
