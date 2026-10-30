/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 11:59:00 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 12:56:54 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_init_empty(void)
{
	char	*s;

	s = malloc(1);
	if (!s)
		return (NULL);
	s[0] = '\0';
	return (s);
}

char	*ft_strjoin_gnl(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	if (!s1)
		s1 = ft_init_empty();
	if (!s1 || !s2)
		return (NULL);
	str = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!str)
		return (free(s1), NULL);
	i = -1;
	while (s1[++i])
		str[i] = s1[i];
	j = -1;
	while (s2[++j])
		str[i++] = s2[j];
	str[i] = '\0';
	free(s1);
	return (str);
}

char	*ft_save_rest(char *buffer, int start)
{
	char	*new;
	int		j;

	new = malloc(sizeof(char) * (ft_strlen(buffer) - start + 1));
	if (!new)
		return (NULL);
	j = 0;
	while (buffer[start])
	{
		new[j] = buffer[start];
		j++;
		start++;
	}
	new[j] = '\0';
	return (new);
}
