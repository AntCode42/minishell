/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchaillo <gchaillo@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:38:43 by gchaillo          #+#    #+#             */
/*   Updated: 2026/03/30 16:32:20 by gchaillo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

/*
** Boucle principale du shell :
** 1. Affiche le prompt (readline) - OK
** 2. Lexer → tokens - OK
** 3. Parser → commandes (t_cmd)
** 4. Expander → expansion des $VAR
** 5. Executor → exécution
** 6. Free tout
** 7. Recommencer
*/

/*
ajout du check de la syntax pour éviter les if a tout va dans le code
attention pour après faut bien penser a garder le check de la syntaxe avant
de faire le parser
car pas de check après, si la syntaxe est mauvaise le parser peut faire
n'importe quoi
*/
static void	process_input(t_mini *mini, char *input)
{
	t_token	*tokens;
	t_cmd	*cmds;

	if (*input)
		add_history(input);
	tokens = lexer(input);
	free(input);
	if (!tokens)
		return ;
	// print_tokens(tokens);// imprime token avant expand
	if (check_syntax(tokens))
	{
		expand_tokens(tokens, mini);
		// printf("--- AFTER EXPANDER ---\n");// a supprimer apres
		// print_tokens(tokens); // Affiche les tokens après expansion - supprimer après hein
		cmds = parse_tokens(tokens);
		// print_cmd(cmds);// affiche les cmds
		mini->cmd = cmds;
		ck_pipe(mini);
		if (cmds)
			free_cmd(cmds);
	}
	else
		mini->exit_status = 2;
	free_tokens(tokens);
}

/*
j'ai découpé la loop car trop longue, c'est renvoyé dans le process_input
*/
void	minishell_loop(t_mini *mini)
{
	char	*input;

	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		process_input(mini, input);
	}
}
/*
pour le moment void envp mais devra etre utilisé pour environnement
*/
int	main(int argc, char **argv, char **envp)
{
	t_mini	mini;
	
	(void)argc;
	(void)argv;
	(void)envp;
	
	ft_memset(&mini, 0, sizeof(t_mini));
	
	t_env	env1;  // simule PATH=...
	t_env	env2;  // simule HOME=...

	
	// 1. Construire env1 (PATH)
	env1.key = "PATH";
	env1.value = "/usr/local/bin:/usr/bin:/bin";
	env1.next = &env2;

	// 2. Construire env2 (HOME) - juste pour avoir une liste
	env2.key = "HOME";
	env2.value = "/home/chapoux";
	env2.next = NULL;

	// 4. Assembler t_mini
	mini.env = &env1;
	
	minishell_loop(&mini);
	return (mini.exit_status);
}

