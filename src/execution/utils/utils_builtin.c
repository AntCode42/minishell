/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:46:07 by agard             #+#    #+#             */
/*   Updated: 2026/05/05 16:49:08 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_inpt(t_cmd *cmd)
{
	if ((cmd->cmd_args[1]) == NULL)
		return (1);
	else if (ft_strcmp(cmd->cmd_args[1], ".") == 0)
		return (1);
	else if (ft_strcmp(cmd->cmd_args[1], "..") == 0)
		return (1);
	else if (ft_strcmp(cmd->cmd_args[1], "...") == 0)
		return (1);
	else if (ft_strcmp(cmd->cmd_args[1], "-") == 0)
		return (2);
	else if (ft_strcmp(cmd->cmd_args[1], "~") == 0)
		return (3);
	else
		return (1);
	return (0);
}

int	ck_alpha(char *input)
{
	int	i;

	i = 0;
	while (input[i] != '\0')
	{
		if (ft_isalpha(input[i]) == 1)
			return (1);
		else
			i++;
	}
	return (0);
}

int	is_n_option(char *arg)
{
	int	i;

	if (arg[0] != '-')
		return (0);
	i = 1;
	if (arg[i] == '\0')
		return (0);
	while (arg[i] == 'n')
		i++;
	return (arg[i] == '\0');
}
