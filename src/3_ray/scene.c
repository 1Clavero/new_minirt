
#include "header.h"

void	add_obj_to_list(t_obj obj, t_objlist **list)
{
	t_objlist	*new;
	t_objlist	*prev;

	new = malloc(sizeof(t_objlist));
	if (!new)
		return ;
	new->obj = obj;
	new->next = NULL;
	new->prev = NULL;
	if (*list == NULL)
		*list = new;
	else
	{
		prev = *list;
		while (prev->next)
			prev = prev->next;
		new->prev = prev;
		prev->next = new;
	}
}

void	new_sphere(t_mlx *mlx)
{
	t_obj	sph;

	sph.type = SPHERE;
	create_tupple(&sph.og, 0, 0, 0);
	sph.r = 1;
	sph.is_transformed = FALSE;
	sph.ray = malloc(sizeof(t_ray));
	create_vector(&sph.color, 1, 1, 1);
	add_obj_to_list(sph, &mlx->obj_list);
}

void	new_ray(t_ray *ray, float pixel[2], t_mlx *mlx)
{
	t_vec	target;

	ray->hit = NULL;
	ray->closest = NULL;
	create_tupple(&ray->og, 0, 0, -5);
	target[X] = mlx->vp_min[X] + (mlx->pixel_size * pixel[X]);
	target[Y] = mlx->vp_min[Y] - (mlx->pixel_size * pixel[Y]);
	target[Z] = mlx->vp_wall;
	target[TYPE] = POINT;
	substract(target, ray->og, &ray->dir);
	copy_t_vec(&mlx->ray->target, target);
}

void	init_viewport(t_mlx *mlx)
{
	mlx->vp_size[X] = 80;
	mlx->vp_size[Y] = 80;
	mlx->vp_min[X] = -mlx->vp_size[X] / 2;
	mlx->vp_min[Y] = mlx->vp_size[Y] / 2;
	mlx->vp_wall = 10;
	mlx->win_size[X] = 800;
	mlx->win_size[Y] = 800;
	mlx->pixel_size = mlx->vp_size[X] / mlx->win_size[X];
}

void	free_intersections(t_ray *ray)
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

void	new_light(t_light *light)
{
	create_tupple(&light->og, -10, 10, 0);
	create_vector(&light->color, 1, 1, 1);
	light->ambient = 0.1;
	light->specular = 0.9;
	light->diffuse = 0.9;
	light->shine = 200;
}

void	init_scene(t_mlx *mlx)
{
	t_mtrx		mt[MAX_TRANSF];
	t_objlist	*obj;
	float		pixel[2];
	t_light		light;

	new_sphere(mlx);
	/*
	translation(&mt[0], 5, 0, 0);
	print_matrix(mt[0], 4);
	transform_object(mt, 1, &mlx->obj_list->obj);*/
	new_light(&light);
	init_viewport(mlx);
	pixel[X] = -1;
	while (++pixel[X] < mlx->win_size[X])
	{
		pixel[Y] = -1;
		while (++pixel[Y] < mlx->win_size[Y])
		{
			new_ray(mlx->ray, pixel, mlx);
			obj = mlx->obj_list;
			while (obj)
			{
				transform_ray(mlx->ray, obj->obj.ray, obj->obj);
				intersects_sphere(mlx->ray, obj->obj.ray, obj->obj);
				obj = obj->next;
				print_intersections(mlx->ray);
				return ;
			}
			if (mlx->ray->closest)
			{
				calc_light_vectors(&light, *(mlx->ray->closest->object.ray), 
				mlx->ray->closest);
				compute_final_color(light, mlx->ray->closest->object, 
				mlx->ray->closest->object.ray);
				create_tupple(&mlx->ray->closest->object.ray->color, 1, 1, 1);
				ft_mlx_pixel_put(&mlx->image, (int)pixel[X], (int)pixel[Y], \
				mlx->ray->closest->object.ray->color);
			}
			free_intersections(mlx->ray);
		}
	}
}
