/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:35:32 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/07 13:16:59 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

void			extprint(bool one, int i1, unsigned int nbr1, unsigned int plg1)
{
	if (one)
	{
		ft_putstr("	Page #");
		ft_putnbr(i1);
		ft_putstr(" with size ");
		ft_putnbr(nbr1);
		ft_putstr(" @ 0x10");
		print_hex(plg1);
		ft_putstr("\n");
	}
	else
	{
		ft_putstr("		alloc #");
		ft_putnbr(i1);
		ft_putstr(" [0x10");
		print_hex(nbr1);
		ft_putstr(" -> 0x10");
		print_hex(plg1);
		ft_putstr("\n");
	}
}

int				subprint(t_plage **plg, int i1)
{
	t_malloc	*m;
	t_plage		*plg1;
	int			i;

	i = 0;
	plg1 = *plg;
	m = plg1->data;
	extprint(true, i1++, plg1->size, (unsigned int)plg1);
	while (m)
	{
		extprint(false, ++i, (unsigned int)m->data, (unsigned int)m->end);
		if (m->next && (m->end + sizeof(void*) != m->next))
		{
			ft_putstr("		free space of ");
			ft_putnbr((void*)m->next - m->end);
			ft_putstr(" bytes\n");
		}
		m = m->next;
	}
	return (i1);
}

void			show_alloc_mem_plg(t_plage *plg, bool custom)
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
		{
			ft_putstr("	Page #");
			ft_putnbr(i1);
			ft_putstr(" | size ");
			ft_putnbr(plg1->size);
			ft_putstr(" @ 0x10");
			print_hex((unsigned int)plg1);
			ft_putstr("\n");
		}
		plg1 = plg1->next;
	}
}

void			show_alloc_mem(void)
{
	if (g_alc_mng.small_plage && g_alc_mng.small_plage->data)
	{
		ft_putstr("TINY : 0x10");
		print_hex((unsigned int)g_alc_mng.small_plage);
		ft_putstr("\n");
		show_alloc_mem_plg(g_alc_mng.small_plage, false);
	}
	if (g_alc_mng.med_plage && g_alc_mng.med_plage->data)
	{
		ft_putstr("MEDIUM : 0x10");
		print_hex((unsigned int)g_alc_mng.med_plage);
		ft_putstr("\n");
		show_alloc_mem_plg(g_alc_mng.med_plage, false);
	}
	if (g_alc_mng.custom_plage)
	{
		ft_putstr("LARGE : 0x10");
		print_hex((unsigned int)g_alc_mng.custom_plage);
		ft_putstr("\n");
		show_alloc_mem_plg(g_alc_mng.custom_plage, true);
	}
}

t_plage			*checkpage(size_t size)
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
