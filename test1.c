/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pierre <pierre@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/08 14:49:06 by byoung-w          #+#    #+#             */
/*   Updated: 2018/01/29 15:03:45 by pierre           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/malloc.h"



int			main(void)
{
	int i = 1; // not 0

	char *dada;

	while (i < 9995)
	{
		dada = (char*)malloc(sizeof(char)*i);
		int i2 = 0;
		while (i2 < i)
			dada[i2++] = 'A';
		dada[i2++] = '\0';
		i++;
	}

	return (0);
}