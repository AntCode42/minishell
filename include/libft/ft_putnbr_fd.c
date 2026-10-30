/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:47:10 by gchaillo          #+#    #+#             */
/*   Updated: 2025/10/10 15:51:57 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	nb;

	nb = (long)n;
	if (nb < 0)
	{
		ft_putchar_fd ('-', fd);
		nb *= -1;
	}
	if (nb >= 10)
		ft_putnbr_fd((nb / 10), fd);
	ft_putchar_fd(((nb % 10) + '0'), fd);
}
