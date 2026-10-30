/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 14:25:21 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:55 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*tmp_d;
	unsigned char	*tmp_s;

	i = 0;
	if (dest == NULL && src == NULL)
		return (NULL);
	tmp_d = (unsigned char *)dest;
	tmp_s = (unsigned char *)src;
	if (tmp_d > tmp_s && tmp_d < (tmp_s + n))
		while (n-- > 0)
			tmp_d[n] = tmp_s[n];
	else
		while (i++ < n)
			tmp_d[i - 1] = tmp_s[i - 1];
	return (dest);
}
