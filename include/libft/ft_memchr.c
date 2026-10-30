/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 16:30:39 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:45 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*tmp_s;
	unsigned char	cast_c;

	i = 0;
	tmp_s = (unsigned char *)s;
	cast_c = (unsigned char)c;
	while (i < n)
	{
		if (tmp_s[i] == cast_c)
			return ((void *)&tmp_s[i]);
		i++;
	}
	return (NULL);
}
