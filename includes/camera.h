/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arturo <arturo@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 11:03:44 by arturo            #+#    #+#             */
/*   Updated: 2024/05/21 17:33:28 by arturo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

typedef struct s_camera
{
	float	half_canvas[2]; // [half_width, half_height]
	float	half_window[2]; // [hsize, vsize];
	float	vp_min[2];
	float	vp_size[2];
	float	fov;
	float	pixel_size;
	bool	default_orient;
	t_vec	og;
	t_mtrx	mt_trans;
	t_mtrx	inv_trans;
	t_vec	from;
	t_vec	up;
	t_vec	to;
}		t_camera;

void	cam_transform(t_camera *cam, t_vec orientation);
void	new_ray_test(t_camera cam, t_ray *ray, float pixel[2]);
void	new_parent_ray(t_camera cam, t_ray *ray, float pixel[2]);
void	calc_pixel_size(t_camera *cam);

#endif