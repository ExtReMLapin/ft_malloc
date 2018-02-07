# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/01/28 13:26:56 by pfichepo          #+#    #+#              #
#    Updated: 2018/02/07 11:43:24 by pfichepo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

CC = gcc
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

.PHONY: fclean all clean re
