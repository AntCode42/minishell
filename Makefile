# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agard <agard@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/17 14:30:27 by gchaillo          #+#    #+#              #
#    Updated: 2026/05/05 15:33:42 by agard            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline

INC_DIR = include
INCLUDES = -I $(INC_DIR) -I $(LIBFT_DIR)

LIBFT_DIR = include/libft
LIBFT = $(LIBFT_DIR)/libft.a

SRC_DIR = src
OBJ_DIR = obj

SRC = $(shell find $(SRC_DIR) -name "*.c")

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
RED = \033[0;31m
RESET = \033[0m
BOLD = \033[1m
BG_GREEN = \033[42m
BG_GRAY = \033[100m

TOTAL_FILES := $(words $(SRC))
COUNT_FILE = .compile_count

all: $(NAME)

$(LIBFT):
	@echo "$(CYAN)$(BOLD)📚 Compilation de libft...$(RESET)"
	@make -C $(LIBFT_DIR) --silent
	@echo "$(GREEN)✓ libft prête$(RESET)"
	@echo ""
	@echo "$(CYAN)$(BOLD)🛠️ Compilation de Minishell...$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@if [ ! -f $(COUNT_FILE) ]; then echo 0 > $(COUNT_FILE); fi
	@COUNT=$$(cat $(COUNT_FILE)); \
	COUNT=$$((COUNT + 1)); \
	echo $$COUNT > $(COUNT_FILE); \
	PERCENT=$$((COUNT * 100 / $(TOTAL_FILES))); \
	FILLED=$$((PERCENT / 2)); \
	EMPTY=$$((50 - FILLED)); \
	BAR=""; \
	i=0; while [ $$i -lt $$FILLED ]; do BAR="$${BAR}█"; i=$$((i + 1)); done; \
	i=0; while [ $$i -lt $$EMPTY ]; do BAR="$${BAR}░"; i=$$((i + 1)); done; \
	printf "\r$(YELLOW)⚙  $(RESET)[$(GREEN)$${BAR}$(RESET)] $(BOLD)%3d%%$(RESET) $(CYAN)%-25s$(RESET)" $$PERCENT $$(basename $<)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(LIBFT) $(OBJ)
	@rm -f $(COUNT_FILE)
	@echo ""
	@echo ""
	@echo "$(BLUE)🔗 Linkage...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)✓ EXECUTABLE CRÉE ✓$(RESET)"
clean:
	@echo "$(RED)🧹 Nettoyage des objets...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(COUNT_FILE)
	@make -C $(LIBFT_DIR) clean --silent
	@echo "$(GREEN)✓ Objets supprimés$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean --silent
	@echo "$(GREEN)✓ Exécutable supprimé$(RESET)"

re: fclean all

.PHONY: all clean fclean re
