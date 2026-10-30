/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 17:28:33 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 22:11:46 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Trie un tableau d'environnement par ordre alphabétique (clé)
static void	sort_env_array(t_env **arr, int count)
{
	t_env	*swap;
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strncmp(arr[i]->key, arr[j]->key,
					ft_strlen(arr[i]->key) + 1) > 0)
			{
				swap = arr[i];
				arr[i] = arr[j];
				arr[j] = swap;
			}
			j++;
		}
		i++;
	}
}

// Convertit la liste chaînée d'environnement en tableau trié
t_env	**sorted_env_array(t_env *env, int count)
{
	t_env	**arr;
	t_env	*tmp;
	int		i;

	arr = malloc(sizeof(t_env *) * (count + 1));
	if (!arr)
		return (NULL);
	tmp = env;
	i = 0;
	while (tmp)
	{
		arr[i++] = tmp;
		tmp = tmp->next;
	}
	arr[count] = NULL;
	sort_env_array(arr, count);
	return (arr);
}

// Affiche une variable d'environnement au format pour export
static void	print_env_var(t_env *node)
{
	ft_putstr_fd("export ", 1);
	ft_putstr_fd(node->key, 1);
	if (node->value)
	{
		ft_putstr_fd("=\"", 1);
		ft_putstr_fd(node->value, 1);
		ft_putstr_fd("\"", 1);
	}
	ft_putstr_fd("\n", 1);
}

// Affiche les variables si export est sans argument
void	print_export(t_env *env)
{
	t_env	**arr;
	t_env	*tmp;
	int		count;
	int		i;

	count = 0;
	tmp = env;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	arr = sorted_env_array(env, count);
	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		print_env_var(arr[i]);
		i++;
	}
	free(arr);
}
