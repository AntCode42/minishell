/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:38:43 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/06 22:30:47 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

static int	is_input_empty(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (input[i] != ' ' && input[i] != '\t')
			return (0);
		i++;
	}
	return (1);
}

static void	execute_commands(t_mini *mini, t_token *tokens)
{
	t_cmd	*cmds;

	expand_tokens(tokens, mini);
	cmds = parse_tokens(tokens);
	free_tokens(tokens);
	if (!cmds)
		return ;
	mini->cmd = cmds;
	ck_pipe(mini);
	free_cmd(cmds);
	mini->cmd = NULL;
}

static void	process_input(t_mini *mini, char *input)
{
	t_token	*tokens;
	int		is_empty;

	mini->cmd = NULL;
	if (*input)
		add_history(input);
	is_empty = is_input_empty(input);
	tokens = lexer(input);
	free(input);
	if (!tokens)
	{
		if (!is_empty)
			mini->exit_status = 2;
		return ;
	}
	if (!check_syntax(tokens))
	{
		mini->exit_status = 2;
		free_tokens(tokens);
		return ;
	}
	execute_commands(mini, tokens);
}

static void	minishell_loop(t_mini *mini)
{
	char	*input;

	while (1)
	{
		g_signal_received = 0;
		set_signal_mode(SIG_MODE_INTERACTIVE);
		rl_done = 0;
		input = readline("minishell$ ");
		if (g_signal_received == SIGINT)
		{
			mini->exit_status = 130;
			g_signal_received = 0;
		}
		if (!input)
		{
			if (isatty(STDIN_FILENO) && isatty(STDERR_FILENO))
				ft_putendl_fd("exit", 2);
			break ;
		}
		process_input(mini, input);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_mini	mini;

	(void)argc;
	(void)argv;
	ft_memset(&mini, 0, sizeof(t_mini));
	mini.env = init_env(envp);
	rl_catch_signals = 0;
	minishell_loop(&mini);
	free_env(mini.env);
	return (mini.exit_status);
}
