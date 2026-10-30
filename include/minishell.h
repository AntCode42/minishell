/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoine-pro <antoine-pro@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:00:00 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/10 14:09:00 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# define _GNU_SOURCE

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"
# include <errno.h>
# include "execution/execution.h"

extern volatile sig_atomic_t	g_signal_received;

typedef enum e_signal_mode
{
	SIG_MODE_INTERACTIVE,
	SIG_MODE_EXECUTION,
	SIG_MODE_DEFAULT,
	SIG_MODE_HEREDOC,
}	t_signal_mode;

void		set_signal_mode(t_signal_mode mode);
int			wait_status_to_exit_code(int status);

//structure pour type de token

typedef enum token_type
{
	TOKEN_WORD,
	TOKEN_EMPTY,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
}	t_token_type;

// structure pour valeur de token (ajout quoted pour heredoc)
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	int				quoted;
	struct s_token	*next;
}	t_token;

//struct a remplir pour envoyer la sauce (ajout quoted pour heredoc)
typedef struct s_redir
{
	char			*file;
	int				int_fd;
	t_token_type	type;
	int				quoted;
	struct s_redir	*next;
}	t_redir;

// liste des commandes
typedef struct s_cmd
{
	char			**cmd_args;
	t_redir			*redir_cmd;
	struct s_cmd	*next;
	struct s_cmd	*prev;
}	t_cmd;

// environnement pour expender
typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

// structure de base
typedef struct s_mini
{
	t_env	*env;
	t_cmd	*cmd;
	int		exit_status;
}	t_mini;

t_env		*init_env(char **envp);
void		free_env(t_env *env);
int			ft_export(t_mini *mini);
int			ft_unset(t_mini *mini);
int			ft_env(t_mini *mini);

// export utils
void		split_key_value(const char *str, char **key, char **value);
int			is_valid_key(const char *key);
t_env		*find_env_node(t_env *env, const char *key);
void		append_env_node(t_env **env, t_env *new_node);
t_env		*new_env_node(const char *key, const char *value);
void		print_export(t_env *env);
char		*normalize_spaces(const char *value);

t_token		*lexer(char *input);
t_token		*read_next_token(char *input, int *i);
t_token		*read_operator(char *input, int *i);
t_token		*read_word(char *input, int *i);
int			read_word_loop(char *input, int *i, int *quoted);
int			skip_quotes(char *input, int *i);
t_token		*create_token(char *value, t_token_type type);
void		add_token(t_token **list, t_token *new);
void		free_tokens(t_token *list);

t_cmd		*parse_tokens(t_token *tokens);
t_cmd		*init_cmd(void);
int			check_syntax(t_token *tokens);
int			is_redir_type(t_token_type type);
int			count_cmd_words(t_token *tokens);
int			add_redir(t_cmd *cmd, t_token_type type, char *file, int quoted);
void		free_cmd(t_cmd *cmd);

// expander
void		expand_tokens(t_token *tokens, t_mini *mini);
char		*join_and_free(char *s1, char *s2);
char		*append_char(char *str, char c);
char		*get_env_val(t_env *env, char *key);
char		*expand_tilde(char *str, int *i, char *res, t_mini *mini);
char		*expand_heredoc_line(char *line, t_mini *mini);
char		*expand_exit_status(char *res, t_mini *mini, int *i);
char		*expand_env_key(char *line, int *i, char *res, t_mini *mini);
int			is_unquoted_var(char *original);

#endif
