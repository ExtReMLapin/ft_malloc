/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test4.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/09/08 14:49:06 by byoung-w          #+#    #+#             */
/*   Updated: 2018/02/01 12:49:57 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/malloc.h"



static void fill(char *str, size_t size)
{
	size_t i = 0;
	while (i < size)
	{
		str[i] = 33 + i%94;
		i++;
	}
	str[i] = 0;
}


static bool eq(char *str, char* str2)
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



int main()
{


	int i = 0;

	while (i < 8000)
	{
		char *str = (char*)malloc(sizeof(char)*(i+1));
		fill(str,i);
		char *str2 = (char*)malloc(sizeof(char)*(i+50));
		fill(str2,i+49);
		if (eq(str, str2) == false)
			printf("%s\n", "reeeeee");
		i++;
	}
	printf("%s\n", "phase 1 done");

	i = 0;

	while (i < 8000)
	{
		char *str = (char*)malloc(sizeof(char)*(i+1));
		char *str2 = (char*)malloc(sizeof(char)*(i+50));
		fill(str2,i+49);
		fill(str,i);
		if (eq(str, str2) == false)
			printf("%s\n", "reeeeee");
		i++;
	}

	printf("%s\n", "phase 2 done");
}