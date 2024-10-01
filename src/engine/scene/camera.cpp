// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/camera.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>

namespace scene {

geom::ray<float, 3> camera::pick(const math::fvec2& ndc) const
{
	const auto near = m_inv_view_projection * math::fvec4{ndc[0], ndc[1], 1.0f, 1.0f};
	const auto origin = math::fvec3(near) / near[3];
	const auto direction = math::normalize(origin - get_translation());
	
	return {origin, direction};
}

math::fvec3 camera::project(const math::fvec3& object, const math::fvec4& viewport) const
{
	math::fvec4 result = m_view_projection * math::fvec4{object[0], object[1], object[2], 1.0f};
	result /= result[3];
	
	result.x() = result.x() * viewport[2] + viewport[0];
	result.y() = result.y() * viewport[3] + viewport[1];
	
	return math::fvec3(result);
}

math::fvec3 camera::unproject(const math::fvec3& window, const math::fvec4& viewport) const
{
	math::fvec4 result;
	result[0] = ((window[0] - viewport[0]) / viewport[2]) * 2.0f - 1.0f;
	result[1] = ((window[1] - viewport[1]) / viewport[3]) * 2.0f - 1.0f;
	//result[2] = window[2] * 2.0f - 1.0f; z: [-1, 1]
	//result[2] = window[2]; // z: [0, 1]
	result[2] = 1.0f - window[2]; // z: [1, 0]
	result[3] = 1.0f;
	
	result = m_inv_view_projection * result;

	return math::fvec3(result) * (1.0f / result[3]);
}

void camera::set_perspective(float vertical_fov, float aspect_ratio, float near, float far)
{
	// Set projection mode to perspective
	m_orthographic = false;
	
	// Update perspective projection parameters
	m_vertical_fov = vertical_fov;
	m_aspect_ratio = aspect_ratio;
	m_clip_near = near;
	m_clip_far = far;
	
	// Recalculate projection matrix (reversed depth) and its inverse
	if (m_clip_far == std::numeric_limits<float>::infinity())
	{
		std::tie(m_projection, m_inv_projection) = math::inf_perspective_half_z_reverse_inv(m_vertical_fov, m_aspect_ratio, m_clip_near);
	}
	else
	{
		std::tie(m_projection, m_inv_projection) = math::perspective_half_z_inv(m_vertical_fov, m_aspect_ratio, m_clip_far, m_clip_near);
	}
	
	// Recalculate view-projection matrix
	m_view_projection = m_projection * m_view;
	m_inv_view_projection = m_inv_view * m_inv_projection;
	
	// Recalculate view frustum
	update_frustum();
}

void camera::set_vertical_fov(float vertical_fov)
{
	if (!m_orthographic)
	{
		set_perspective(vertical_fov, m_aspect_ratio, m_clip_near, m_clip_far);
	}
}

void camera::set_aspect_ratio(float aspect_ratio)
{
	if (!m_orthographic)
	{
		set_perspective(m_vertical_fov, aspect_ratio, m_clip_near, m_clip_far);
	}
}

void camera::set_orthographic(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far)
{
	// Set projection mode to orthographic
	m_orthographic = true;
	
	// Update signed distances to clipping planes
	m_clip_left = clip_left;
	m_clip_right = clip_right;
	m_clip_bottom = clip_bottom;
	m_clip_top = clip_top;
	m_clip_near = clip_near;
	m_clip_far = clip_far;
	
	// Recalculate projection matrix (reversed depth) and its inverse
	std::tie(m_projection, m_inv_projection) = math::ortho_half_z_inv(m_clip_left, m_clip_right, m_clip_bottom, m_clip_top, m_clip_far, m_clip_near);
	
	// Update view-projection matrix and its inverse
	m_view_projection = m_projection * m_view;
	m_inv_view_projection = m_inv_view * m_inv_projection;
	
	// Update view frustum
	update_frustum();
}

void camera::set_exposure_value(float ev100)
{
	m_exposure_value = ev100;
	// m_exposure_normalization = 1.0f / (std::exp2(m_exposure_value) * 1.2f);
	m_exposure_normalization = 1.0f / (std::exp2(m_exposure_value));
}

void camera::transformed()
{
	// Update basis vectors
	m_forward = get_rotation() * math::fvec3{0.0f, 0.0f, -1.0f};
	m_up = get_rotation() * math::fvec3{0.0f, 1.0f, 0.0f};
	
	// Recalculate view matrix and its inverse
	std::tie(m_view, m_inv_view) = math::look_at_rh_inv(get_translation(), get_translation() + m_forward, m_up);
	
	// Update view-projection matrix and its inverse
	m_view_projection = m_projection * m_view;
	m_inv_view_projection = m_inv_view * m_inv_projection;
	
	// Update view frustum
	update_frustum();
}

void camera::update_frustum()
{
	// Recalculate view frustum
	m_view_frustum.extract(m_view_projection);
	
	// Reverse half z clip-space coordinates of a cube
	constexpr math::fvec4 clip_space_cube[8] =
	{
		{-1, -1, 1, 1}, // NBL
		{ 1, -1, 1, 1}, // NBR
		{-1,  1, 1, 1}, // NTL
		{ 1,  1, 1, 1}, // NTR
		{-1, -1, 0, 1}, // FBL
		{ 1, -1, 0, 1}, // FBR
		{-1,  1, 0, 1}, // FTL
		{ 1,  1, 0, 1}  // FTR
	};
	
	// Update bounds
	m_bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
	for (std::size_t i = 0; i < 8; ++i)
	{
		const math::fvec4 frustum_corner = m_inv_view_projection * clip_space_cube[i];
		m_bounds.extend(math::fvec3(frustum_corner) / frustum_corner[3]);
	}
}

} // namespace scene
