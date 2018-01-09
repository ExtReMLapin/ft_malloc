/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/09 09:24:58 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <stdio.h>

static unsigned long int page_size(bool big)
{
	unsigned int i;

	i = sizeof(t_plage);
	if (big)
		i += MED_PAGE_SIZE;
	else
		i += TINY_PAGE_SIZE;
	return (i);
}


static void *ezmmap(unsigned long int size)
{
	return mmap(0, size, PROT, MAP, -1, 0);
}

static void init_page(t_plage *plage, size_t size)
{
	plage->data = NULL;
	plage->next	= NULL;
	plage->size = size;
	plage->max_allowed_alloc = (void*)plage + (uintptr_t)size;
}


t_malloc *init_malloc(void* ptr, size_t size)
{
	t_malloc *mlc;

	if (ptr == NULL)
		return NULL;
	mlc = (t_malloc*)ptr;
	mlc->end = &(mlc->data) + size;
	mlc->data = &(mlc->data);
	mlc->next = NULL;

	return mlc;
}


static t_malloc* find_freespace(t_plage *plage, size_t wanted)
{
	t_malloc *curmalloc;
	t_malloc *tmp;
	if (wanted > plage->size)
		return NULL;
	if (plage->data == NULL)
	{
		plage->data = init_malloc(&plage->data + 1, wanted - sizeof(t_malloc));
		t_malloc	*mal = (t_malloc*)(plage->data);
		mal->past = NULL;
		return (mal->data);
	}
	curmalloc = plage->data;
	while (curmalloc && curmalloc->next) // searching for free space between already existings mallocs
	{
		if ((size_t)((void*)curmalloc->next - curmalloc->end + sizeof(int) ) > wanted) // find the first good place, not scanning the whole plages
		{
			tmp = curmalloc->end + 1;
			init_malloc( tmp, wanted - sizeof(t_malloc));
			tmp->next = curmalloc->next;
			tmp->next->past = tmp;
			curmalloc->next = tmp;
			return (tmp->data);
		}
		curmalloc = curmalloc->next;
	}	

	if ((curmalloc->end + 1 + wanted) <= plage->max_allowed_alloc) // if nothing between allocs then put it at the end
	{
		tmp = curmalloc->end + 1;		
		curmalloc->next = tmp;
		tmp->past = curmalloc;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		return (tmp->data);
	}
	return (NULL);
}

static t_malloc *find_free_space_plages(t_plage *plage, size_t wanted)
{
	bool	found;
	t_malloc	*target;
	t_plage *plagebrowse;

	if (plage == NULL)
	{
		printf("%s\n","REEEEEE PLAGE IS NULL, ABORTING" );
		return (NULL);
	}

	found = false;
	plagebrowse = plage;
	while (plagebrowse)
	{
		target = find_freespace(plagebrowse, wanted);
		if (target != NULL)
		{
			//printf("Found space at %p\n", target);
			found = true;
			break;
		}
		//printf("%s\n", "Next plage....");
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	if (!found)
	{
		plagebrowse->next = ezmmap(plagebrowse->size);
		init_page(plagebrowse->next, plagebrowse->size);
		init_malloc(&plagebrowse->next->data + 1, wanted - sizeof(t_malloc));
		return (plagebrowse->next->data);
	}
	else
		return (target);
}








void printmalloc(t_malloc *mal)
{
	printf("Malloc pos : %p next is %p  data is %p", mal, mal->next, mal->data);
}


t_malloc *find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage *plagebrowse;

	if (plage == NULL || ptr == NULL)
	{
		printf("%s\n","REEEEEE PLAGE IS NULL, ABORTING" );
		return (NULL);
	}
	plagebrowse = plage;
	t_malloc *mal;
	while (plagebrowse)
	{
		mal = plagebrowse->data;

		while (mal)
		{
			if (mal->data == ptr)
				return (mal);
			mal = mal->next;
		}
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	return (NULL);
}


void checkpage(void)
{
	void *page;
	if (alc_mng.small_plage == NULL || alc_mng.med_plage)
	{
		page = ezmmap(page_size(false) + page_size(true));
		alc_mng.small_plage = (t_plage*)page;
		init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
		alc_mng.med_plage = (t_plage*)(page + page_size(false));
		init_page(alc_mng.med_plage, MED_PAGE_SIZE);
	}
}


void *_malloc(size_t size)
{
	t_plage		*target;
	t_malloc 	*adr;

	if (size == 0)
		return (NULL);
	if (size < MAX_TINY_SIZE)
		target = alc_mng.small_plage;
	else if (size < MAX_MED_SIZE)
		target = alc_mng.med_plage;
	else
		target = NULL;
	adr = find_free_space_plages(target, size + sizeof(t_malloc));
	if (adr != NULL)
		return (adr);
	return (NULL);	
}


void _free(void *ptr)
{
	t_malloc *malfind;
	t_plage *plage;
	if (ptr == NULL)
		return; 
	plage = NULL;
	malfind = find_malloc_in(ptr, alc_mng.small_plage);
	if (malfind)
		plage = alc_mng.small_plage;
	if (malfind == NULL)
	{
		malfind = find_malloc_in(ptr, alc_mng.med_plage);
		if (malfind)
			plage = alc_mng.med_plage;
	}
	if (plage == NULL)
		return;
	t_malloc *mlc = malfind;
	if (mlc == NULL)
		printf("%s %p\n", "could not free", ptr);
	if (mlc->past == NULL && mlc->next == NULL)
	{
		plage->data = NULL; // todo : free plage is empty
	}
	else if (mlc->past == NULL)
	{
		mlc->next->past = NULL;
		plage->data = mlc->next;
	}
	else if (mlc->next == NULL)
	{
		mlc->past->next = NULL;
	}
	else
	{
		mlc->past->next = mlc->next;
		mlc->next->past = mlc->past;
		mlc->next = NULL;
	}
}


int main(void)
{
	printf("%p %p\n", alc_mng.small_plage, alc_mng.med_plage);


	ezmmap(page_size(false) + page_size(true));
	/*alc_mng.small_plage = (t_plage*)page;
	init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
	alc_mng.med_plage = (t_plage*)(page + page_size(false));
	init_page(alc_mng.med_plage, MED_PAGE_SIZE);*/



	return 0;
}


