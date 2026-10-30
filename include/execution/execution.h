/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 09:44:27 by agard             #+#    #+#             */
/*   Updated: 2026/03/30 15:20:40 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ************************************************************************** */
// ./execution/sort_cmd.c

int ck_pipe(t_mini *mini);
// Determines whether there are pipes or not.
// @param mini : main struct containing all sub-structs
// @call	pipeline() | single()
// @return	exit_status. 1 if mini is NULL.

int ext_func_finder(char **list_cmd);
// Determines whether the command is a builtin or external.
// @param list_cmd : array of command + options (NULL-terminated)
// @call	ft_strncmp(), ft_strlen()
// @return	0 = external, 1 = builtin, 2 = error

int single(t_mini *mini);
// Routes a single command to fork or builtin handler.
// @param mini : main struct containing all sub-structs
// @call	ext_func_finder(), create_fork(), builtin_call()
// @return	exit_status. 2 if list_cmd error.

/* ************************************************************************** */
// ./execution/fork_management.c

int create_single_fork(t_mini *mini);
// Creates a child process to execute an external command.
// @param mini : main struct containing all sub-structs
// @call	fork(), execve(), perror(), waitpid()
// @call	WIFEXITED(), WEXITSTATUS()
// @return	exit_status.

int fork_all_commands(t_mini *mini, pid_t *last_pid);
// Forks all commands in the pipeline with proper pipe chaining.
// @param mini : main struct containing all sub-structs
// @param last_pid : output parameter to store the last forked PID
// @call	pipe(), go_through_pipe(), ck_pipe(), close()
// @return	total number of commands forked.

/* ************************************************************************** */
// ./execution/utils/utils_fork.c

char **env_to_array(t_env *env);
// transforme the chaine list t_env in a **array.
// @param env : struct containing the environement
// @ call	count_node(), ft_strjoin(), free()
// @return	a **array containing all the environement

int count_node(t_env *env);
// Count the number of node in a simple chaine liste.
// @param env : struct containing the environement
// @call	none
// @return	the number of node forom 0 et n.

char *path(t_env *env, char *cmd);
// return the correct path for the bin of the commande cmd in param
// @param env : struct containing the environement
// @param cmd : argument containing the name of the commande to find
// @call	find_path(), ft_split(), join_cmd(), access()
// @return	the correct path for the bin

t_env *find_path(t_env *env);
// return the node inside env that containe the "PATH" key and value
// @param env : struct containing the environement
// @call	ft_strncmp, ft_strlen
// @return	return the node inside env that containe the "PATH" key and
// value

char **join_cmd(char **path_array, char *cmd);
// take a array of all the path inside "PATH" and add the cmd at the end to
// complete the commande
// @param prath_array : **array containing all the path possible
// @param cmd : argument containing the name of the commande to add
// @call	ft_strjoin()
// @return	return all the path possible in a **array with de commande at
// the end

/* ************************************************************************** */
// ./execution/builtin/builtin.c

// int builtin_sort(t_mini *mini);
// detects which built-in function is being called and executes the
// corresponding command.
// @param mini : main struct containing all sub-structs.
// @call	ft_strncmp, ft_echo, ft_cd, ft_pwd
// @call	ft_export, ft_unset, ft_env, ft_exit
// @return	exit_status

int ft_echo(t_mini mini);
// sends the **list_cmd to isolate the input and prints it to standard output
// @param mini : main struct containing all sub-structs.
// @call	--- (for print in stdout)
// @return	exit_status

char read_echo(char **cmd, int *lf);
// isolates the input from the echo command and checks whether an option is
// present or not.
// @param	word : contains one complete **array with the command, option
// and argument.
// @param	lf : flag to check whether the -n option is present or not
// @return	yhe isolated echo register. The flag is passed by address.

/* ************************************************************************** */
// ./execution/pipe_management.c

int	pipeline(t_mini *mini);
// Routes pipeline commands to executor or builtin handler.
// @param mini : main struct containing all sub-structs
// @call	ext_func_finder(), execute_pipeline(), builtin_sort()
// @return	exit_status.

int execute_pipeline(t_mini *mini);
// Orchestrates the complete pipeline execution: fork all, then wait all.
// @param mini : main struct containing all sub-structs
// @call	fork_all_commands(), wait_all_children()
// @return	exit_status of the last command in pipeline.

int wait_all_children(int count, pid_t last_pid);
// Waits for all child processes, retrieving exit status from the last one.
// @param count : total number of forked processes
// @param last_pid : PID of the last command in pipeline
// @call	wait(), waitpid(), WIFEXITED(), WEXITSTATUS()
// @return	exit_status of last_pid. 1 if abnormal termination.

int	go_through_pipe(t_cmd *node, t_env *env, int *pipefd, int pipe_status);
// Forks a single command with appropriate pipe redirections.
// @param node : current command node in the linked list
// @param env : environment variables list
// @param pipefd : current pipe file descriptors [read, write]
// @param pipe_status : read end of previous pipe, or -1 if none
// @call	fork(), dup2(), close(), execve(), path(), env_to_array()
// @return	child PID, or 1 on fork error.

int ck_pid(int *pipefd, int pipe_status, t_cmd *node);
// Closes previous pipe and rotates to current pipe's read end.
// @param node : current command node (to check for next command)
// @param pipefd : current pipe file descriptors [read, write]
// @param pipe_status : read end of previous pipe, or -1 if none
// @call	close()
// @return	new pipe_status (read end of current pipe), or -1 if last cmd.