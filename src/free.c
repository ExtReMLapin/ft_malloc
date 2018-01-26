/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:35:00 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:35:01 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

static bool		freecustomsizeptr(void *ptr)
{
	t_plage	*browse;

	if (ptr == NULL)
		return (true);
	if (g_alc_mng.custom_plage == NULL)
		return (false);
	browse = find_cmalloc_in(ptr);
	if (browse)
	{
		if (&browse->data + sizeof(void*) == ptr)
		{
			if (browse->next && browse->past)
			{
				browse->next->past = browse->past;
				browse->past->next = browse->next;
			}
			else if (browse->next == NULL && browse->past)
				browse->past->next = NULL;
			else
				g_alc_mng.custom_plage = browse->next;
			munmap(browse, browse->size);
			return (true);
		}
	}
	return (false);
}

static bool		free2(t_retplgmlc *data)
{
	if (data->plage->next && data->plage->past)
	{
		data->plage->next->past = data->plage->past;
		data->plage->past->next = data->plage->next;
	}
	else if (data->plage->past)
		data->plage->past->next = NULL;
	else if (data->plage->next)
	{
		if (data->plage == g_alc_mng.small_plage)
			g_alc_mng.small_plage = data->plage->next;
		else if (data->plage == g_alc_mng.med_plage)
			g_alc_mng.med_plage = data->plage->next;
	}
	else
	{
		data->plage->data = NULL;
		return (true);
	}
	munmap(data->plage, data->plage->size);
	return (false);
}

void			free(void *ptr)
{
	t_retplgmlc	data;

	if (freecustomsizeptr(ptr))
		return ;
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return ;
	if (data.mlc->past == NULL && data.mlc->next == NULL)
	{
		if (free2(&data))
			return ;
	}
	else if (data.mlc->next && data.mlc->past == NULL)
	{
		data.mlc->next->past = NULL;
		data.plage->data = data.mlc->next;
	}
	else if (data.mlc->past && data.mlc->next == NULL)
		data.mlc->past->next = NULL;
	else
	{
		data.mlc->past->next = data.mlc->next;
		data.mlc->next->past = data.mlc->past;
		data.mlc->next = NULL;
	}
}
