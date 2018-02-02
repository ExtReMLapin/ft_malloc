/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   special_custom_realloc.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:33:55 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/02 11:12:55 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

static void	*special_custom_realloc01(size_t size, t_plage *incustom, void *ptr)
{
	void			*mlc2;

	mlc2 = malloc(size);
	ft_memcpy(mlc2, &incustom->data + sizeof(void*),
		mathmin(incustom->size, size));
	free(ptr);
	return (mlc2);
}

static void	*special_custom_realloc02(void *ptr, size_t size)
{
	void			*mlc2;
	t_retplgmlc		mlc;
	mlc = find_mallocandplage(ptr);
	mlc2 = malloc(size);
	ft_memcpy(mlc2, ptr, mathmin(mlc.mlc->end - mlc.mlc->data, size));
	return (mlc2);
}

void		*special_custom_realloc(void *p, size_t s, t_plage *ic, bool goc)
{
	void		*page;

	if (ic && goc)
	{
		if (ic->size - (sizeof(t_plage) + sizeof(void*)) >= s)
			return (p);
		else
		{
			page = malloc(s);
			ft_memcpy(page , &ic->data + 1,
				mathmin(ic->max_allowed_alloc - ((void*)&ic->data+1), s));
			free(p);
			return (page);
		}
	}
	else if (ic)
		return (special_custom_realloc01(s, ic, p));
	else
		return (special_custom_realloc02(p, s));
}

void		*special_custom_malloc(size_t size)
{
	t_plage	*plagebrowse;
	size_t	plagesize;

	plagesize = closestsize(size + sizeof(t_plage) + sizeof(void*));
	if (g_alc_mng.custom_plage == NULL)
	{
		g_alc_mng.custom_plage = (t_plage*)ezmmap(plagesize);
		init_page(g_alc_mng.custom_plage, plagesize, true);
		g_alc_mng.custom_plage->max_allowed_alloc = (void*)
		&g_alc_mng.custom_plage->data + size + 1;
		return (&g_alc_mng.custom_plage->data + 1);
	}

	plagebrowse = g_alc_mng.custom_plage;
	while (plagebrowse->next)
		plagebrowse = plagebrowse->next;
	plagebrowse->next = (t_plage*)ezmmap(plagesize);
	init_page(plagebrowse->next, plagesize, true);
	plagebrowse->next->past = plagebrowse;
	plagebrowse->next->max_allowed_alloc = (void*)&plagebrowse->next->data
	+ size + 1;
	return (&plagebrowse->next->data + 1);
}
