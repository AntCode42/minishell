/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_management.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 18:50:11 by agard             #+#    #+#             */
/*   Updated: 2026/05/06 22:08:05 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	update_pwd_vars(t_mini *mini, char *old_path)
{
	t_env	*oldpwd;
	t_env	*newpwd;
	char	*tmp;

	oldpwd = find_env_node(mini->env, "OLDPWD");
	if (oldpwd)
	{
		free(oldpwd->value);
		oldpwd->value = ft_strdup(old_path);
	}
	else
		append_env_node(&mini->env, new_env_node("OLDPWD", old_path));
	newpwd = find_env_node(mini->env, "PWD");
	tmp = getcwd(NULL, 0);
	if (newpwd)
	{
		free(newpwd->value);
		newpwd->value = ft_strdup(tmp);
	}
	else
		append_env_node(&mini->env, new_env_node("PWD", tmp));
	free(tmp);
}

int	cd_regular(t_mini *mini)
{
	char	*old_path;

	if (mini->cmd->cmd_args[1] == NULL)
	{
		cd_home(mini);
		return (0);
	}
	old_path = getcwd(NULL, 0);
	if (chdir(mini->cmd->cmd_args[1]) < 0)
	{
		ft_putstr_fd("cd: ", 2);
		ft_putstr_fd(mini->cmd->cmd_args[1], 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		mini->exit_status = 1;
		free(old_path);
		return (1);
	}
	update_pwd_vars(mini, old_path);
	free(old_path);
	return (0);
}

int	cd_home(t_mini *mini)
{
	char	*old_path;
	t_env	*home;

	old_path = getcwd(NULL, 0);
	home = find_env_node(mini->env, "HOME");
	if (!home || chdir(home->value) < 0)
	{
		ft_putstr_fd("minishell: cd: HOME not set or ", 2);
		ft_putendl_fd(strerror(errno), 2);
		mini->exit_status = 1;
		free(old_path);
		return (1);
	}
	update_pwd_vars(mini, old_path);
	free(old_path);
	return (0);
}

int	cd_minus(t_mini *mini)
{
	char	*old_path;
	t_env	*oldpwd;

	old_path = getcwd(NULL, 0);
	oldpwd = find_env_node(mini->env, "OLDPWD");
	if (!oldpwd || chdir(oldpwd->value) < 0)
	{
		ft_putstr_fd("minishell: cd: OLDPWD not set or ", 2);
		ft_putendl_fd(strerror(errno), 2);
		mini->exit_status = 1;
		free(old_path);
		return (1);
	}
	update_pwd_vars(mini, old_path);
	free(old_path);
	return (0);
}
