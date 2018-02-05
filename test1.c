/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/08 14:49:06 by byoung-w          #+#    #+#             */
/*   Updated: 2018/02/05 12:10:35 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/malloc.h"

static void fill(char *str, size_t size) // on remplis la string de char de la table ascii, pas que le meme pour detecter le decalage possible
{
	size_t i = 0;
	while (i < size)
	{
		str[i] = 33 + i%94;
		i++;
	}
	str[i] = 0;
}


static bool eq(char *str, char* str2) // on check == des 2 strings, s'arrete a la fin de la plus petite string
{
	int i = 0;

	while (str[i] && str2[i])
	{
		if (str[i] != str2[i])
			return (false);
		i++;
	}
	return (true);
}


int			main(void)
{
	int i = 0; // not 0


	while (i < 39)
	{
		malloc(1024);
		printf("%i\n",i );
		i++;
	}


	show_alloc_mem();

	return (0);
}