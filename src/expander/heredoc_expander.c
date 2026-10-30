/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 16:55:25 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 21:13:16 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Expansion heredoc: uniquement $VAR et $?, sans gestion de quotes
char	*expand_heredoc_line(char *line, t_mini *mini)
{
	char	*res;
	int		i;

	if (!line)
		return (NULL);
	res = ft_strdup("");
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			if (line[i] == '?')
				res = expand_exit_status(res, mini, &i);
			else if (ft_isalnum(line[i]) || line[i] == '_')
				res = expand_env_key(line, &i, res, mini);
			else
				res = append_char(res, '$');
		}
		else
			res = append_char(res, line[i++]);
	}
	return (res);
}

int	is_unquoted_var(char *original)
{
	if (!original || original[0] != '$')
		return (0);
	if (ft_strchr(original, '"') || ft_strchr(original, '\''))
		return (0);
	return (1);
}
