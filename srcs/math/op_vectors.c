/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op_vectors.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arturo <arturo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 19:16:28 by arturo            #+#    #+#             */
/*   Updated: 2024/05/08 19:55:36 by arturo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

float	dot_product(t_vec vec1, t_vec vec2)
{
	float	result;
	int		i;

	result = 0;
	i = -1;
	while (++i < 3)
		result += vec1[i] * vec2[i];
	return (result);
}

void	cross_product(t_vec v1, t_vec v2, t_vec *result)
{
	(*result)[X] = (v1[Y] * v2[Z]) - (v1[Z] * v2[Y]);
	(*result)[Y] = (v1[Z] * v2[X]) - (v1[X] * v2[Z]);
	(*result)[Z] = (v1[X] * v2[Y]) - (v1[Y] * v2[X]);
	(*result)[TYPE] = VECTOR;
}

float	inv_magnitude(t_vec vec)
{
	long	i;
	float	x2;
	float	y;
	float	number;

	number = dot_product(vec, vec);
	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5F - (x2 * y * y));
	return (y);
}

void	normalize(t_vec vec, t_vec *result)
{
	float	mag;
	int		i;

	mag = inv_magnitude(vec);
	i = -1;
	while (++i < 4)
		(*result)[i] = vec[i] * mag;
}

void	negate(t_vec vec, t_vec *result)
{
	int		i;
	t_vec	zero;

	i = -1;
	while (++i < 4)
		zero[i] = 0;
	i = -1;
	while (++i < 4)
		(*result)[i] = zero[i] - vec[i];
}