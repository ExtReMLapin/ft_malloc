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

	char *dada = (char*)malloc(1);


	while (i < 15000)
	{
		dada = (char*)realloc(dada, sizeof(char)*i);
		dada[0] = 'H';
		dada[i-1] = '\0';
		i++;
	}

	return (0);
}