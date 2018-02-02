/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:31:13 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/02 10:53:27 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

void				init_page(t_plage *plage, size_t size, bool custom)
{
	if (plage == NULL)
		return ;
	plage->data = NULL;
	plage->next = NULL;
	plage->past = NULL;
	plage->size = size;
	plage->custom = custom;
	plage->max_allowed_alloc = (void*)plage + size;
}

t_malloc			*init_malloc(void *ptr, size_t size)
{
	t_malloc	*mlc;

	if (ptr == NULL)
		return (NULL);
	mlc = (t_malloc*)ptr;
	mlc->data = (void*)(&(mlc->data)) + sizeof(void*);
	mlc->end = mlc->data + size + sizeof(void*);
	mlc->next = NULL;
	return (mlc);
}
