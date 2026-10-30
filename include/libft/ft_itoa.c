/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 10:05:09 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:09:38 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_numlen(long n)
{
	size_t	len;

	len = 1;
	while (n >= 10)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	long	nb;
	size_t	len;
	size_t	sign;
	char	*str;

	nb = (long)n;
	sign = 0;
	if (nb < 0)
	{
		sign = 1;
		nb = -nb;
	}
	len = ft_numlen(nb) + sign;
	str = ft_calloc(len + 1, sizeof(char));
	if (!str)
		return (NULL);
	if (sign == 1)
		str[0] = '-';
	while (len > sign)
	{
		str[len - 1] = (nb % 10) + '0';
		nb /= 10;
		len--;
	}
	return (str);
}
