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
#include <string.h>

void				_free(void *ptr);
void				*_malloc(size_t size);

static void			*ezmmap(unsigned long int size)
{
	return (mmap(0, size, PROT, MAP, -1, 0));
}

static size_t		closestsize(size_t size)
{
	int		i;
	size_t	fact;

	i = 1;
	fact = getpagesize();
	while (i * fact < size)
	{
		i++;
	}
	return (i * fact);
}

static void			*ft_memcpy(void *s1, const void *s2, size_t n)
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

size_t				mathmin(size_t a, size_t b)
{
	if (a < b)
		return (a);
	return (b);
}

static void			init_page(t_plage *plage, size_t size, bool custom)
{
	if (plage == NULL)
		return ;
	plage->data = NULL;
	plage->next	= NULL;
	plage->past = NULL;
	plage->size = size;
	plage->custom = custom;
	plage->max_allowed_alloc = (void*)plage + size;
}

t_malloc			*init_malloc(void* ptr, size_t size)
{
	t_malloc *mlc;
	
	if (ptr == NULL)
		return NULL;
	mlc = (t_malloc*)ptr;
	mlc->data = (void*)(&(mlc->data)) + sizeof(void*);
	mlc->end = mlc->data + size + sizeof(void*);
	mlc->next = NULL;
	return (mlc);
}


static t_malloc *find_freespacewhile(t_malloc *mlc, size_t w)
{
	t_malloc *tmp;

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

static t_malloc		*find_freespace(t_plage *plage, size_t wanted)
{
	t_malloc *curmalloc;
	t_malloc *tmp;

	if (plage == NULL || wanted > plage->size || wanted == 0)
		return NULL;
	if (plage->data == NULL)
	{
		plage->data = init_malloc(&plage->data + sizeof(void*), wanted - sizeof(t_malloc));
		tmp = (t_malloc*)(plage->data);
		tmp->past = NULL;
		return (tmp);
	}
	curmalloc = plage->data;
	tmp = find_freespacewhile( curmalloc, wanted);
	if (tmp != NULL)
		return (tmp);
	if ((curmalloc->end + sizeof(void*) + wanted) < plage->max_allowed_alloc)
	{
		tmp = curmalloc->end + sizeof(void*);		
		curmalloc->next = tmp;
		tmp->past = curmalloc;
		init_malloc(tmp, wanted - sizeof(t_malloc));
		return (tmp);
	}
	return (NULL);
}



static bool 		find_free_space_plages_bool(t_plage *plagebrowse, size_t wanted, t_malloc *target)
{
	while (plagebrowse)
	{
		target = find_freespace(plagebrowse, wanted);
		if (target != NULL)
			return (true);
		if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			return (false);
	}
	return (false);
}


static t_malloc		*find_free_space_plages(t_plage *plage, size_t wanted)
{
	bool		found;
	t_malloc	*target;
	t_plage		*plagebrowse;

	target = NULL;
	if (plage == NULL || wanted == 0)
		return (NULL);
	plagebrowse = plage;
	found = find_free_space_plages_bool(plagebrowse, wanted, target);
	if (!found)
	{
		plagebrowse->next = ezmmap(plagebrowse->size);
		init_page(plagebrowse->next, plagebrowse->size, false);
		plagebrowse->next->past = plagebrowse;
		plagebrowse->next->data = init_malloc((void*)(&plagebrowse->next->data) + sizeof(char), wanted - sizeof(t_malloc));
		return (plagebrowse->next->data);
	}
	else
		return (target);
}


void				show_alloc_mem_plg(t_plage *plg, bool custom)
{
	int 		i;
	int			i1;
	t_malloc	*mlc;
	t_plage		*plg1;

	i1 = 0;
	plg1 = plg;
	while (plg1)
	{
		i = 0;
		if (! custom)
		{
			mlc = plg1->data;
			printf("	Plage #%i with size %lu @ %p\n", ++i1, plg1->size, plg1);
			while (mlc)
			{
				printf("		alloc #%i %p [%p -> %p] %lu bytes\n",++i, mlc, mlc->data, mlc->end, mlc->end - mlc->data - sizeof(void*));
				if (mlc->next && (mlc->end + sizeof(void*) != mlc->next))
					printf("		free space of %lu bytes from %p to %p \n", (void*)mlc->next - mlc->end , mlc->end + 1, (void*)mlc->next - 1);
				mlc = mlc->next;
			}
		}
		else
			printf("	Custom plage #%i with size %lu @ %p\n",++i1, plg1->size, plg1 );
		plg1 = plg1->next;
	}
}


void				show_alloc_mem()
{
	if (g_alc_mng.small_plage && g_alc_mng.small_plage->data)
	{
		printf("TINY : %p \n", g_alc_mng.small_plage);
		show_alloc_mem_plg(g_alc_mng.small_plage, false);
	}
	if (g_alc_mng.med_plage && g_alc_mng.med_plage->data)
	{
		printf("MEDIUM : %p \n", g_alc_mng.med_plage);
		show_alloc_mem_plg(g_alc_mng.med_plage, false);
	}
	if (g_alc_mng.custom_plage)
	{
		printf("LARGE : %p \n", g_alc_mng.custom_plage);
		show_alloc_mem_plg(g_alc_mng.custom_plage, true);
	}
}

t_malloc			*find_malloc_in(void *ptr, t_plage *plage)
{
	t_plage		*plagebrowse;
	t_malloc	*mal;

	if (plage == NULL || !plage->data || ptr == NULL)
		return (NULL);
	plagebrowse = plage;
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
			return (NULL);
		}
		else if (plagebrowse->next)
			plagebrowse = plagebrowse->next;
		else
			break;
	}
	return (NULL);
}

t_plage				*find_cmalloc_in(void *ptr)
{
	t_plage	*browse;

	if (ptr == NULL)
		return (NULL);
	browse = g_alc_mng.custom_plage;
	while (browse)
	{
		if (&browse->data + sizeof(void*) == ptr)
			return (browse);
		if (browse->next)
			browse = browse->next;
		else
			break;
	}
	return (NULL);
}


t_retplgmlc			find_mallocandplage(void *ptr)
{
	t_retplgmlc	ret;
	t_malloc	*malfind;

	ret.plage = NULL;
	ret.mlc = NULL;
	if (ptr == NULL)
		return (ret);
	malfind = find_malloc_in(ptr, g_alc_mng.small_plage);
	if (malfind)
	{
		ret.plage = g_alc_mng.small_plage;
		ret.mlc = malfind;
		return (ret);
	}
	malfind = find_malloc_in(ptr, g_alc_mng.med_plage);
	if (malfind)
	{
		ret.plage = g_alc_mng.med_plage;
		ret.mlc = malfind;
		return (ret);
	}
	return (ret);
}


t_plage				*checkpage(size_t size)
{
	if (size <= MAX_TINY_SIZE)
	{
		if (g_alc_mng.small_plage == NULL)
		{
			g_alc_mng.small_plage = (t_plage*)ezmmap(TINY_PAGE_SIZE);
			init_page(g_alc_mng.small_plage, TINY_PAGE_SIZE, false);
		}
		return (g_alc_mng.small_plage);
	}
	else if (size <= MAX_MED_SIZE)
	{
		if (g_alc_mng.med_plage == NULL)
		{
			g_alc_mng.med_plage = (t_plage*)ezmmap(MED_PAGE_SIZE);
			init_page(g_alc_mng.med_plage, MED_PAGE_SIZE, false);
		}
		return (g_alc_mng.med_plage);
	}
	else
	{
		return (g_alc_mng.custom_plage);
	}
}

void				*special_custom_malloc(size_t size)
{
	t_plage	*plagebrowse;
	size_t	plagesize;

	if (g_alc_mng.custom_plage == NULL)
	{
		g_alc_mng.custom_plage = (t_plage*)ezmmap(closestsize(size + sizeof(t_plage) + sizeof(void*)));
		init_page(g_alc_mng.custom_plage, closestsize(size + sizeof(t_plage) + sizeof(void*)), true);
		g_alc_mng.custom_plage->max_allowed_alloc = (void*) &g_alc_mng.custom_plage->data + size; 
		return (&g_alc_mng.custom_plage->data + sizeof(void*));
	}
	plagesize = closestsize(size + sizeof(t_plage) + sizeof(void*));
	plagebrowse = g_alc_mng.custom_plage;
	while (plagebrowse->next)
		plagebrowse = plagebrowse->next;
	plagebrowse->next = (t_plage*)ezmmap(plagesize);
	init_page(plagebrowse->next, plagesize, true);
	plagebrowse->next->past = plagebrowse;
	plagebrowse->next->max_allowed_alloc = (void*)&plagebrowse->next->data + size; 
	return (&plagebrowse->next->data + sizeof(void*));
}

static void			*special_custom_realloc01( size_t size, t_plage *incustom, void *ptr)
{
	void			*mlc2;

	mlc2 = _malloc(size);
	ft_memcpy(mlc2, &incustom->data + sizeof(void*), mathmin(incustom->size, size));
	_free(ptr);
	return (mlc2);
}

static void			*special_custom_realloc02(void *ptr, size_t size)
{
	void			*mlc2;
	t_retplgmlc		mlc;

	mlc = find_mallocandplage(ptr);
	mlc2 = _malloc(size);
	ft_memcpy(mlc2, ptr, mathmin(mlc.mlc->end - mlc.mlc->data, size));
	return (mlc2);
}

void				*special_custom_realloc(void *ptr, size_t size, t_plage *incustom, bool gocustom)
{
	void		*page;

	if (incustom && gocustom)
	{
		if (incustom->size - (sizeof(t_plage) + sizeof(void*)) >= size )
			return (ptr);
		else
		{
			page = _malloc(size);
			init_page(page, closestsize(size + sizeof(t_plage) + sizeof(void*)), true);
			ft_memcpy(page + sizeof(void*), &incustom->data + sizeof(void*), mathmin(incustom->max_allowed_alloc - (void*)&incustom->data  , size));
			_free(ptr);
			return (page + sizeof(void*));
		}
	}
	else if(incustom)
		return (special_custom_realloc01(size, incustom, ptr));
	else
		return (special_custom_realloc02(ptr, size));
}

bool				freecustomsizeptr(void *ptr)
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

void				*_malloc(size_t size)
{
	t_plage		*target;
	t_malloc	*adr;

	if (size == 0)
		return (NULL);
	target = checkpage(size);
	if (target && target->custom == false)
		adr = find_free_space_plages(target, size + sizeof(t_malloc));
	else
		return (special_custom_malloc(size));
	if (adr != NULL)
		return (adr->data);
	return (NULL);	
}


bool 				_free2(t_retplgmlc *data)
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

void				_free(void *ptr)
{
	t_retplgmlc	data;

	if (freecustomsizeptr(ptr))
		return;
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return;
	if (data.mlc->past == NULL && data.mlc->next == NULL)
	{
		if (_free2(&data))
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

void 				*_realloc(void *ptr, size_t size)
{
	t_retplgmlc	data;
	t_malloc	*newmlc;
	t_plage		*correctplage;

	if (ptr == NULL)
		return (NULL);
	correctplage = checkpage(size);
	if (correctplage == g_alc_mng.custom_plage || find_cmalloc_in(ptr))
		return (special_custom_realloc(ptr, size,find_cmalloc_in(ptr), correctplage == g_alc_mng.custom_plage));
	data = find_mallocandplage(ptr);
	if (data.plage == NULL)
		return (NULL);
	if (data.plage == correctplage)
	{
		if (data.mlc->next != NULL)
		{
			if (((void*)data.mlc->next - sizeof(int) - ptr) >= (long)size)
			{
				data.mlc->end = data.mlc->data + size;
				return (ptr);
			}
			else
			{
				newmlc = find_free_space_plages(data.plage, size);
				ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - data.mlc->data, size));
				data.mlc->past->next = newmlc;
				data.mlc->next->past = newmlc;
				return (newmlc->data);
			}
		}
		else
		{
			if (data.plage->max_allowed_alloc > (ptr + size))
			{
				data.mlc->end = data.mlc->data + size;
				return (ptr);
			}
			else
			{
				newmlc = find_free_space_plages(data.plage, size);
				ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - data.mlc->data, size));
				data.mlc->past->next = newmlc;
				return (newmlc->data);
			}
		}
	}
	else
	{
		newmlc = find_free_space_plages(correctplage, size);
		ft_memcpy(newmlc->data, ptr, mathmin(data.mlc->end - data.mlc->data, size));
		_free(ptr);
		return (newmlc->data);
	}
}


#define M (1024 * 1024)

void print(char *s)
{
   write(1, s, strlen(s));
}

int					main(void)
{	
	char *addr1;
	char *addr3;

	addr1 = (char*)_malloc(16*M);
	strcpy(addr1, "Bonjours\n");
	print(addr1);
	addr3 = (char*)_realloc(addr1, 128*M);
	addr3[127*M] = 42;
	print(addr3);
	_malloc(1024);
	_malloc(42);
	_malloc(43);
	_malloc(1024 * 32);
	_malloc(1024 * 1024);
	_malloc(1024 * 1024 * 16);
	_malloc(1024 * 1024 * 128);
	show_alloc_mem();
	return (0);
}
