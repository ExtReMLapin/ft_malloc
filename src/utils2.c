/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:35:32 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:35:33 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

void				show_alloc_mem_plg(t_plage *plg, bool custom)
{
	int			i;
	int			i1;
	t_malloc	*mlc;
	t_plage		*plg1;

	i1 = 0;
	plg1 = plg;
	while (plg1)
	{
		i = 0;
		if (!custom)
		{
			mlc = plg1->data;
			printf("	Plage #%i with size %lu @ %p\n", ++i1, plg1->size, plg1);
			while (mlc)
			{
				printf("		alloc #%i %p [%p -> %p] %lu bytes\n", ++i, mlc, mlc->data, mlc->end, mlc->end - mlc->data - sizeof(void*));
				if (mlc->next && (mlc->end + sizeof(void*) != mlc->next))
					printf("		free space of %lu bytes from %p to %p \n", (void*)mlc->next - mlc->end, mlc->end + 1, (void*)mlc->next - 1);
				mlc = mlc->next;
			}
		}
		else
			printf("	Custom plage #%i with size %lu @ %p\n", ++i1, plg1->size, plg1);
		plg1 = plg1->next;
	}
}

void				show_alloc_mem(void)
{
	if (g_alc_mng.small_plage && g_alc_mng.small_plage->data)
	{
		printf("TINY : %p \n", g_alc_mng.small_plage);
		show_alloc_mem_plg(g_alc_mng.small_plage, false);
	}
	if (g_alc_mng.med_plage && g_alc_mng.med_plage->data)
	{
		printf("MEDIUM : %p \n", g_alc_mng.med_plage);
		show_alloc_mem_plg(g_alc_mng.med_plage, false);
	}
	if (g_alc_mng.custom_plage)
	{
		printf("LARGE : %p \n", g_alc_mng.custom_plage);
		show_alloc_mem_plg(g_alc_mng.custom_plage, true);
	}
}

t_plage				*checkpage(size_t size)
{
	if (size <= MAX_TINY_SIZE)
	{
		if (g_alc_mng.small_plage == NULL)
		{
			g_alc_mng.small_plage = (t_plage*)ezmmap(TINY_PAGE_SIZE);
			init_page(g_alc_mng.small_plage, TINY_PAGE_SIZE, false);
		}
		return (g_alc_mng.small_plage);
	}
	else if (size <= MAX_MED_SIZE)
	{
		if (g_alc_mng.med_plage == NULL)
		{
			g_alc_mng.med_plage = (t_plage*)ezmmap(MED_PAGE_SIZE);
			init_page(g_alc_mng.med_plage, MED_PAGE_SIZE, false);
		}
		return (g_alc_mng.med_plage);
	}
	else
		return (g_alc_mng.custom_plage);
}
