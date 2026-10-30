/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 17:03:01 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:57:22 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *str, const char *to_find, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (to_find[0] == 0)
		return ((char *)str);
	while (str[i] && i < len)
	{
		j = 0;
		while (str[i + j] == to_find[j] && to_find[j] && i + j < len)
		{
			j++;
			if (to_find[j] == 0)
				return ((char *)&str[i]);
		}
		i++;
	}
	return (NULL);
}
