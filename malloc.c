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

void _free(void *ptr);
void *_malloc(size_t size);

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

static size_t closestsize(size_t size)
{
	int i = 1;
	size_t fact = getpagesize();
	while (i * fact < size)
	{
		i++;
	}
	return (i * fact);
}

static void	*ft_memcpy(void *s1, const void *s2, size_t n)
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

size_t mathmin(size_t a, size_t b)
{
	if (a < b)
		return (a);
	return (b);
}

static void init_page(t_plage *plage, size_t size, bool custom)
{
	printf("New plage at : %p with size = %lu\n", plage, size);
	plage->data = NULL;
	plage->next	= NULL;
	plage->past = NULL;
	plage->size = size;
	plage->custom = custom;
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
		return NULL;
	if (plage->data == NULL) // very first malloc of the plage
	{
		plage->data = init_malloc(&plage->data + 1, wanted - sizeof(t_malloc));
		tmp = (t_malloc*)(plage->data);
		tmp->past = NULL;
		return (tmp);
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
		tmp = curmalloc->end + 1;		
		curmalloc->next = tmp;
		tmp->past = curmalloc;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		return (tmp);
	}
	return (NULL);
}

static t_malloc *find_free_space_plages(t_plage *plage, size_t wanted)
{
	bool		found;
	t_malloc	*target;
	t_plage 	*plagebrowse;

	if (plage == NULL)
		return (NULL);

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
		init_page(plagebrowse->next, plagebrowse->size, false);
		plagebrowse->next->past = plagebrowse;
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

t_plage *find_cmalloc_in(void *ptr)
{
	t_plage *browse;

	browse = alc_mng.custom_plage;
	while (browse)
	{
		if (&browse->data + 1 == ptr)
			return (browse);
		if (browse->next)
			browse = browse->next;
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
			init_page(alc_mng.small_plage, TINY_PAGE_SIZE, false);
		}
		return (alc_mng.small_plage);
	}
	else if (size <= MAX_MED_SIZE)
	{
		if (alc_mng.med_plage == NULL)
		{
			alc_mng.med_plage = (t_plage*)ezmmap(page_size(true));
			init_page(alc_mng.med_plage, MED_PAGE_SIZE, false);
		}
		return (alc_mng.med_plage);
	}
	else
	{
		if (alc_mng.custom_plage == NULL)
		{
			alc_mng.custom_plage = (t_plage*)ezmmap(closestsize(size + sizeof(t_plage) + 1));
			init_page(alc_mng.custom_plage, closestsize(size + sizeof(t_plage) + 1), true);
		}
		return (alc_mng.custom_plage);
	}
	return (NULL);
}

void *special_custom_malloc(size_t size)
{
	t_plage *plagebrowse;

	size_t plagesize;
	if (alc_mng.custom_plage == NULL) // if very first malloc on custom
	{
		alc_mng.custom_plage = (t_plage*)ezmmap(closestsize(size + sizeof(t_plage) + 1));
		init_page(alc_mng.custom_plage, closestsize(size + sizeof(t_plage) + 1), true);
		return (&alc_mng.custom_plage->data + 1);
	}
	plagesize = closestsize(size + sizeof(t_plage) + 1);
	plagebrowse = alc_mng.custom_plage;
	while (plagebrowse->next)
		plagebrowse = plagebrowse->next;
	plagebrowse->next = (t_plage*)ezmmap(plagesize);
	init_page(plagebrowse->next, plagesize, true);
	plagebrowse->next->past = plagebrowse;
	return (&plagebrowse->next->data + 1);
}


void *special_custom_realloc(void *ptr, size_t size, t_plage *incustom, bool gocustom)
{
	t_plage *page;
	t_retplgmlc mlc;
	void *mlc2;

	if (incustom && gocustom)
	{
		if (incustom->size - (sizeof(t_plage) + 1) >= size ) // already enough room in the current plage
			return (ptr);
		else
		{
			page = (t_plage*)ezmmap(closestsize(size + sizeof(t_plage) + 1));
			init_page(page, closestsize(size + sizeof(t_plage) + 1), true);
			ft_memcpy(&page->data + 1, &incustom->data + 1, mathmin(incustom->size, size));
			_free(ptr);
			return (&page->data + 1);
		}

	}
	else if(incustom) // custom plage -> pas custom plage
	{
		mlc2 = _malloc(size);
		ft_memcpy(mlc2, &incustom->data + 1, mathmin(incustom->size, size));
		_free(ptr);
		return (mlc2);
	}
	else // de data normal à custom;
	{
		mlc = find_mallocandplage(ptr);
		mlc2 = _malloc(size);
		ft_memcpy(mlc2, ptr, mathmin(mlc.mlc->end - mlc.mlc->data, size));
		return (mlc2);
	}
}

bool freecustomsizeptr(void *ptr)
{
	t_plage *browse;

	if (ptr == NULL || alc_mng.custom_plage == NULL)
		return (false);
	browse = find_cmalloc_in(ptr);
	if (browse)
	{
		if (&browse->data + 1 == ptr)
		{
			if (browse->next && browse->past)
			{
				browse->next->past = browse->past;
				browse->past->next = browse->next;
			}
			else if (browse->next == NULL && browse->past) // nothing after
				browse->past->next = NULL;
			else // very first but something after it
				alc_mng.custom_plage = browse->next;
			munmap(browse, browse->size);
			return (true);
		}
	}
	return (false);
}

void *_malloc(size_t size)
{
	t_plage		*target;
	t_malloc 	*adr;

	if (size == 0)
		return (NULL);
	target = checkpage(size);
	if (target->custom == false)
		adr = find_free_space_plages(target, size + sizeof(t_malloc));
	else
		return (special_custom_malloc(size));

	if (adr != NULL)
		return (adr->data);
	return (NULL);	
}

void _free(void *ptr)
{
	t_retplgmlc data;

	if (freecustomsizeptr(ptr)) // found it in custom plages
		return;

	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
	{
		printf("%s\n", "coudln't find ptr");
		return;
	}
	if (data.mlc->past == NULL && data.mlc->next == NULL) // only malloc of its plage
	{
		if (data.plage->next && data.plage->past) // plage after and before
		{
			data.plage->next->past = data.plage->past;
			data.plage->past->next = data.plage->next;
		}
		else if (data.plage->past) // nothing after
			data.plage->past->next = NULL;
		else if (data.plage->next)// very first but something after it
		{
			if (data.plage == alc_mng.small_plage)
				alc_mng.small_plage = data.plage->next;
			else if (data.plage == alc_mng.med_plage)
				alc_mng.med_plage = data.plage->next;
		}
		else // only plage of its category
		{
			data.plage->data = NULL;
			return;
		}
		
		munmap(data.plage, data.plage->size);
	}
	else if (data.mlc->next) // first of the plage
	{
		data.mlc->next->past = NULL;
		data.plage->data = data.mlc->next; // here is the deal, its going to create some offset
	}
	else if (data.mlc->past) // last of the plage
		data.mlc->past->next = NULL;
	else // one before and one after it
	{
		data.mlc->past->next = data.mlc->next;
		data.mlc->next->past = data.mlc->past;
		data.mlc->next = NULL;
	}
}



void *_realloc(void *ptr, size_t size)
{
	t_retplgmlc data;
	t_malloc *newmlc;
	t_plage *correctplage;

	correctplage = checkpage(size);
	if (correctplage == alc_mng.custom_plage || find_cmalloc_in(ptr))
		return (special_custom_realloc(ptr, size,find_cmalloc_in(ptr) , correctplage == alc_mng.custom_plage ));
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
				data.mlc->past->next = newmlc; // no need to free, lower level hacky free
				data.mlc->next->past = newmlc;
				return (newmlc->data);
			}
		}
		else// nothing after it
		{
			if (data.plage->max_allowed_alloc > (ptr + size))
			{
				data.mlc->end = data.mlc->data + size;
				return (ptr); 
			}
			else // no free space at the end
			{
				newmlc = find_free_space_plages(data.plage, size);
				ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - ptr, size));
				data.mlc->past->next = newmlc;
				return (newmlc->data);
			}
		}
	}
	else
	{
		newmlc = find_free_space_plages(correctplage, size);
		ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - ptr, size));
		_free(ptr);
		return (newmlc->data);
	}
}


// malloc retest required
// free retest required
// realloc retest required
// 

int main(void)
{

	void *dada;
	int i = 0;
	while (i < 500)
	{
		dada = _malloc(127);
		_free(dada);
		i++;
	}

	return 0;
}
