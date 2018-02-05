/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_freespace.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:32:12 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/05 10:43:17 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

t_malloc *dothing(t_malloc **tmp1, t_malloc *cmlc, size_t w)
{
	t_malloc *tmp;

	tmp = *tmp1;
	tmp = cmlc->end + sizeof(void*);
	init_malloc(tmp, w - sizeof(t_malloc));
	tmp->next = cmlc->next;
	tmp->next->past = tmp;
	cmlc->next = tmp;
	return (tmp);
}

static t_malloc	*find_freespace(t_plage *p, size_t w)
{
	t_malloc *cmlc;
	t_malloc *tmp;

	if (p == NULL || w > p->size || w == 0)
		return (NULL);
	if (p->data == NULL)
	{
		p->data = init_malloc(&p->data + sizeof(void*), w - sizeof(t_malloc));
		tmp = (t_malloc*)(p->data);
		tmp->past = NULL;
		return (tmp);
	}
	cmlc = p->data;
	while (cmlc && cmlc->next)
	{
		if ((size_t)((void*)cmlc->next - (cmlc->end + sizeof(void*))) > w)
			return (dothing(&tmp, cmlc, w));
		cmlc = cmlc->next;
	}
	if ((cmlc->end + sizeof(void*) + w) < p->max_allowed_alloc)
	{
		tmp = cmlc->end + sizeof(void*);
		cmlc->next = tmp;
		tmp->past = cmlc;
		init_malloc(tmp, w - sizeof(t_malloc));
		return (tmp);
	}
	return (NULL);
}

static t_malloc	*find_free_space_plages_bool(t_plage *pb, size_t w, t_malloc *t)
{
	while (pb)
	{
		t = find_freespace(pb, w);
		if (t != NULL)
			return (t);
		if (pb->next)
			pb = pb->next;
		else
			return (NULL);
	}
	return (NULL);
}

t_malloc		*find_free_space_plages(t_plage *plage, size_t w)
{
	bool		found;
	t_malloc	*target;
	t_plage		*p;

	target = NULL;
	if (plage == NULL || w == 0)
		return (NULL);
	p = plage;
	target = find_free_space_plages_bool(p, w, target);
	found = target != NULL;
	if (!found)
	{
		p->next = ezmmap(p->size);
		init_page(p->next, p->size, false);
		p->next->past = p;
		p->next->data = init_malloc((void*)(&p->next->data)\
			+ sizeof(char), w - sizeof(t_malloc));
		return (p->next->data);
	}
	else
		return (target);
}
