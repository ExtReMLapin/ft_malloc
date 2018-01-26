/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_freespace.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:32:12 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:32:14 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

static t_malloc	*find_freespacewhile(t_malloc *mlc, size_t w)
{
	t_malloc	*tmp;

	while (mlc && mlc->next)
	{
		if ((size_t)((void*)mlc->next - (mlc->end + sizeof(void*))) > w)
		{
			tmp = mlc->end + sizeof(void*);
			init_malloc(tmp, w - sizeof(t_malloc));
			tmp->next = mlc->next;
			tmp->next->past = tmp;
			mlc->next = tmp;
			return (tmp);
		}
		mlc = mlc->next;
	}
	return (NULL);
}

static t_malloc	*find_freespace(t_plage *p, size_t w)
{
	t_malloc *curmalloc;
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
	curmalloc = p->data;
	tmp = find_freespacewhile(curmalloc, w);
	if (tmp != NULL)
		return (tmp);
	if ((curmalloc->end + sizeof(void*) + w) < p->max_allowed_alloc)
	{
		tmp = curmalloc->end + sizeof(void*);
		curmalloc->next = tmp;
		tmp->past = curmalloc;
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
