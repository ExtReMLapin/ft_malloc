/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/02 10:03:56 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

void				*malloc(size_t size)
{
	//printf( "MALLOC CALLED %lu\n", size);
	write(1, "MALLOC CALLED\n", 15);
	t_plage		*target;
	t_malloc	*adr;

	if (size == 0)
		return (NULL);
	target = checkpage(size);

	if (target && target->custom == false)
		adr = find_free_space_plages(target, size + sizeof(t_malloc));
	else
		return (special_custom_malloc(size));
	if (adr != NULL)
		return (adr->data);
	return (NULL);
}

void				*realloc(void *ptr, size_t size)
{
	//printf( "REALLOC CALLED %p %lu \n",ptr,  size);
	write(1, "REALLOC CALLED\n", 16);
	t_retplgmlc	data;
	t_plage		*cp;
	bool		icp;

	if (ptr == NULL)
		return (malloc(size));
	cp = checkpage(size);
	icp = cp == g_alc_mng.custom_plage;
	if (icp || find_cmalloc_in(ptr))
		return (special_custom_realloc(ptr, size, find_cmalloc_in(ptr), icp));
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return (NULL);
	return (reallocsub(&data, size, ptr, cp));
}



void				*calloc(size_t nmemb, size_t size)
{
		write(1, "CALLOC CALLED\n", 15);

	unsigned char	*ptr;
	size_t			idx;

	if ((ptr = malloc(size * nmemb)) == NULL)
		return (NULL);
	idx = 0;
	while (idx < size)
	{
		ptr[idx] = 0;
		++idx;
	}
	return (ptr);
}