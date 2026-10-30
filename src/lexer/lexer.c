/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 13:41:42 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 21:31:00 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Fonction principale : transforme la chaine en liste de tokens
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
		while (input[i] && (input[i] == ' ' || input[i] == '\t'))
			i++;
		if (!input[i])
			break ;
		new = read_next_token(input, &i);
		if (!new)
		{
			free_tokens(tokens);
			return (NULL);
		}
		add_token(&tokens, new);
	}
	return (tokens);
}
