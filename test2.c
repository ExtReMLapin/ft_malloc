/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/08 14:49:06 by byoung-w          #+#    #+#             */
/*   Updated: 2018/02/05 12:04:07 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/malloc.h"



int			main(void)
{
	int i = 1; // not 0

	char *dada;

	while (i < 6000)
	{
		dada = (char*)malloc(sizeof(char)*i);
		int i2 = 0;
		while (i2 < i)
			dada[i2++] = 'A';
		dada[i2++] = '\0';

		i++;
	}
	show_alloc_mem();
	printf("%s\n", "reee");
	return (0);
}