/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:00:00 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 15:43:49 by gchaillo         ###   ########.fr       */
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

extern volatile sig_atomic_t	g_signal_received;

//structure pour type de token

typedef enum token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC,
}	t_token_type;

// structure pour valeur de token
typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

//struct a remplir pour envoyer la sauce
typedef struct s_redir
{
	char			*file; //nom du fichier a utiliser
	int				int_fd;
	t_token_type	type;
	struct s_redir	*next;
}	t_redir;

// liste des commandes
typedef struct s_cmd
{
	char			**cmd_args; // commandes et options parsées (mots)
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

t_env		*init_env(void);
t_token		*lexer(char *input);
t_token		*create_token(char *value, t_token_type type);
void		add_token(t_token **list, t_token *new);
void		free_tokens(t_token *list);

t_cmd		*parse_tokens(t_token *tokens);
t_cmd		*init_cmd(void);
int			check_syntax(t_token *tokens);
int			is_redir_type(t_token_type type);
int			count_cmd_words(t_token *tokens);
void		add_redir(t_cmd *cmd, t_token_type type, char *file);
void		free_cmd(t_cmd *cmd);

// expander
void		expand_tokens(t_token *tokens, t_mini *mini);
char		*expand_word(char *word, t_mini *mini);
char		*expand_dollar(char *str, int *i, char *res, t_mini *mini);
char		*handle_dquote(char *str, int *i, char *res, t_mini *mini);
char		*join_and_free(char *s1, char *s2);
char		*append_char(char *str, char c);
char		*get_env_val(t_env *env, char *key);

//debug
// void		print_tokens(t_token *list);
// void		print_cmd(t_cmd *cmd);

#include "execution/execution.h"

#endif
