/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:49:33 by pfichepo          #+#    #+#             */
/*   Updated: 2018/02/05 10:02:55 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>
# include <sys/mman.h>
# include <stdbool.h>
# include <stdio.h>
# include <string.h>
# define MAX_TINY_SIZE 	64
# define MAX_MED_SIZE 	1024
# define MED_PAGE_SIZE 	(12 * getpagesize())
# define PROT			PROT_READ | PROT_WRITE
# define MAP			MAP_ANON | MAP_PRIVATE

typedef struct			s_malloc
{
	struct s_malloc		*past;
	struct s_malloc		*next;
	void				*end;
	void				*data;
}						t_malloc;

typedef struct			s_plage
{
	size_t				size;
	bool				custom;
	void				*max_allowed_alloc;
	struct s_plage		*next;
	struct s_plage		*past;
	t_malloc			*data;
}						t_plage;

typedef struct			s_retplgmlc
{
	t_plage				*plage;
	t_malloc			*mlc;
}						t_retplgmlc;

typedef struct			s_plage_mng
{
	t_plage				*small_plage;
	t_plage				*med_plage;
	t_plage				*custom_plage;
}						t_plage_mng;

t_plage_mng				g_alc_mng;

void					*reallocsub(t_retplgmlc *d, size_t s,
						void *ptr, t_plage *cp);
t_malloc				*find_free_space_plages(t_plage *plage, size_t wanted);
void					init_page(t_plage *plage, size_t size, bool custom);
t_malloc				*init_malloc(void *ptr, size_t size);
void					*ezmmap(unsigned long int size);
size_t					closestsize(size_t size);
void					*ft_memcpy(void *s1, const void *s2, size_t n);
size_t					mathmin(size_t a, size_t b);
void					*special_custom_realloc(void *p, size_t s,
						t_plage *ic, bool goc);
void					*special_custom_malloc(size_t size);
t_malloc				*find_malloc_in(void *ptr, t_plage *plage);
t_plage					*find_cmalloc_in(void *ptr);
t_retplgmlc				find_mallocandplage(void *ptr);
void					show_alloc_mem_plg(t_plage *plg, bool custom);
void					show_alloc_mem();
t_plage					*checkpage(size_t size);
void					free(void *ptr);
void					*malloc(size_t size);
void					*realloc(void *ptr, size_t size);
void					*calloc(size_t nmemb, size_t size);
#endif
