/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 12:35:04 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:53 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*tmp_s;
	unsigned char	*tmp_d;
	size_t			i;

	if ((dest == NULL && src == NULL && n > 0))
		return (NULL);
	if (n == 0)
		return (dest);
	tmp_d = (unsigned char *)dest;
	tmp_s = (unsigned char *)src;
	i = 0;
	while (i < n)
	{
		tmp_d[i] = tmp_s[i];
		i++;
	}
	return (dest);
}
