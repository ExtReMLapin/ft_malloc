/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/08 14:49:06 by byoung-w          #+#    #+#             */
/*   Updated: 2018/02/02 10:39:55 by pfichepo         ###   ########.fr       */
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
	write(1, "in\n",3);
	int i = 0; // not 0



	char **table = (char**)ezmmap(1500001);

	char *orgin = (char*)ezmmap(1024);

	fill(orgin, 1023);


	while (i < 1024)
	{
		table[i] =  (char*)malloc(1024);
		fill(table[i], 1023);
		i++;
	}

	i = 0;

	while (i < 1024)
	{
		if (! eq(table[i], orgin))
			printf("%s\n","reeeee" );
		i++;
	}

	printf("%s \n", "okay for size 1024");

	return (0);
}