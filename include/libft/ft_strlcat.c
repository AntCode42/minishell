/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 10:15:23 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:21:49 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	len_src;
	size_t	len_dst;
	size_t	len_cop;

	len_dst = ft_strlen(dst);
	len_src = ft_strlen(src);
	if (size <= len_dst)
		return (size + len_src);
	len_cop = size - len_dst - 1;
	if (len_cop > len_src)
		len_cop = len_src;
	ft_memmove(dst + len_dst, src, len_cop);
	dst[len_dst + len_cop] = '\0';
	return (len_dst + len_src);
}
