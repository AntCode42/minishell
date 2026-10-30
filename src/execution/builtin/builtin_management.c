/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_management.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 12:37:00 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 15:16:59 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int builtin_sort(t_mini *mini)
// {
// 	int		i;
// 	char	**cmd;
// 	char	*inter[] = {"echo", "cd", "pwd", "export", "unset", "env", "exit",
// 						NULL};

// 	cmd = mini->cmd->cmd_args;
// 	i = 0;
// 	while (inter[i] != NULL)
// 	{
// 		if (ft_strncmp(cmd[0], inter[i], ft_strlen(cmd[0]) + 1) == 0)
// 			if (i = 0)
// 				ft_echo(mini);
// 			else if (i = 1)
// 				ft_cd(mini);
// 			else if (i = 2)
// 				ft_pwd(mini);
// 			else if (i = 3)
// 				ft_export(mini);
// 			else if (i = 4)
// 				ft_unset(mini);
// 			else if (i = 5)
// 				ft_env(mini);
// 			else if (i = 6)
// 				ft_exti(mini);
// 		i++;
// 	}
// 	return (0);
// }
