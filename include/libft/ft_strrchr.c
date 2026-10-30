/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:04:48 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:57:25 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	len;
	char	cast_c;

	cast_c = (char)c;
	len = ft_strlen(s);
	if (cast_c == '\0')
		return ((char *)&s[len]);
	while (len > 0)
	{
		len--;
		if (s[len] == cast_c)
			return ((char *)&s[len]);
	}
	return (NULL);
}
