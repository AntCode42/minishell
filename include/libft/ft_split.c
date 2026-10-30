/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:58:42 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:04 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_word_count(const char *s, char c)
{
	int	count;
	int	in_word;
	int	i;

	count = 0;
	in_word = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && in_word == 0)
		{
			in_word = 1;
			count++;
		}
		else if (s[i] == c)
			in_word = 0;
		i++;
	}
	return (count);
}

static char	**ft_free_all(char **split, int j)
{
	while (j > 0)
		free(split[--j]);
	free(split);
	return (NULL);
}

static char	**ft_fill_strings(const char *s, char **split, char c)
{
	int	i;
	int	j;
	int	start;
	int	len;

	i = 0;
	j = 0;
	while (s[i])
	{
		while (s[i] == c)
			i++;
		start = i;
		len = 0;
		while (s[i] && s[i] != c && ++len)
			i++;
		if (len > 0)
		{
			split[j] = ft_substr(s, start, len);
			if (!split[j])
				return (ft_free_all(split, j));
			j++;
		}
	}
	split[j] = NULL;
	return (split);
}

char	**ft_split(const char *s, char c)
{
	char	**split;
	int		count;

	if (!s)
		return (NULL);
	count = ft_word_count(s, c);
	split = ft_calloc(count + 1, sizeof(char *));
	if (!split)
		return (NULL);
	return (ft_fill_strings(s, split, c));
}
