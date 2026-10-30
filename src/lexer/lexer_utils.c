/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 13:42:58 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 20:32:02 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Ignore les caracteres a l'interieur des guillemets simples ou doubles
int	skip_quotes(char *input, int *i)
{
	char	quote;

	quote = input[*i];
	(*i)++;
	while (input[*i] && input[*i] != quote)
		(*i)++;
	if (input[*i] == quote)
	{
		(*i)++;
		return (1);
	}
	ft_putendl_fd("minishell: syntax error: unclosed quotes", 2);
	return (0);
}

// Avance jusqu'a la fin d'un mot en tenant compte des guillemets
int	read_word_loop(char *input, int *i, int *quoted)
{
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			*quoted = 1;
			if (!skip_quotes(input, i))
				return (0);
		}
		else
			(*i)++;
	}
	return (1);
}

// Lit un mot (WORD) jusqu'au prochain espace, operateur ou fin de chaine
t_token	*read_word(char *input, int *i)
{
	int		start;
	int		quoted;
	char	*value;
	t_token	*token;

	start = *i;
	quoted = 0;
	if (!read_word_loop(input, i, &quoted))
		return (NULL);
	value = ft_substr(input, start, *i - start);
	if (!value)
		return (NULL);
	token = create_token(value, TOKEN_WORD);
	free(value);
	if (!token)
		return (NULL);
	token->quoted = quoted;
	return (token);
}

// Analyse un operateur de redirection ou un pipe
t_token	*read_operator(char *input, int *i)
{
	if (input[*i] == '|')
		return ((*i)++, create_token("|", TOKEN_PIPE));
	if (input[*i] == '>' && input[*i + 1] == '>')
		return (*i += 2, create_token(">>", TOKEN_APPEND));
	if (input[*i] == '>')
		return ((*i)++, create_token(">", TOKEN_REDIR_OUT));
	if (input[*i] == '<' && input[*i + 1] == '<')
		return (*i += 2, create_token("<<", TOKEN_HEREDOC));
	if (input[*i] == '<')
		return ((*i)++, create_token("<", TOKEN_REDIR_IN));
	return (NULL);
}

t_token	*read_next_token(char *input, int *i)
{
	if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
		return (read_operator(input, i));
	return (read_word(input, i));
}
