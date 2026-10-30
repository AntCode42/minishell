/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 14:31:02 by gchaillo          #+#    #+#             */
/*   Updated: 2025/12/18 10:21:48 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*rslt;
	size_t	i;
	size_t	join;

	rslt = ft_calloc((ft_strlen(s1) + ft_strlen(s2) + 1), sizeof(char));
	if (!rslt)
		return (NULL);
	join = ft_strlen(s1);
	rslt = ft_memcpy(rslt, s1, join);
	i = 0;
	while (s2[i])
	{
		rslt[join + i] = s2[i];
		i++;
	}
	return (rslt);
}
