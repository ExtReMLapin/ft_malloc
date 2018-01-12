/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pierre <pierre@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:59:14 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/12 14:04:30 by pierre           ###   ########.fr       */
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
	plage->max_allowed_alloc = (void*)plage + size * sizeof(char);
}

t_malloc *init_malloc(void* ptr, size_t size)
{
	t_malloc *mlc;
	printf("New malloc at %p\n", ptr);
	if (ptr == NULL)
		return NULL;
	mlc = (t_malloc*)ptr;
	mlc->end = &(mlc->data) + size * sizeof(char);
	mlc->data = &(mlc->data) + sizeof(char);
	mlc->next = NULL;

	return mlc;
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
		plage->data = init_malloc(&plage->data + sizeof(char), wanted - sizeof(t_malloc));
		t_malloc	*mal = (t_malloc*)(plage->data);
		mal->past = NULL;
		return (mal->data);
	}
	curmalloc = plage->data;
	while (curmalloc && curmalloc->next) // searching for free space between already existings mallocs
	{
		if ((size_t)((void*)curmalloc->next - curmalloc->end + sizeof(char) ) > wanted) // find the first good place, not scanning the whole plages
		{
			tmp = curmalloc->end + sizeof(char);
			init_malloc( tmp, wanted - sizeof(t_malloc));
			tmp->next = curmalloc->next;
			tmp->next->past = tmp;
			curmalloc->next = tmp;
			return (tmp->data);
		}
		curmalloc = curmalloc->next;
	}	

	if ((curmalloc->end + sizeof(char) + wanted) <= plage->max_allowed_alloc) // if nothing between allocs then put it at the end
	{
		tmp = curmalloc->end + sizeof(char);		
		curmalloc->next = tmp;
		tmp->past = curmalloc;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		return (tmp->data);
	}
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
		init_malloc(&plagebrowse->next->data + sizeof(int), wanted - sizeof(t_malloc));
		return (plagebrowse->next->data);
	}
	else
		return (target);
}

void printmalloc(t_malloc *mal)
{

	printf("Malloc pos : %p\nnext is %p past is : %p\nstart is %p end is %p\n", mal, mal->next, mal->past, mal->data, mal->end);
}

t_malloc *find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage *plagebrowse;

	if (plage == NULL || ptr == NULL)
		return (NULL);
	plagebrowse = plage;
	t_malloc *mal;
	while (plagebrowse)
	{
		if (ptr > (void*)plagebrowse->data && ptr < plagebrowse->max_allowed_alloc)
		{
			printf("%s\n", "found plage");
			mal = plagebrowse->data;
			while (mal)
			{
				if (mal->data == ptr)
					return (mal);
				printf("found %p but looking for %p\n", mal->data, ptr);
				mal = mal->next;
			}
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
	if (size < MAX_TINY_SIZE)
	{
		if (alc_mng.small_plage == NULL)
		{
			alc_mng.small_plage = (t_plage*)ezmmap(page_size(false));
			init_page(alc_mng.small_plage, TINY_PAGE_SIZE);
		}
		return (alc_mng.small_plage);
	}
	else if (size < MAX_MED_SIZE)
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
	t_retplgmlc data;

	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return;
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


void	*ft_memcpy(void *s1, const void *s2, size_t n)
{
	char	*c1;
	char	*c2;

	if (n == 0 || s1 == s2)
		return (s1);
	c1 = (char *)s1;
	c2 = (char *)s2;
	while (--n)
		*c1++ = *c2++;
	*c1 = *c2;
	return (s1);
}




void *_realloc(void *ptr, size_t size)
{
	t_retplgmlc data;
	t_malloc *newmlc;

	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return (NULL);
	if (data.mlc->next != NULL)
	{
		if (((void*)data.mlc->next - sizeof(int) - ptr) >= (long)size)
		{
			data.mlc->end = data.mlc->data + size * sizeof(char);
			return (ptr); 
		}
		else // no freespace after it so we just realoc it
		{
			newmlc = find_free_space_plages(data.plage, size);
			ft_memcpy(newmlc->data, ptr, data.mlc->end - ptr);
			newmlc->end = newmlc->data + size;
			data.mlc->past->next = newmlc;
			data.mlc->next->past = newmlc;
			return (newmlc->data);
		}
	}
	else
	{
		if (data.plage->max_allowed_alloc > (ptr + size * sizeof(char)))
		{
			data.mlc->end = data.mlc->data + size;
			return (ptr); 
		}
		else // no free space at the end
		{
			newmlc = find_free_space_plages(data.plage, size);
			ft_memcpy(newmlc->data, ptr, data.mlc->end - ptr);
			newmlc->end = newmlc->data + size;
			data.mlc->past->next = newmlc;
			return (newmlc->data);
		}
	}
}

int main(void)
{


	char* dada = (char*)_malloc(27*sizeof(char));
	int i = 0 ;
	while (i < 26)
	{
		dada[i] = 'a' + i;
		i++;
	}
	dada[i] = 0;


	char* dada2 = _malloc(5*sizeof(char));
	i = 0;
	while (i < 4)
	{
		dada2[i] = '1' + i;
		i++;
	}
	dada2[i] = 0;



	t_retplgmlc dada1 = find_mallocandplage((void*)dada);
	printmalloc(dada1.mlc);


	t_retplgmlc dada22 = find_mallocandplage((void*)dada2);
	printmalloc(dada22.mlc);

	printf("%s %s\n", dada, dada2 );

	return 0;
}
