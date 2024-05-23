/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arturo <arturo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 18:53:55 by arturo            #+#    #+#             */
/*   Updated: 2024/05/23 09:10:09 by arturo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	new_parent_ray(t_camera cam, t_ray *ray, float pixel[2])
{
	float	offset[2];
	t_vec	target;
	t_vec	temp;

	ray->hit = NULL;
	ray->closest = NULL;
	offset[X] = (pixel[X] + 0.5) * cam.pixel_size;
	offset[Y] = (pixel[Y] + 0.5) * cam.pixel_size;
	target[X] = -cam.half_canvas[X] + offset[X];
	target[Y] = cam.half_canvas[Y] - offset[Y];
	target[Z] = 1;
	create_tupple(&ray->og, 0, 0, 0);
	substract(target, ray->og, &ray->dir);
	if (cam.default_orient == FALSE)
	{
		matrix_by_t_vec(cam.inv_trans, ray->og, &temp, 4);
		copy_t_vec(&ray->og, temp);
		//	substract(target, ray->og, &ray->dir);
		matrix_by_t_vec(cam.inv_trans, ray->dir, &temp, 4);
		copy_t_vec(&ray->dir, temp);
	}
	normalize(ray->dir, &ray->dir);
}

void	clean_ray(t_ray *ray)
{
	t_intersect	*temp;

	if (ray->hit == NULL)
		return ;
	while (ray->hit)
	{
		temp = ray->hit->next;
		free(ray->hit);
		ray->hit = temp;
	}
}

void	intersect_all(t_ray *parent_ray, t_ray *child_ray, t_obj obj)
{
	if (obj.type == SPHERE)
		intersects_sphere(parent_ray, child_ray, obj);
}


int	find_intersection(t_ray *parent_ray, t_mlx *mlx)
{
	t_objlist	*list;
	t_ray		child_ray;

	list = mlx->obj_list;
	while (list)
	{
		transform_ray(parent_ray, &child_ray, list->obj);
		if (list->obj.type == SPHERE)
			intersects_sphere(parent_ray, &child_ray, list->obj);
		list = list->next;
		//free intersections in child ray
	}
	if (parent_ray->closest)
		return (TRUE);
	return (FALSE);
}

void	is_point_in_shadow(t_light *light, t_mlx *mlx)
{
	t_ray	shadow_ray;
	t_vec	temp;
	float	light_to_point_dist;

	light->is_shadow = FALSE;
	shadow_ray.hit = NULL;
	shadow_ray.closest = NULL;
	substract(light->og, light->point, &temp);
	light_to_point_dist = sqrtf(dot_product(temp, temp));
	normalize(temp, &shadow_ray.dir);
	copy_t_vec(&shadow_ray.og, light->point);
	if (find_intersection(&shadow_ray, mlx) == TRUE \
	&& shadow_ray.closest->dist < light_to_point_dist)
		light->is_shadow = TRUE;
}

void	get_pixel_color(t_mlx *mlx, float pixel[2])
{
	if (find_intersection(mlx->ray, mlx) == TRUE)
	{
		//RAY USED FOR LIGHT CALCS IS PARENT RAY
		calc_light_vectors(mlx->light, *(mlx->ray), \
		mlx->ray->closest, mlx->cam);
		is_point_in_shadow(mlx->light, mlx);
		compute_final_color(*(mlx->light), mlx->ray->closest->object, \
		mlx->ray);
		ft_mlx_pixel_put(&mlx->image, (int)pixel[X], (int)pixel[Y], \
		mlx->ray->color);
	}
}

void	init_scene(t_mlx *mlx)
{
	float		pixel[2];

	pixel[X] = -1;
	while (++pixel[X] < mlx->win_size[X])
	{
		pixel[Y] = -1;
		while (++pixel[Y] < mlx->win_size[Y])
		{
			//pixel[X] = mlx->win_size[X] / 2;
			//pixel[Y] = mlx->win_size[Y] / 2;
			new_parent_ray(mlx->cam, mlx->ray, pixel);
			get_pixel_color(mlx, pixel);
			clean_ray(mlx->ray);
		}
	}
}
