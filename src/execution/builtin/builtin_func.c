/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_func.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 17:46:31 by agard             #+#    #+#             */
/*   Updated: 2026/05/10 14:10:00 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*read_echo(char **cmd, int *lf)
{
	int		i;
	char	*word;

	i = 0;
	while (cmd[++i] && is_n_option(cmd[i]))
		*lf = 1;
	if (cmd[i] == NULL)
		return (ft_strdup(""));
	word = ft_strdup("");
	while (cmd[i])
	{
		word = ft_strcat(word, cmd[i++]);
		if (cmd[i])
			word = ft_strcat(word, " ");
	}
	return (word);
}

int	ft_echo(t_mini *mini)
{
	char	**cmd;
	char	*word;
	int		lf;
	int		saved_fd;

	lf = 0;
	cmd = mini->cmd->cmd_args;
	word = read_echo(cmd, &lf);
	saved_fd = dup(1);
	ft_redir(mini->cmd->redir_cmd);
	if (lf == 1)
		printf("%s", word);
	else
		printf("%s\n", word);
	fflush(stdout);
	dup2(saved_fd, 1);
	close(saved_fd);
	free(word);
	return (0);
}

int	ft_cd(t_mini *mini)
{
	int		flag;
	int		saved_fd;
	int		ret;

	if (mini->cmd->cmd_args[1] && mini->cmd->cmd_args[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		mini->exit_status = 2;
		return (2);
	}
	saved_fd = dup(1);
	ft_redir(mini->cmd->redir_cmd);
	flag = check_inpt(mini->cmd);
	ret = 0;
	if (flag == 1)
		ret = cd_regular(mini);
	else if (flag == 2)
		ret = cd_minus(mini);
	else if (flag == 3)
		ret = cd_home(mini);
	dup2(saved_fd, 1);
	close(saved_fd);
	return (ret);
}

int	ft_pwd(t_mini *mini)
{
	char	*pwd;
	int		saved_fd;

	saved_fd = dup(1);
	ft_redir(mini->cmd->redir_cmd);
	pwd = getcwd(NULL, 0);
	if (pwd == NULL)
	{
		mini->exit_status = 1;
		dup2(saved_fd, 1);
		close(saved_fd);
		return (1);
	}
	printf("%s\n", pwd);
	fflush(stdout);
	free (pwd);
	dup2(saved_fd, 1);
	mini->exit_status = 0;
	close(saved_fd);
	return (0);
}

int	ft_exit(t_mini *mini)
{
	t_cmd	*cmd;

	cmd = mini->cmd;
	ft_putendl_fd("exit", 2);
	if (cmd->cmd_args[1] == NULL)
		exit(mini->exit_status);
	if (ck_alpha(cmd->cmd_args[1]))
	{
		ft_putendl_fd("minishell: exit: numeric argument required", 2);
		mini->exit_status = 2;
		exit(2);
	}
	if (cmd->cmd_args[2] != NULL)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		mini->exit_status = 1;
		return (1);
	}
	exit(ft_atoi(cmd->cmd_args[1]));
}
