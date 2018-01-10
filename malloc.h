/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <pfichepo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/14 10:49:33 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/08 13:05:38 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <stdlib.h>
# include <unistd.h>
# include <sys/mman.h>
# include <errno.h>
# include <stdbool.h>

# define MAX_TINY_SIZE 	32
# define TINY_PAGE_SIZE (128 * getpagesize())
# define MAX_MED_SIZE 	128
# define MED_PAGE_SIZE 	(1024 * getpagesize())
# define PROT			PROT_READ | PROT_WRITE
# define MAP			MAP_ANON | MAP_PRIVATE

typedef struct			s_malloc
{
	struct s_malloc		*past;
	struct s_malloc		*next;
	void 				*end;
	void 				*data;
}						t_malloc;



typedef struct			s_plage
{
	size_t				size;
	void				*max_allowed_alloc;
	struct s_plage		*next;
	t_malloc			*data;
}						t_plage;

typedef struct			s_cplage
{
	size_t				size;
	struct s_cplage		*next;
	void				*data;
}						t_cplage;



typedef struct 			s_retplgmlc
{
	t_plage 			*plage;
	t_malloc 			*mlc;	
}						t_retplgmlc;


typedef struct			s_plage_mng
{
	t_plage				*small_plage;
	t_plage				*med_plage;
}	t_plage_mng;

t_plage_mng				alc_mng;

#endif