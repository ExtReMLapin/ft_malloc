/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:35:32 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/07 11:03:50 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

int					subprint(t_plage **plg, int i1)
{
	t_malloc	*m;
	t_plage		*plg1;
	int			i;

	i = 0;
	plg1 = *plg;
	m = plg1->data;
	printf("	Page #%i with size %lu @ %p\n", i1++, plg1->size, plg1);
	while (m)
	{
		printf("		alloc #%i %p [%p -> %p] %lu bytes\n", ++i, m,
			m->data, m->end, m->end - m->data - sizeof(void*));
		if (m->next && (m->end + sizeof(void*) != m->next))
		{
			printf("		free space of %lu bytes from %p to %p \n",
				(void*)m->next - m->end, m->end + 1, (void*)m->next - 1);
		}
		m = m->next;
	}
	return (i1);
}

void				show_alloc_mem_plg(t_plage *plg, bool custom)
{
	int			i;
	int			i1;
	t_plage		*plg1;

	i1 = 0;
	plg1 = plg;
	while (plg1)
	{
		i = 0;
		if (!custom)
			i1 = subprint(&plg1, i1);
		else
			printf("	Page #%i | size %lu @ %p\n", ++i1, plg1->size, plg1);
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
			g_alc_mng.small_plage = (t_plage*)ezmmap(4 * getpagesize());
			init_page(g_alc_mng.small_plage, 4 * getpagesize(), false);
		}
		return (g_alc_mng.small_plage);
	}
	else if (size <= MAX_MED_SIZE)
	{
		if (g_alc_mng.med_plage == NULL)
		{
			g_alc_mng.med_plage = (t_plage*)ezmmap(5 * getpagesize());
			init_page(g_alc_mng.med_plage, 5 * getpagesize(), false);
		}
		return (g_alc_mng.med_plage);
	}
	else
		return (g_alc_mng.custom_plage);
}
