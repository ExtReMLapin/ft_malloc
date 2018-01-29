/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pierre <pierre@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/18 13:31:01 by pierre           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

void				*malloc(size_t size)
{
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
	t_retplgmlc	data;
	t_plage		*cp;
	bool		icp;

	if (ptr == NULL)
		return (NULL);
	cp = checkpage(size);
	icp = cp == g_alc_mng.custom_plage;
	if (icp || find_cmalloc_in(ptr))
		return (special_custom_realloc(ptr, size, find_cmalloc_in(ptr), icp));
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return (NULL);
	return (reallocsub(&data, size, ptr, cp));
}
