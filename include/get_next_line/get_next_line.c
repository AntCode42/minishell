/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 12:57:06 by gchaillo          #+#    #+#             */
/*   Updated: 2026/01/29 11:48:00 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	ft_line_len(char *buffer)
{
	int	i;

	i = 0;
	if (!buffer)
		return (0);
	while (buffer[i] && buffer[i] != '\n')
		i++;
	return (i);
}

char	*ft_extract_line(char *buffer)
{
	char	*line;
	int		i;
	int		len;

	if (!buffer || !buffer[0])
		return (NULL);
	len = ft_line_len(buffer);
	if (buffer[len] == '\n')
		line = malloc(sizeof(char) * (len + 2));
	else
		line = malloc(sizeof(char) * (len + 1));
	if (!line)
		return (NULL);
	i = 0;
	while (i < len)
	{
		line[i] = buffer[i];
		i++;
	}
	if (buffer[len] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

char	*ft_new_buffer(char *buffer)
{
	int		i;
	char	*new;

	i = 0;
	if (!buffer)
		return (NULL);
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (!buffer[i])
	{
		free(buffer);
		return (NULL);
	}
	new = ft_save_rest(buffer, i + 1);
	if (!new)
	{
		free(buffer);
		return (NULL);
	}
	free(buffer);
	return (new);
}

char	*ft_read_and_stock(int fd, char *buffer)
{
	char	*temp;
	int		bytes_read;

	temp = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!temp)
	{
		free(buffer);
		return (NULL);
	}
	bytes_read = 1;
	while ((!buffer || !ft_strchr(buffer, '\n')) && bytes_read > 0)
	{
		bytes_read = read(fd, temp, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(temp);
			free(buffer);
			return (NULL);
		}
		temp[bytes_read] = '\0';
		buffer = ft_strjoin_gnl(buffer, temp);
	}
	free(temp);
	return (buffer);
}

char	*get_next_line(int fd)
{
	static char	*buffer[FD_MAX];
	char		*line;

	if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
		return (NULL);
	buffer[fd] = ft_read_and_stock(fd, buffer[fd]);
	if (!buffer[fd])
		return (NULL);
	line = ft_extract_line(buffer[fd]);
	if (!line)
	{
		free(buffer[fd]);
		buffer[fd] = NULL;
		return (NULL);
	}
	buffer[fd] = ft_new_buffer(buffer[fd]);
	return (line);
}
