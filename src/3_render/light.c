/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   light.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arturo <arturo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 12:04:22 by arturo            #+#    #+#             */
/*   Updated: 2024/05/24 10:59:53 by arturo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	calc_sph_normal(t_light *light, t_intersect *closest)
{
	t_vec	child_point;
	t_vec	child_normal;
	t_vec	zero;
	t_mtrx	back_to_parent;

	if (closest->object.is_transformed == FALSE)
		substract(light->point, closest->object.og, &light->normal);
	else
	{
		matrix_by_t_vec(closest->object.inv_trans, \
		light->point, &child_point, 4);
		substract(child_point, closest->object.og, &child_normal);
		transpose(closest->object.inv_trans, &back_to_parent, 4);
		matrix_by_t_vec(back_to_parent, child_normal, &light->normal, 4);
		light->normal[TYPE] = VECTOR;
	}
	normalize(light->normal, &light->normal);
	if (dot_product(light->normal, light->eye) < 0)
	{
		create_vector(&zero, 0, 0, 0);
		substract(zero, light->normal, &light->normal);
	}
}

void	calc_cyl_normal(t_light *light, t_intersect *closest)
{
	float	hit;
	t_vec	point;
	t_mtrx	back_to_parent;
	t_vec	temp;

	copy_t_vec(&point, light->point);
	if (closest->object.is_transformed == TRUE)
		matrix_by_t_vec(closest->object.inv_trans, light->point, &point, 4);
	hit = (point[X] * point[X]) + (point[Z] * point[Z]);
	if (hit < 1 && point[Y] >= closest->object.max - EPSILON)
		create_vector(&light->normal, 0, 1, 0);
	else if (hit < 1 && point[Y] <= closest->object.min + EPSILON)
		create_vector(&light->normal, 0, -1, 0);
	else
		create_vector(&light->normal, point[X], 0, point[Z]);
	if (closest->object.is_transformed == TRUE)
	{
		transpose(closest->object.inv_trans, &back_to_parent, 4);
		matrix_by_t_vec(back_to_parent, light->normal, &temp, 4);
		copy_t_vec(&light->normal, temp);
		light->normal[TYPE] = VECTOR;
	}
	normalize(light->normal, &temp);
	copy_t_vec(&light->normal, temp);
}

void	calc_plane_normal(t_light *light, t_intersect *closest)
{
	t_vec	temp;

	create_vector(&light->normal, 0, 1, 0);
	if (closest->object.is_transformed == TRUE)
	{
		matrix_by_t_vec(closest->object.inv_trans, light->normal, &temp, 4);
		copy_t_vec(&light->normal, temp);
		/*transpose(closest->object.inv_trans, &back_to_parent, 4);
		matrix_by_t_vec(back_to_parent, light->normal, &temp, 4);
		copy_t_vec(&light->normal, temp);
		light->normal[TYPE] = VECTOR;*/
		normalize(light->normal, &temp);
		copy_t_vec(&light->normal, temp);
	}
}

void	calc_light_vectors(t_light *light, t_ray ray, t_intersect *closest)
{
	t_vec	temp;

	scalar_mult(ray.dir, closest->dist, &temp);
	add(ray.og, temp, &light->point);
	negate(ray.dir, &light->eye);
	normalize(light->eye, &light->eye);
	if (closest->object.type == SPHERE)
		calc_sph_normal(light, closest);
	else if (closest->object.type == PLANE)
		calc_plane_normal(light, closest);
	else if (closest->object.type == CYLINDER)
		calc_cyl_normal(light, closest);
}

void	calc_light_reflection(t_vec in, t_vec normal, t_vec *result)
{
	t_vec	temp;

	scalar_mult(normal, (2 * dot_product(in, normal)), &temp);
	substract(temp, in, result);
	clear_negative_zeros_t_vec(result);
	normalize(*result, &temp);
	copy_t_vec(result, temp);
}

void	compute_final_color(t_light light, t_obj obj, t_ray *ray)
{
	t_vec	base_color;
	t_vec	temp;
	float	dot;

	blend_colors(light.color, obj.color, &base_color);
	substract(light.og, light.point, &light.dir);
	normalize(light.dir, &light.dir);
	scalar_mult(base_color, light.ambient, &ray->color);
	dot = dot_product(light.dir, light.normal);
	if (dot < 0 || light.is_shadow == TRUE)
		return ;
	scalar_mult(base_color, (light.diffuse * dot), &temp);
	add(ray->color, temp, &ray->color);
	calc_light_reflection(light.dir, light.normal, &light.reflect);
	dot = dot_product(light.reflect, light.eye);
	if (dot <= 0)
		return ;
	scalar_mult(light.color, (pow(dot, light.shine) * light.specular), &temp);
	add(ray->color, temp, &ray->color);
}
