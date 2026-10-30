/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 11:50:21 by agard             #+#    #+#             */
/*   Updated: 2026/05/06 09:42:08 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTION_H
# define EXECUTION_H

# include "minishell.h"

typedef struct s_mini	t_mini;
typedef struct s_cmd	t_cmd;
typedef struct s_env	t_env;
typedef struct s_redir	t_redir;

/* ************************************************************************** */
// ./execution/sort_cmd.c

int		ck_pipe(t_mini *mini);
// Determines whether there are pipes or not.
// @param 	mini : main struct containing all sub-structs
// @call	pipeline() | single()
// @return	exit_status. 1 if mini is NULL.

int		ext_func_finder(char **list_cmd);
// Determines whether the command is a builtin or external.
// @param 	list_cmd : array of command + options (NULL-terminated)
// @call	ft_strncmp(), ft_strlen()
// @return	0 = external, 1 = builtin, 2 = error
/* ************************************************************************** */
// ./execution/fork_management.c

int		create_single_fork(t_mini *mini);
// Creates a child process to execute an external command.
// @param 	mini : main struct containing all sub-structs
// @call	fork(), execve(), perror(), waitpid()
// @call	WIFEXITED(), WEXITSTATUS()
// @return	exit_status.

int		fork_all_commands(t_mini *mini, pid_t *last_pid);
int		check_heredoc_error(t_cmd *node, t_mini *mini, int pipe_status);
// Forks all commands in the pipeline with proper pipe chaining.
// @param 	mini : main struct containing all sub-structs
// @param 	last_pid : output parameter to store the last forked PID
// @call	pipe(), go_through_pipe(), ck_pipe(), close()
// @return	total number of commands forked.
/* ************************************************************************** */
// ./execution/utils/utils_fork.c

void	free_array(char **array);
// Free a **array
// @param 	array : array to free

char	**env_to_array(t_env *env);
// transforme the chaine list t_env in a **array.
// @param 	env : struct containing the environement
// @ call	count_node(), ft_strjoin(), free()
// @return	a **array containing all the environement

int		count_node(t_env *env);
// Count the number of node in a simple chaine liste.
// @param 	env : struct containing the environement
// @call	none
// @return	the number of node forom 0 et n.

char	*path(t_env *env, char *cmd, int flag);
// return the correct path for the bin of the commande cmd in param
// @param 	env : struct containing the environement
// @param 	cmd : argument containing the name of the commande to find
// @call	find_path(), ft_split(), join_cmd(), access()
// @return	the correct path for the bin
/* ************************************************************************** */
// ./execution/pipe_management.c

int		pipeline(t_mini *mini);
// Routes pipeline commands to executor or builtin handler.
// @param 	mini : main struct containing all sub-structs
// @call	ext_func_finder(), execute_pipeline(), builtin_sort()
// @return	exit_status.

int		go_through_pipe(t_mini *mini, t_cmd *node, int *p_fd, int p_status);
// Forks a single command with appropriate pipe redirections.
// @param 	node : current command node in the linked list
// @param 	env : environment variables list
// @param 	p_fd : current pipe file descriptors [read, write]
// @param 	p_status : read end of previous pipe, or -1 if none
// @call	fork(), dup2(), close(), execve(), path(), env_to_array()
// @return	child PID, or 1 on fork error.

int		wait_all_children(int count, pid_t last_pid);
// Waits for all child processes, retrieving exit status from the last one.
// @param 	count : total number of forked processes
// @param 	last_pid : PID of the last command in pipeline
// @call	wait(), waitpid(), WIFEXITED(), WEXITSTATUS()
// @return	exit_status of last_pid. 1 if abnormal termination.

int		ck_pid(int *pipefd, int pipe_status, t_cmd *node);
// Closes previous pipe and rotates to current pipe's read end.
// @param 	node : current command node (to check for next command)
// @param 	pipefd : current pipe file descriptors [read, write]
// @param 	pipe_status : read end of previous pipe, or -1 if none
// @call	close()
// @return	new pipe_status (read end of current pipe), or -1 if last cmd.
/* ************************************************************************** */
// ./execution/redir_management.c

int		ft_redir(t_redir *redir);
// Check if there is a redirection or not. If a redirection is find, the
// function will lunch the correct function depend on the redirection type
// @param 	redir : struct containing all info on the redirection
// @call    ft_redir_in(), ft_redir_out(), ft_append(), mk_heredoc()
// @return  1 if no redirection found, 0 if redirection was applied
/* ************************************************************************** */
// ./execution/heredoc_management.c

int		ft_heredoc(t_redir *redir, t_mini *mini);
// Reads lines from the user until the delimiter is found, writes each line
// into a pipe and stores the read-end fd in redir->int_fd for later use
// @param 	redir : struct containing the delimiter string and the fd storage
// @call    pipe(), readline(), ft_strcmp(), write(), free(), close()
// @return  1 if redir is NULL, 0 on success

int		mk_heredoc(t_redir *redir);
// Redirects stdin (fd 0) to the pipe read-end stored in redir->int_fd,
// making the heredoc content available as standard input for the command
// @param 	redir : struct containing the pipe read-end fd in redir->int_fd
// @call    dup2(), close()
// @return  0 on success
/* ************************************************************************** */
// ./execution/builtin/builtin_management.c

int		builtin_sort(t_mini *mini);
// detects which built-in function is being called and executes the
// corresponding command.
// @param 	mini : main struct containing all sub-structs.
// @call	ft_strncmp, ft_echo, ft_cd, ft_pwd
// @call	ft_export, ft_unset, ft_env, ft_exit
// @return	exit_status

/* ************************************************************************** */
// ./execution/builtin/builtin_func.c

int		ft_echo(t_mini *mini);
// sends the **list_cmd to isolate the input and prints it to standard output
// @param 	mini : main struct containing all sub-structs.
// @call	--- (for print in stdout)
// @return	exit_status

int		ft_cd(t_mini *mini);
// Dispatches the cd command to the appropriate handler based on the input.
// @param   mini : the main shell structure containing env, cmd and exit_status
// @return  0 on success, exit_status on failure

int		ft_pwd(t_mini *mini);
// Prints the current working directory to stdout.
// @param   mini : the main shell structure containing exit_status
// @return  0 on success, 1 on failure
/* ************************************************************************** */
// ./execution/builtin/cd_management.c
int		cd_regular(t_mini *mini);
// Handles cd with a path argument (absolute, relative, .., ., ...).
// Updates PWD and OLDPWD in the environment.
// @param   mini : the main shell structure containing env, cmd and exit_status
// @return  0 on success, exit_status on failure

int		cd_home(t_mini *mini);
// Handles cd with no argument or ~. Changes to the HOME directory.
// Updates PWD and OLDPWD in the environment.
// @param   mini : the main shell structure containing env and exit_status
// @return  0 on success, exit_status on failure

int		cd_minus(t_mini *mini);
// Handles cd -. Changes to the previous directory stored in OLDPWD.
// Updates PWD and OLDPWD in the environment.
// @param   mini : the main shell structure containing env and exit_status
// @return  0 on success, exit_status on failure

int		ft_exit(t_mini *mini);
// Handles the exit command. Validates the argument if present and exits
// the shell with the appropriate status code.
// @param   mini : the main shell structure containing cmd and exit_status
// @return  1 if too many arguments, does not return otherwise
/* ************************************************************************** */
// ./execution/utils/utils_share.c

int		ft_strcmp(char *s1, char *s2);
// Compares two strings character by character
// @param	s1 : first string
// @param	s2 : second string
// @return  0 if strings are equal, positive value if s1 > s2,
//  negative value if s1 < s2

char	*ft_strcat(char *dest, char *src);
// Concatenates src at the end of dest and returns the resulting string.
// @param   dest : the destination string, must be allocated
// @param   src  : the string to append to dest
// @return  the concatenated string

int		external(t_mini *mini, char **env_arr, char *cmd_path);
// Handles the case where the command is a directory and exits the program.
// @param   mini     : the main minishell structure containing cmd and env
// @param   env_arr  : the environment array to free
// @param   cmd_path : the resolved command path to free
// @return  does not return (exits with status 126)

/* ************************************************************************** */
// ./execution/utils/utils_builtin.c

int		check_inpt(t_cmd *cmd);
// Checks the type of argument passed to cd.
// @param   cmd : the command structure containing cmd_args
// @return  1 for regular path, 2 for -, 3 for ~ or no argument, 0 for error

int		ck_alpha(char *input);
// Checks whether a string contains any alphabetical character.
// @param   input : the string to check
// @return  1 if an alpha character is found, 0 otherwise

int		is_n_option(char *arg);
// Checks if the argument is a valid "-n" option (e.g., -n, -nnn).
// @param   arg : the argument string to check
// @return  1 if the argument is a valid -n option, 0 otherwise
/* ************************************************************************** */
// ./execution/utils/utils_pipe.c

void	dir_pipe(t_cmd *node, int *pipefd, int pipe_status);
// directe and redirecte the file desciptor of the pipe if they is a other pipe
// or not
// @param	node : current command node (to check for next command)
// @param	pipefd : current pipe file descriptors [read, write]
// @para	pipe_status : check if there is a other pipe or not
// @return	void.

#endif