/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "animation/camera-rig.hpp"
#include "math/constants.hpp"
#include "configuration.hpp"
#include <algorithm>
#include <cmath>

camera_rig::camera_rig():
	camera(nullptr),
	transform(math::identity_transform<float>),
	forward(global_forward),
	right(global_right),
	up(global_up)
{}

void camera_rig::attach(scene::camera* camera)
{
	this->camera = camera;
	if (camera != nullptr)
	{
		camera->set_transform(transform);
	}
}

void camera_rig::detach()
{
	camera = nullptr;
}

void camera_rig::update_transform(const transform_type& transform)
{
	this->transform = transform;
	
	// Calculate orthonormal basis
	forward = transform.rotation * global_forward;
	up = transform.rotation * global_up;
	right = transform.rotation * global_right;
	
	if (camera != nullptr)
	{
		camera->set_transform(transform);
	}
}

void camera_rig::update_projection(float fov, float aspect_ratio, float clip_near, float clip_far)
{
	if (camera != nullptr)
	{
		camera->set_perspective(fov, aspect_ratio, clip_near, clip_far);
	}
}

void camera_rig::update_projection(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far)
{
	if (camera != nullptr)
	{
		camera->set_orthographic(clip_left, clip_right, clip_bottom, clip_top, clip_near, clip_far);
	}
}
