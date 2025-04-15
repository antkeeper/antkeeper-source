// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.camera;
import engine.math.functions;
import engine.math.vector;
import engine.math.quaternion;
import engine.math.matrix;
import engine.math.projection;
import engine.utility.sized_types;

using namespace engine::math;

namespace engine::scene
{
	geom::ray<float, 3> camera::pick(const fvec2& ndc) const
	{
		const auto near = m_inv_view_projection * fvec4{ndc[0], ndc[1], 1.0f, 1.0f};
		const auto origin = fvec3(near) / near[3];
		const auto direction = normalize(origin - get_translation());

		return {origin, direction};
	}

	fvec3 camera::project(const fvec3& object, const fvec4& viewport) const
	{
		fvec4 result = m_view_projection * fvec4{object[0], object[1], object[2], 1.0f};
		result /= result[3];

		result.x() = result.x() * viewport[2] + viewport[0];
		result.y() = result.y() * viewport[3] + viewport[1];

		return fvec3(result);
	}

	fvec3 camera::unproject(const fvec3& window, const fvec4& viewport) const
	{
		fvec4 result;
		result[0] = ((window[0] - viewport[0]) / viewport[2]) * 2.0f - 1.0f;
		result[1] = ((window[1] - viewport[1]) / viewport[3]) * 2.0f - 1.0f;
		//result[2] = window[2] * 2.0f - 1.0f; z: [-1, 1]
		//result[2] = window[2]; // z: [0, 1]
		result[2] = 1.0f - window[2]; // z: [1, 0]
		result[3] = 1.0f;

		result = m_inv_view_projection * result;

		return fvec3(result) * (1.0f / result[3]);
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
		if (m_clip_far == inf<float>)
		{
			std::tie(m_projection, m_inv_projection) = inf_perspective_half_z_reverse_inv(m_vertical_fov, m_aspect_ratio, m_clip_near);
		}
		else
		{
			std::tie(m_projection, m_inv_projection) = perspective_half_z_inv(m_vertical_fov, m_aspect_ratio, m_clip_far, m_clip_near);
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
		std::tie(m_projection, m_inv_projection) = ortho_half_z_inv(m_clip_left, m_clip_right, m_clip_bottom, m_clip_top, m_clip_far, m_clip_near);

		// Update view-projection matrix and its inverse
		m_view_projection = m_projection * m_view;
		m_inv_view_projection = m_inv_view * m_inv_projection;

		// Update view frustum
		update_frustum();
	}

	void camera::set_exposure_value(float ev100)
	{
		m_exposure_value = ev100;
		// m_exposure_normalization = 1.0f / (exp2(m_exposure_value) * 1.2f);
		m_exposure_normalization = 1.0f / (exp2(m_exposure_value));
	}

	void camera::transformed()
	{
		// Update basis vectors
		m_forward = get_rotation() * fvec3{0.0f, 0.0f, -1.0f};
		m_up = get_rotation() * fvec3{0.0f, 1.0f, 0.0f};

		// Recalculate view matrix and its inverse
		std::tie(m_view, m_inv_view) = look_at_rh_inv(get_translation(), get_translation() + m_forward, m_up);

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
		constexpr fvec4 clip_space_cube[8] =
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
		m_bounds = {inf<fvec3>, -inf<fvec3>};
		for (usize i = 0; i < 8; ++i)
		{
			const fvec4 frustum_corner = m_inv_view_projection * clip_space_cube[i];
			m_bounds.extend(fvec3(frustum_corner) / frustum_corner[3]);
		}
	}
}
