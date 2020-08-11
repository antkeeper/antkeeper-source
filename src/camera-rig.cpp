/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "orbit-cam.hpp"
#include "scene/camera.hpp"
#include "math/constants.hpp"
#include "configuration.hpp"
#include <algorithm>
#include <cmath>

camera_rig::camera_rig():
	camera(nullptr),
	translation{0.0f, 0.0f, 0.0f},
	rotation(math::identity_quaternion<float>),
	forward(global_forward),
	right(global_right),
	up(global_up)
{}

void camera_rig::attach(::camera* camera)
{
	this->camera = camera;
	if (camera != nullptr)
	{
		camera->look_at(translation, translation + forward, up);
	}
}

void camera_rig::detach()
{
	camera = nullptr;
}

void camera_rig::set_translation(const float3& translation)
{
	this->translation = translation;
}

void camera_rig::set_rotation(const quaternion_type& rotation)
{
	this->rotation = rotation;
	
	// Calculate orthonormal basis
	forward = rotation * global_forward;
	up = rotation * global_up;
	right = rotation * global_right;
}
