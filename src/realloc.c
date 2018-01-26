/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:26:45 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:26:46 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

static void	*reallocret1(t_retplgmlc *data, size_t s, void *ptr)
{
	t_malloc	*newmlc;

	newmlc = find_free_space_plages(data->plage, s);
	ft_memcpy(newmlc->data, ptr, mathmin(data->mlc->end - data->mlc->data, s));
	data->mlc->past->next = newmlc;
	data->mlc->next->past = newmlc;
	return (newmlc->data);
}

static void	*reallocret2(t_retplgmlc *data, size_t s, void *ptr)
{
	t_malloc	*newmlc;

	newmlc = find_free_space_plages(data->plage, s);
	ft_memcpy(newmlc->data, ptr, mathmin(data->mlc->end - data->mlc->data, s));
	data->mlc->past->next = newmlc;
	return (newmlc->data);
}

static void	*reallocret3(t_retplgmlc *d, size_t size, void *ptr, t_plage *cp)
{
	t_malloc	*newmlc;

	newmlc = find_free_space_plages(cp, size);
	ft_memcpy(newmlc->data, ptr, mathmin(d->mlc->end - d->mlc->data, size));
	free(ptr);
	return (newmlc->data);
}

static void	*reallocret4(t_retplgmlc *data, size_t size, void *ptr)
{
	data->mlc->end = data->mlc->data + size;
	return (ptr);
}

void		*reallocsub(t_retplgmlc *d, size_t s, void *ptr, t_plage *cp)
{
	if (d->plage == cp)
	{
		if (d->mlc->next != NULL)
		{
			if (((void*)d->mlc->next - sizeof(int) - ptr) >= (long)s)
				return (reallocret4(d, s, ptr));
			else
				return (reallocret1(d, s, ptr));
		}
		else
		{
			if (d->plage->max_allowed_alloc > (ptr + s))
				return (reallocret4(d, s, ptr));
			else
				return (reallocret2(d, s, ptr));
		}
	}
	else
		return (reallocret3(d, s, ptr, cp));
}
