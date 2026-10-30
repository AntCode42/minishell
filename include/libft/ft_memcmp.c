/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/06 10:38:30 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:48 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*r1;
	const unsigned char	*r2;
	size_t				i;

	r1 = s1;
	r2 = s2;
	i = 0;
	while (i < n)
	{
		if (r1[i] != r2[i])
			return (r1[i] - r2[i]);
		i++;
	}
	return (0);
}
