/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pierre <pierre@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/18 13:31:01 by pierre           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <stdio.h>

static unsigned long int page_size(bool big)
{
	unsigned int i;
	if (big)
		i = MED_PAGE_SIZE;
	else
		i = TINY_PAGE_SIZE;
	return (i);
}

static void *ezmmap(unsigned long int size)
{
	return mmap(0, size, PROT, MAP, -1, 0);
}




static void init_page(t_plage *plage, size_t size)
{
	printf("New plage at : %p\n", plage);
	plage->data = NULL;
	plage->next	= NULL;
	plage->size = size;
	plage->max_allowed_alloc = (void*)plage + size;
}

t_malloc *init_malloc(void* ptr, size_t size)
{
	t_malloc *mlc;
	printf("New malloc at %p\n", ptr);
	if (ptr == NULL)
		return NULL;
	mlc = (t_malloc*)ptr;

	mlc->data = (void*)(&(mlc->data)) + 1;
	mlc->end = (void*)(&(mlc->data)) + size + 1;

	mlc->next = NULL;
	return (mlc);
}


static t_malloc* find_freespace(t_plage *plage, size_t wanted)
{
	t_malloc *curmalloc;
	t_malloc *tmp;
	if (wanted > plage->size)
	{
		printf("%s\n", "REQUESTED SIZE IS BIGGER THAN THE PLAGE, THIS SHOULD NOT HAPPEN");
		return NULL;
	}
	if (plage->data == NULL) // very first malloc of the plage
	{
		plage->data = init_malloc(&plage->data + 1, wanted - sizeof(t_malloc));
		t_malloc	*mal = (t_malloc*)(plage->data);
		mal->past = NULL;
		return (mal);
	}
	curmalloc = plage->data;
	while (curmalloc && curmalloc->next) // searching for free space between already existings mallocs
	{
		if ((size_t)((void*)curmalloc->next - curmalloc->end + 1 ) > wanted) // find the first good place, not scanning the whole plages
		{
			tmp = curmalloc->end + 1;
			init_malloc( tmp, wanted - sizeof(t_malloc));
			tmp->next = curmalloc->next;
			tmp->next->past = tmp;
			curmalloc->next = tmp;
			return (tmp);
		}
		curmalloc = curmalloc->next;
	}	

	if ((curmalloc->end + 1 + wanted) < plage->max_allowed_alloc) // if nothing between allocs then put it at the end
	{

		//printf("Free space before malloc : %lu \n", plage->max_allowed_alloc - (curmalloc->end + 1));

		tmp = curmalloc->end + 1;		
		curmalloc->next = tmp;
		tmp->past = curmalloc;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		//printf("free space after malloc : %lu at %p \n", plage->max_allowed_alloc - tmp->end, tmp->end);
		return (tmp);
	}
	//printf("last alloc end is at %p, free space before end is : %lu while we need space : %lu\n",curmalloc->end , plage->max_allowed_alloc - curmalloc->end, wanted );
	return (NULL);
}

static t_malloc *find_free_space_plages(t_plage *plage, size_t wanted)
{
	bool		found;
	t_malloc	*target;
	t_plage 	*plagebrowse;

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
			found = true;
			break;
		}
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	if (!found)
	{
		plagebrowse->next = ezmmap(plagebrowse->size);
		init_page(plagebrowse->next, plagebrowse->size);
		plagebrowse->next->data = init_malloc((void*)(&plagebrowse->next->data) + sizeof(char), wanted - sizeof(t_malloc));
		return (plagebrowse->next->data); // plage data, not malloc data
	}
	else
	{
		return (target);
	}
}

void printmalloc(t_malloc *mal)
{

	printf("Malloc pos : %p\nnext is %p past is : %p\nstart is %p end is %p\n", mal, mal->next, mal->past, mal->data, mal->end);
}

t_malloc *find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage *plagebrowse;

	if (plage == NULL || !plage->data || ptr == NULL)
		return (NULL);
	plagebrowse = plage;
	t_malloc *mal;
	while (plagebrowse)
	{
		if (ptr > (void*)plagebrowse->data && ptr < plagebrowse->max_allowed_alloc)
		{
			printf("%s %p and %p ends %p\n", "found plage cuz malloc data is at ", ptr, (void*)plagebrowse->data, plagebrowse->max_allowed_alloc );
			mal = plagebrowse->data;
			while (mal)
			{
				if (mal->data == ptr)
					return (mal);
				mal = mal->next;
			}
			printf("%s\n", "wtf didnt find");
			return (NULL);
		}
		else if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	return (NULL);
}

t_retplgmlc find_mallocandplage(void *ptr)
{
	t_retplgmlc ret;
	t_malloc *malfind;

	ret.plage = NULL;
	ret.mlc = NULL;
	if (ptr == NULL)
		return (ret);
	malfind = find_malloc_in(ptr, alc_mng.small_plage);
	if (malfind)
	{
		ret.plage = alc_mng.small_plage;
		ret.mlc = malfind;
		return (ret);
	}
	malfind = find_malloc_in(ptr, alc_mng.med_plage);
	if (malfind)
	{
		ret.plage = alc_mng.med_plage;
		ret.mlc = malfind;
		return (ret);
	}
	return (ret);
}


t_plage *checkpage(size_t size)
{
	if (size <= MAX_TINY_SIZE)
	{
		if (alc_mng.small_plage == NULL)
		{
			alc_mng.small_plage = (t_plage*)ezmmap(page_size(false));
			init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
		}
		return (alc_mng.small_plage);
	}
	else if (size <= MAX_MED_SIZE)
	{
		if (alc_mng.med_plage == NULL)
		{
			alc_mng.med_plage = (t_plage*)ezmmap(page_size(true));
			init_page(alc_mng.med_plage, MED_PAGE_SIZE);
		}
		return (alc_mng.med_plage);
	}
	return (NULL);
}


void *_malloc(size_t size)
{
	t_plage		*target;
	t_malloc 	*adr;

	if (size == 0)
		return (NULL);
	checkpage(size);
	target = checkpage(size);
	adr = find_free_space_plages(target, size + sizeof(t_malloc));
	if (adr != NULL)
		return (adr->data);
	return (NULL);	
}

void _free(void *ptr)
{
	t_retplgmlc data;

	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
	{
		printf("%s\n", "coudln't find ptr");
		return;
	}
	if (data.mlc->past == NULL && data.mlc->next == NULL)
		data.plage->data = NULL; // todo : free plage if empty
	else if (data.mlc->past == NULL)
	{
		data.mlc->next->past = NULL;
		data.plage->data = data.mlc->next;
	}
	else if (data.mlc->next == NULL)
		data.mlc->past->next = NULL;
	else
	{
		data.mlc->past->next = data.mlc->next;
		data.mlc->next->past = data.mlc->past;
		data.mlc->next = NULL;
	}
}


static void	*ft_memcpy(void *s1, const void *s2, size_t n)
{
	char	*c1;
	char	*c2;

	printf("%p %p\n", s1, s2);
	if (n == 0 || s1 == s2)
		return (s1);
	c1 = (char *)s1;
	c2 = (char *)s2;
	while (--n)
		*c1++ = *c2++;
	*c1 = *c2;
	return (s1);
}

size_t mathmin(size_t a, size_t b)
{
	if (a < b)
		return (a);
	return (b);
}

void *_realloc(void *ptr, size_t size)
{
	t_retplgmlc data;
	t_malloc *newmlc;
	t_plage *correctplage;

	correctplage = checkpage(size);
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return (NULL);
	if (data.plage == correctplage)
	{
		if (data.mlc->next != NULL)
		{
			if (((void*)data.mlc->next - sizeof(int) - ptr) >= (long)size) // place after it
			{
				data.mlc->end = data.mlc->data + size;
				return (ptr); 
			}
			else // no freespace after it so we just realoc it
			{
				newmlc = find_free_space_plages(data.plage, size);
				ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - ptr, size));
				//newmlc->end = newmlc->data + size;
				data.mlc->past->next = newmlc;
				data.mlc->next->past = newmlc;
				return (newmlc->data);
			}
		}
		else// nothing after it
		{
			if (data.plage->max_allowed_alloc > (ptr + size))
			{
				printf("Free space : %lu\n",data.plage->max_allowed_alloc -  (ptr + size) );

				data.mlc->end = data.mlc->data + size;
				return (ptr); 
			}
			else // no free space at the end
			{
				newmlc = find_free_space_plages(data.plage, size);
				ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - ptr, size));
				//newmlc->end = newmlc->data + size;
				data.mlc->past->next = newmlc;
				//printf("new realloc at %p\n", newmlc);
				return (newmlc->data);
			}
		}
	}
	else
	{

		newmlc = find_free_space_plages(correctplage, size);
		ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - ptr, size));
		//newmlc->end = newmlc->data + size;
		if (data.mlc->next && data.mlc->past)
		{
			data.mlc->past->next = data.mlc->next;
			data.mlc->next->past = data.mlc->past;
		}
		if (data.mlc->past == NULL && data.mlc->next) // check la distance entre le start de la plage et le 1er malloc pour pas que y'ai un offset vide
		{
			data.plage->data = data.mlc->next; // temp fix
		}
		if (data.mlc->past && data.mlc->next == NULL)
		{
			data.mlc->past->next = NULL;
		}
		return (newmlc->data);

	}
}


// malloc ok (missing CUSTOM SIZE)
// free ok (missing free page)
// realloc
// missing diff size category so new plage

int main(void)
{



	void *dada;

	int i = 0;

	while (i < 1)
	{
		//printf("%i\n", i);
		dada = _malloc(120);
		i++;
	}
	
	_realloc(dada, 32);
	

	return 0;
}
