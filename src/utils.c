/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfichepo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 10:29:07 by pfichepo          #+#    #+#             */
/*   Updated: 2018/01/26 10:29:09 by pfichepo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <malloc.h>

void	*ezmmap(unsigned long int size)
{
	return (mmap(0, size, PROT, MAP, -1, 0));
}

size_t	closestsize(size_t size)
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

size_t	mathmin(size_t a, size_t b)
{
	if (a < b)
		return (a);
	return (b);
}
