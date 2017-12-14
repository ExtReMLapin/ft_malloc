/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2017/12/14 15:09:38 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <stdio.h>

//mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

	
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
}


static void* find_freespace(t_plage *plage, size_t wanted)
{
	t_malloc *curmalloc;
	if (wanted > plage->size)
		return NULL;

	if (plage->data == NULL)
		return (&plage->data);
	curmalloc = plage->data;
	while (curmalloc->next) // searching for free space between already existings mallocs
	{
		if ((size_t)(curmalloc->end+1 - curmalloc->next ) > wanted) // find the first good place, not scanning the whole plages
		{
			printf("Found free space in the plage %p at %p\n",(void*)plage, curmalloc->end + 1);
			return (curmalloc->end + 1);
		}
		curmalloc = curmalloc->next;
	}	

	if ((curmalloc->end + 1 + wanted) <= plage->max_allowed_alloc) // if nothing between allocs then put it at the end
	{
		printf("Found free space at the end of the plage %p at %p\n",(void*)plage, curmalloc->end + 1);
		return (curmalloc->end + 1);
	}
	printf("Found no free space in plage %p\n", (void*)plage);
	return (NULL);
}

static void* find_free_space_plages(t_plage *plage, size_t wanted)
{
	bool	found;
	void	*target;
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
			printf("Found space at %p\n", target);
			found = true;
			break;
		}
		printf("%s\n", "Next plage....");
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	if (!found)
	{
		printf("Found no page with free space, creating a new one with size %zu", plagebrowse->size);
		plagebrowse->next = ezmmap(plagebrowse->size);
		printf("%s", "..");
		init_page(plagebrowse->next, plagebrowse->size);
		printf("%s", "done!\n");
		return (&(plagebrowse->next->data));
	}
	else
		return (target);
}



void *init_malloc(void* ptr, size_t size)
{
	if (ptr == NULL)
		return (NULL);
	t_malloc *mlc;
	mlc = (t_malloc*)ptr;
	mlc->end = &mlc->data + size;
	return (&mlc->data);
}


void *_malloc(size_t size)
{
	t_plage	*target;
	void 	*adr;
	if (size == 0)
		return (NULL);
	if (size < MAX_TINY_SIZE)
	{
		printf("%s\n", "Using Small page");
		target = alc_mng.small_plage;
	}
	else if (size < MAX_MED_SIZE)
		target = alc_mng.med_plage;
	else
		target = NULL;
	adr = find_free_space_plages(target, size + sizeof(t_malloc));
	if (adr != NULL)
		return (init_malloc(adr, size));
	return (NULL);	
}


t_malloc *find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage *plagebrowse;
	if (plage == NULL)
	{
		printf("%s\n","REEEEEE PLAGE IS NULL, ABORTING" );
		return (NULL);
	}
	plagebrowse = plage;
	t_malloc *mal;
	
	while (plagebrowse)
	{
		printf("Started searching for ptr %p in plage %p \n",ptr, plage);
		mal = plagebrowse->data;
		while (mal)
		{
			if (mal->data == ptr)
			{
				printf("Found ptr %p in plage %p in malloc %p\n",ptr, plage, mal );
				return (mal);
			}
			mal = mal->next;
		}
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}

	return (NULL);
}


t_malloc	*find_malloc(void *ptr)
{
	if (ptr == NULL)
		return (NULL);



}

int main(void)
{
	void *page = ezmmap(page_size(false) + page_size(true));
	alc_mng.small_plage = (t_plage*)page;
	init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
	alc_mng.med_plage = (t_plage*)(page + page_size(false));
	init_page(alc_mng.med_plage, MED_PAGE_SIZE);

	printf("%p\n", &alc_mng);
	printf("%p\n", &alc_mng.small_plage);
	printf("%p\n", &alc_mng.med_plage);
	printf("%p\n", alc_mng.med_plage);
	void *bite = _malloc(5);


	return 0;
}


