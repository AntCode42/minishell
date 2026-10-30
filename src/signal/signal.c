/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:08:26 by gchaillo          #+#    #+#             */
/*   Updated: 2026/05/05 15:30:57 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Gère le signal SIGINT (Ctrl+C) en mode interactif
static void	handle_sigint_prompt(int signum)
{
	(void)signum;
	g_signal_received = SIGINT;
	write(1, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	handle_sigint_heredoc(int signum)
{
	(void)signum;
	g_signal_received = SIGINT;
	close(STDIN_FILENO);
	write(1, "^C", 2);
}

// Initialise un signal donne avec une action specifique
static void	init_sig(int signum, void (*handler)(int))
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	sigaction(signum, &sa, NULL);
}

// Configure le comportement des signaux (interactif, execution, etc.)
void	set_signal_mode(t_signal_mode mode)
{
	if (mode == SIG_MODE_INTERACTIVE)
	{
		init_sig(SIGINT, handle_sigint_prompt);
		init_sig(SIGQUIT, SIG_IGN);
	}
	else if (mode == SIG_MODE_EXECUTION)
	{
		init_sig(SIGINT, SIG_IGN);
		init_sig(SIGQUIT, SIG_IGN);
	}
	else if (mode == SIG_MODE_DEFAULT)
	{
		init_sig(SIGINT, SIG_DFL);
		init_sig(SIGQUIT, SIG_DFL);
	}
	else if (mode == SIG_MODE_HEREDOC)
	{
		init_sig(SIGINT, handle_sigint_heredoc);
		init_sig(SIGQUIT, SIG_IGN);
	}
}

// Convertit le statut d'attente d'un processus en code de retour exit (0-255)
int	wait_status_to_exit_code(int status)
{
	int	sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(1, "\n", 1);
		else if (sig == SIGQUIT)
			write(1, "Quit (core dumped)\n", 19);
		return (128 + sig);
	}
	return (1);
}
