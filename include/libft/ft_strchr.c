/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 14:31:28 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:10:26 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	char	cast_c;

	cast_c = (char)c;
	i = 0;
	while (s[i])
	{
		if (s[i] == cast_c)
			return ((char *)&s[i]);
		i++;
	}
	if (s[i] == cast_c)
		return ((char *)&s[i]);
	return (NULL);
}
