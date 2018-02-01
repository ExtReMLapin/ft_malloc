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
	int i = 2050;

	char *dada;

	/*while (i < 9000)
	{*/
		dada = (char*)malloc(sizeof(char)*i);
		dada = (char*)realloc(dada,i*2);
		dada[0] = '4';
		dada[i-1] = '\0';


		i++;
	//}



	return (0);
}