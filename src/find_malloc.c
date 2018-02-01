/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_malloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:32:48 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:32:50 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

t_malloc		*find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage		*pb;
	t_malloc	*mal;

	if (plage == NULL || !plage->data || ptr == NULL)
		return (NULL);
	pb = plage;
	while (pb)
	{
		if (ptr > (void*)pb->data && ptr < pb->max_allowed_alloc)
		{
			mal = pb->data;
			while (mal)
			{
				if (mal->data == ptr)
					return (mal);
				mal = mal->next;
			}
			return (NULL);
		}
		else if (pb->next)
			pb = pb->next;
		else
			break ;
	}
	return (NULL);
}

t_plage			*find_cmalloc_in(void *ptr)
{
	t_plage	*browse;

	if (ptr == NULL)
		return (NULL);
	browse = g_alc_mng.custom_plage;
	while (browse)
	{
		if (&browse->data + 1 == ptr)
			return (browse);
		if (browse->next)
			browse = browse->next;
		else
			break ;
	}
	return (NULL);
}

t_retplgmlc		find_mallocandplage(void *ptr)
{
	t_retplgmlc	ret;
	t_malloc	*malfind;

	ret.plage = NULL;
	ret.mlc = NULL;
	if (ptr == NULL)
		return (ret);
	malfind = find_malloc_in(ptr, g_alc_mng.small_plage);
	if (malfind)
	{
		ret.plage = g_alc_mng.small_plage;
		ret.mlc = malfind;
		return (ret);
	}
	malfind = find_malloc_in(ptr, g_alc_mng.med_plage);
	if (malfind)
	{
		ret.plage = g_alc_mng.med_plage;
		ret.mlc = malfind;
		return (ret);
	}
	return (ret);
}
