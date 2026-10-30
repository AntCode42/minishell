/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 13:13:35 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 12:57:27 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*temp_s;

	i = 0;
	temp_s = (unsigned char *)s;
	while (i < n)
	{
		temp_s[i] = (unsigned char)c;
		i++;
	}
	return (s);
}
