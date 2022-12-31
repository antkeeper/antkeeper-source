/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include "scene/camera.hpp"
#include "config.hpp"
#include "math/constants.hpp"
#include "math/interpolation.hpp"
#include "math/quaternion.hpp"
#include "math/projection.hpp"

namespace scene {

static float4x4 interpolate_view(const camera* camera, const float4x4& x, const float4x4& y, float a)
{
	math::transform<float> transform = camera->get_transform_tween().interpolate(a);
	float3 forward = transform.rotation * config::global_forward;
	float3 up = transform.rotation * config::global_up;
	return math::look_at(transform.translation, transform.translation + forward, up);
}

static float4x4 interpolate_projection(const camera* camera, const float4x4& x, const float4x4& y, float a)
{
	if (camera->is_orthographic())
	{
		return math::ortho(
			camera->get_clip_left_tween().interpolate(a),
			camera->get_clip_right_tween().interpolate(a),
			camera->get_clip_bottom_tween().interpolate(a),
			camera->get_clip_top_tween().interpolate(a),
			camera->get_clip_far_tween().interpolate(a),
			camera->get_clip_near_tween().interpolate(a));
	}
	else
	{
		return math::perspective(
			camera->get_fov_tween().interpolate(a),
			camera->get_aspect_ratio_tween().interpolate(a),
			camera->get_clip_far_tween().interpolate(a),
			camera->get_clip_near_tween().interpolate(a));
	}
}

static float4x4 interpolate_view_projection(const camera* camera, const float4x4& x, const float4x4& y, float a)
{
	return camera->get_projection_tween().interpolate(a) * camera->get_view_tween().interpolate(a);
}

camera::camera():
	compositor(nullptr),
	composite_index(0),
	orthographic(true),
	clip_left(-1.0f, math::lerp<float, float>),
	clip_right(1.0f, math::lerp<float, float>),
	clip_bottom(-1.0f, math::lerp<float, float>),
	clip_top(1.0f, math::lerp<float, float>),
	clip_near(-1.0f, math::lerp<float, float>),
	clip_far(1.0f, math::lerp<float, float>),
	fov(math::half_pi<float>, math::lerp<float, float>),
	aspect_ratio(1.0f, math::lerp<float, float>),
	view(math::matrix4<float>::identity(), std::bind(&interpolate_view, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
	projection(math::matrix4<float>::identity(), std::bind(&interpolate_projection, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
	view_projection(math::matrix4<float>::identity(), std::bind(&interpolate_view_projection, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
	exposure(0.0f, math::lerp<float, float>)
{}

geom::primitive::ray<float, 3> camera::pick(const float2& ndc) const
{
	const float4x4 inverse_view_projection = math::inverse(view_projection[1]);
	
	const float4 near = inverse_view_projection * float4{ndc[0], ndc[1], 1.0f, 1.0f};
	const float4 far = inverse_view_projection * float4{ndc[0], ndc[1], 0.0f, 1.0f};
	
	const float3 origin = float3{near[0], near[1], near[2]} / near[3];
	const float3 direction = math::normalize(float3{far[0], far[1], far[2]} / far[3] - origin);
	
	return {origin, direction};
}

float3 camera::project(const float3& object, const float4& viewport) const
{
	float4 result = view_projection[1] * float4{object[0], object[1], object[2], 1.0f};
	result[0] = (result[0] / result[3]) * 0.5f + 0.5f;
	result[1] = (result[1] / result[3]) * 0.5f + 0.5f;
	result[2] = (result[2] / result[3]) * 0.5f + 0.5f;
	
	result[0] = result[0] * viewport[2] + viewport[0];
	result[1] = result[1] * viewport[3] + viewport[1];
	
	return math::vector<float, 3>(result);
}

float3 camera::unproject(const float3& window, const float4& viewport) const
{
	float4 result;
	result[0] = ((window[0] - viewport[0]) / viewport[2]) * 2.0f - 1.0f;
	result[1] = ((window[1] - viewport[1]) / viewport[3]) * 2.0f - 1.0f;
	//result[2] = window[2] * 2.0f - 1.0f; z: [-1, 1]
	//result[2] = window[2]; // z: [0, 1]
	result[2] = 1.0f - window[2]; // z: [1, 0]
	result[3] = 1.0f;
	
	result = math::inverse(view_projection[1]) * result;

	return math::vector<float, 3>(result) * (1.0f / result[3]);
}

void camera::set_perspective(float fov, float aspect_ratio, float clip_near, float clip_far)
{
	orthographic = false;

	this->fov[1] = fov;
	this->aspect_ratio[1] = aspect_ratio;
	this->clip_near[1] = clip_near;
	this->clip_far[1] = clip_far;

	projection[1] = math::perspective_half_z(fov, aspect_ratio, clip_far, clip_near);
	
	// Recalculate view-projection matrix
	view_projection[1] = projection[1] * view[1];
	
	// Recalculate view frustum
	/// @TODO: this is a hack to fix the half z projection matrix view frustum
	view_frustum.set_matrix(math::perspective(this->fov[1], this->aspect_ratio[1], this->clip_near[1], this->clip_far[1]) * view[1]);
}

void camera::set_orthographic(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far)
{
	orthographic = true;

	this->clip_left[1] = clip_left;
	this->clip_right[1] = clip_right;
	this->clip_bottom[1] = clip_bottom;
	this->clip_top[1] = clip_top;
	this->clip_near[1] = clip_near;
	this->clip_far[1] = clip_far;

	projection[1] = math::ortho_half_z(clip_left, clip_right, clip_bottom, clip_top, clip_far, clip_near);
	
	// Recalculate view-projection matrix
	view_projection[1] = projection[1] * view[1];
	
	// Recalculate view frustum
	view_frustum.set_matrix(view_projection[1]);
}

void camera::set_exposure(float ev100)
{
	exposure[1] = ev100;
}

void camera::set_compositor(render::compositor* compositor)
{
	this->compositor = compositor;
}

void camera::set_composite_index(int index)
{
	composite_index = index;
}

void camera::update_tweens()
{
	object_base::update_tweens();
	clip_left.update();
	clip_right.update();
	clip_bottom.update();
	clip_top.update();
	clip_near.update();
	clip_far.update();
	fov.update();
	aspect_ratio.update();
	view.update();
	projection.update();
	view_projection.update();
	exposure.update();
}

void camera::transformed()
{
	// Recalculate view and view-projection matrices
	float3 forward = get_rotation() * config::global_forward;
	float3 up = get_rotation() * config::global_up;
	view[1] = math::look_at(get_translation(), get_translation() + forward, up);
	view_projection[1] = projection[1] * view[1];
	
	// Recalculate view frustum
	/// @TODO: this is a hack to fix the half z projection matrix view frustum
	if (orthographic)
		view_frustum.set_matrix(view_projection[1]);
	else
		view_frustum.set_matrix(math::perspective(fov[1], aspect_ratio[1], clip_near[1], clip_far[1]) * view[1]);
}

} // namespace scene
