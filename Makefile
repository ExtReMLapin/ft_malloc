# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tvisenti <tvisenti@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/09/14 10:52:08 by tvisenti          #+#    #+#              #
#    Updated: 2017/10/09 17:05:34 by tvisenti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

CC = clang
CFLAGS =  -Wall -Werror -Wextra

SRC_PATH = ./src
SRC_NAME = find_freespace.c init.c utils.c find_malloc.c realloc.c utils2.c \
			free.c malloc.c special_custom_realloc.c


SRC = $(addprefix $(SRC_PATH)/, $(SRC_NAME))
OBJ = $(SRC:.c=.o)
INC = -I includes
MAL = inc/malloc.h

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJ)   $(INC) 
	@ln -sf $(NAME) $(LINK)
	@echo "\033[1;34mMalloc\t\t\033[1;33mCompilation\t\033[0;32m-OK-\033[0m"

%.o: %.c
	@$(CC) -c $(CFLAGS) $(INC)  $^ -o $@ -fPIC

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(LINK)

re: fclean all

norme:
	@cd libft && make norme
	@echo "\033[1;34mMalloc\t\t\033[1;33mNorminette\t\033[0;32m-OK-\033[0m"
	@norminette $(SRC) $(MAL)

.PHONY: fclean all clean re norme