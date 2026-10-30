/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 13:41:42 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 14:51:48 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	skip_spaces(char *input, int *i)
{
	while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
		(*i)++;
}
/*
juste pour les quotes, était intégré dans le read_word mais trop long
*/
static int	skip_quotes(char *input, int *i)
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

/*
lit un WORD puis s'arrete sur espace, tab, |, <, > ou \0.
le \ pour gérer le simple quote
pour le moment la quote est intégrée au mot, penser a retirer avec expender
Retourne le token WORD ou NULL
*/
static t_token	*read_word(char *input, int *i)
{
	int		start;
	char	*value;

	start = *i;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '|' && input[*i] != '<' && input[*i] != '>')
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			if (!skip_quotes(input, i))
				return (NULL);
		}
		else
			(*i)++;
	}
	t_token	*token;

	value = ft_substr(input, start, *i - start);
	if (!value)
		return (NULL);
	token = create_token(value, TOKEN_WORD);
	return (token);
}

/*
lit un signe |, <, >, >>, <<
Retourne le token correspondant ou NULL
c'est condense sinon ca fait trop de ligne avec les i++
*/
static t_token	*read_operator(char *input, int *i)
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

/*
début du lexer pour en faire liste de token
retourne le TOKEN ou NULL
*/
t_token	*lexer(char *input)
{
	t_token	*tokens;
	t_token	*new;
	int		i;

	tokens = NULL;
	i = 0;
	if (!input)
		return (NULL);
	while (input[i])
	{
		skip_spaces(input, &i);
		if (!input[i])
			break ;
		if (input[i] == '|' || input[i] == '<' || input[i] == '>')
			new = read_operator(input, &i);
		else
			new = read_word(input, &i);
		if (!new)
		{
			free_tokens(tokens);
			return (NULL);
		}
		add_token(&tokens, new);
	}
	return (tokens);
}
