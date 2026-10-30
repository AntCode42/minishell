/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_fork.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 11:50:21 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 14:51:32 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char **env_to_array(t_env *env) {
  t_env *node;
  char *tmp;
  char **array;
  int nbr_node;
  int i;

  node = env;
  nbr_node = count_node(env);
  array = (char **)malloc((sizeof(char *) * nbr_node) + sizeof(nbr_node));
  i = 0;
  while (i < nbr_node) {
    tmp = ft_strjoin(node->key, "=");
    array[i] = ft_strjoin(tmp, node->value);
    free(tmp);
    node = node->next;
    i++;
  }
  array[i] = NULL;
  return (array);
}

int count_node(t_env *env) {
  t_env *runner;
  t_env *node;
  int i;

  if (!env)
    return (0);
  node = env;
  if (!node->next)
    return (1);
  runner = node->next;
  i = 1;
  while (runner) {
    node = runner;
    runner = node->next;
    i++;
  }
  return (i);
}

char *path(t_env *env, char *cmd) {
  t_env *path;
  char **path_array;
  char *tmp;
  int i;

  path = find_path(env);
  path_array = ft_split(path->value, ':');
  tmp = ft_strjoin("/", cmd);
  path_array = join_cmd(path_array, tmp);
  i = 0;
  while (path_array[i]) {
    if (access(path_array[i], X_OK) == 0)
      return (path_array[i]);
    else
      i++;
  }
  return (path_array[i]); // to check return value
}

t_env *find_path(t_env *env) {
  t_env *tmp;

  tmp = env;
  while (tmp) {
    if (ft_strncmp(tmp->key, "PATH", ft_strlen("PATH")) == 0)
      return (tmp);
    tmp = tmp->next;
  }
  return (tmp); // to_check return value
}

char **join_cmd(char **path_array, char *cmd) {
  int i;
  char *tmp;

  i = 0;
  while (path_array[i]) {
    tmp = path_array[i];
    path_array[i] = ft_strjoin(tmp, cmd);
    free(tmp);
    i++;
  }
  return (path_array);
}