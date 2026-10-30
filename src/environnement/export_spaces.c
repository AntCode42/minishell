/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_spaces.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 18:10:24 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/06 09:44:29 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	skip_spaces(const char *value, size_t i)
{
	while (value[i] && ft_isspace(value[i]))
		i++;
	return (i);
}

static size_t	normalized_len(const char *value)
{
	size_t	i;
	size_t	len;
	int		in_space;

	i = skip_spaces(value, 0);
	len = 0;
	in_space = 0;
	while (value[i])
	{
		if (ft_isspace(value[i]))
		{
			in_space = 1;
			i = skip_spaces(value, i);
		}
		else
		{
			if (in_space && len > 0)
				len++;
			in_space = 0;
			len++;
			i++;
		}
	}
	return (len);
}

static void	fill_normalized(char *res, const char *value)
{
	size_t	i;
	size_t	len;
	int		in_space;

	i = skip_spaces(value, 0);
	len = 0;
	in_space = 0;
	while (value[i])
	{
		if (ft_isspace(value[i]))
		{
			in_space = 1;
			i = skip_spaces(value, i);
		}
		else
		{
			if (in_space && len > 0)
				res[len++] = ' ';
			in_space = 0;
			res[len++] = value[i++];
		}
	}
	res[len] = '\0';
}

char	*normalize_spaces(const char *value)
{
	char	*res;
	size_t	len;

	if (!value)
		return (NULL);
	len = normalized_len(value);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	fill_normalized(res, value);
	return (res);
}
