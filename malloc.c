/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/08 10:52:31 by pfichepo         ###   ########.fr       */
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
	plage->max_allowed_alloc = (void*)plage + (uintptr_t)size;
	printf("%lu size is %lu\n",(void*)plage->max_allowed_alloc - (void*)plage , size);
}


t_malloc *init_malloc(void* ptr, size_t size)
{
	if (ptr == NULL)
		return NULL;

	t_malloc *mlc;

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
		printf("%s\n", "Creating malloc at the start of the place");
		plage->data = init_malloc(&plage->data+1, wanted - sizeof(t_malloc));
		t_malloc	*mal = (t_malloc*)(plage->data);
		printf("First malloc of plage %p with data at %p | data next is %p\n",plage, plage->data, mal->next);
		return (plage->data);
	}
	curmalloc = plage->data;
	while (curmalloc && curmalloc->next) // searching for free space between already existings mallocs
	{
		if ((size_t)(curmalloc->next - curmalloc->end+1 ) > wanted) // find the first good place, not scanning the whole plages
		{
			printf("Got %p that ends at %p and next starts at %p\n", curmalloc, curmalloc->end, curmalloc->next);


			tmp = curmalloc->end + 1;
			init_malloc( tmp, wanted - sizeof(t_malloc));

			tmp->next = curmalloc->next;
			curmalloc->next = tmp;
			printf("Found free space in the plage %p at %p\n free space is %lu bits and we need only %lu\n",(void*)plage, tmp, (size_t)(curmalloc->next - curmalloc->end+1 ), wanted);
			return (tmp);
		}
		curmalloc = curmalloc->next;
	}	

	if ((curmalloc->end + 1 + wanted) <= plage->max_allowed_alloc) // if nothing between allocs then put it at the end
	{
		tmp = curmalloc->end + 1;
		printf("Max allowed malloc %p\n",plage->max_allowed_alloc);
		printf("Found free space at the end of the plage %p at %p free space will be : %lu \n",(void*)plage, curmalloc->end + 1, (size_t)plage->max_allowed_alloc - ((size_t)tmp + (size_t)wanted));
		
		curmalloc->next = tmp;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		return (tmp);
	}
	printf("Found no free space in plage %p\n", (void*)plage);
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
		init_malloc(&plagebrowse->next->data+1, wanted - sizeof(t_malloc));
		return (plagebrowse->next->data);
	}
	else
		return (target);
}



void free(void *ptr)
{
	if (ptr == NULL)
		return;

}


void *_malloc(size_t size)
{
	t_plage		*target;
	t_malloc 	*adr;
	if (size == 0)
		return (NULL);
	if (size < MAX_TINY_SIZE)
	{
		printf("%s\n", "Using Small page");
		target = alc_mng.small_plage;
	}
	else if (size < MAX_MED_SIZE)
	{
		printf("%s\n", "Using Med page");
		target = alc_mng.med_plage;
	}
	else
		target = NULL;
	adr = find_free_space_plages(target, size + sizeof(t_malloc));
	if (adr != NULL)
	{
		return (adr);
	}
	return (NULL);	
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
		printf("Started searching for ptr %p in plage %p \n",ptr, plage);
		mal = plagebrowse->data;
		while (mal)
		{
			if (mal->data == ptr)
			{
				printf("dFound ptr %p in plage %p in malloc %p\n",ptr, plage, mal );
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
	t_malloc *malfind;

	if (ptr == NULL)
		return (NULL); 
	
	malfind = find_malloc_in(ptr,alc_mng.small_plage);
	if (malfind)
		return (malfind);
	malfind = find_malloc_in(ptr,alc_mng.med_plage);
	if (malfind)
		return (malfind);


return NULL;
}



int main(void)
{
	void *page = ezmmap(page_size(false) + page_size(true));
	alc_mng.small_plage = (t_plage*)page;
	init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
	alc_mng.med_plage = (t_plage*)(page + page_size(false));
	init_page(alc_mng.med_plage, MED_PAGE_SIZE);
	//printf("Page med %p size is %lu last possible mal is %p data pos is %p \n",alc_mng.med_plage, alc_mng.med_plage->size,  alc_mng.med_plage->max_allowed_alloc, &alc_mng.med_plage->data );

	int i = 0;
	while (i < 700 )
	{
		_malloc(15);
		i++;
	}


	


	/*void *bite = _malloc(5);

	printf("Malloc address of %p is %p \n",bite, find_malloc_in(bite, alc_mng.small_plage) );*/

	return 0;
}


