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

#ifndef ANTKEEPER_SCENE_CAMERA_HPP
#define ANTKEEPER_SCENE_CAMERA_HPP

#include <engine/scene/object.hpp>
#include <engine/geom/primitives/view-frustum.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/compositor.hpp>
#include <engine/math/numbers.hpp>

namespace scene {

/**
 *
 */
class camera: public object<camera>
{
public:
	/// Camera view frustum type.
	using view_frustum_type = geom::view_frustum<float>;
	
	/**
	 * Constructs a picking ray from normalized device coordinates (NDC).
	 *
	 * @param ndc NDC coordinates.
	 *
	 * @return Picking ray.
	 */
	[[nodiscard]] geom::ray<float, 3> pick(const math::fvec2& ndc) const;

	/**
	 * Maps object coordinates to window coordinates.
	 *
	 * @param object Object coordinates.
	 * @param viewport Vector containing the `x`, `y`, `w`, and `h` of the viewport.
	 * @return Projected window coordinates.
	 */
	[[nodiscard]] math::fvec3 project(const math::fvec3& object, const math::fvec4& viewport) const;

	/**
	 * Maps window coordinates to object coordinates.
	 *
	 * @param window Window coordinates.
	 * @param viewport Vector containing the `x`, `y`, `w`, and `h` of the viewport.
	 * @return Unprojected object coordinates.
	 */
	[[nodiscard]] math::fvec3 unproject(const math::fvec3& window, const math::fvec4& viewport) const;

	/**
	 * Sets the camera's projection matrix using perspective projection.
	 *
	 * @param vertical_fov Vertical field of view, in radians.
	 * @param aspect_ratio Aspect ratio.
	 * @param clip_near Distance to near clipping plane.
	 * @param clip_far Distance to far clipping plane.
	 */
	void set_perspective(float vertical_fov, float aspect_ratio, float clip_near, float clip_far);
	
	/**
	 * Sets the camera's vertical field of view.
	 *
	 * @param vertical_fov Vertical field of view, in radians.
	 */
	void set_vertical_fov(float vertical_fov);
	
	/**
	 * Sets the camera's projection matrix using orthographic projection.
	 *
	 * @param clip_left Signed distance to left clipping plane.
	 * @param clip_right Signed distance to right clipping plane.
	 * @param clip_bottom Signed distance to bottom clipping plane.
	 * @param clip_top Signed distance to top clipping plane.
	 * @param clip_near Signed distance to near clipping plane.
	 * @param clip_far Signed distance to far clipping plane.
	 */
	void set_orthographic(float clip_left, float clip_right, float clip_bottom, float clip_top, float clip_near, float clip_far);

	/**
	 * Sets the camera's ISO 100 exposure value.
	 *
	 * @param ev100 ISO 100 exposure value.
	 */
	void set_exposure_value(float ev100);
	
	/**
	 * Sets the camera's compositor.
	 *
	 * @param compositor Compositor.
	 */
	inline void set_compositor(render::compositor* compositor) noexcept
	{
		m_compositor = compositor;
	}
	
	/**
	 * Sets the composite index of the camera.
	 *
	 * @param index Composite index.
	 */
	inline void set_composite_index(int index) noexcept
	{
		m_composite_index = index;
	}
	
	/**
	 * Returns the camera's compositor.
	 */
	/// @{
	[[nodiscard]] inline const render::compositor* get_compositor() const noexcept
	{
		return m_compositor;
	}
	[[nodiscard]] inline render::compositor* get_compositor() noexcept
	{
		return m_compositor;
	}
	/// @}
	
	/// Returns the composite index of the camera.
	[[nodiscard]] inline int get_composite_index() const noexcept
	{
		return m_composite_index;
	}
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
	/// Returns `true` if the camera uses an orthographic projection matrix, `false` otherwise.
	[[nodiscard]] inline bool is_orthographic() const noexcept
	{
		return m_orthographic;
	}

	/// Returns the signed distance to the camera's left clipping plane.
	[[nodiscard]] inline float get_clip_left() const noexcept
	{
		return m_clip_left;
	}
	
	/// Returns the signed distance to the camera's right clipping plane.
	[[nodiscard]] inline float get_clip_right() const noexcept
	{
		return m_clip_right;
	}
	
	/// Returns the signed distance to the camera's bottom clipping plane.
	[[nodiscard]] inline float get_clip_bottom() const noexcept
	{
		return m_clip_bottom;
	}
	
	/// Returns the signed distance to the camera's top clipping plane.
	[[nodiscard]] inline float get_clip_top() const noexcept
	{
		return m_clip_top;
	}
	
	/// Returns the signed distance to the camera's near clipping plane.
	[[nodiscard]] inline float get_clip_near() const noexcept
	{
		return m_clip_near;
	}
	
	/// Returns the signed distance to the camera's far clipping plane.
	[[nodiscard]] inline float get_clip_far() const noexcept
	{
		return m_clip_far;
	}
	
	/// Returns the camera's vertical field of view, in radians.
	[[nodiscard]] inline float get_vertical_fov() const noexcept
	{
		return m_vertical_fov;
	}
	
	/// Returns the camera's aspect ratio.
	[[nodiscard]] inline float get_aspect_ratio() const noexcept
	{
		return m_aspect_ratio;
	}
	
	/// Returns the camera's ISO 100 exposure value.
	[[nodiscard]] inline float get_exposure_value() const noexcept
	{
		return m_exposure_value;
	}
	
	/// Returns the camera's exposure normalization factor.
	[[nodiscard]] inline float get_exposure_normalization() const noexcept
	{
		return m_exposure_normalization;
	}
	
	/// Returns the camera's view matrix.
	[[nodiscard]] inline const math::fmat4& get_view() const noexcept
	{
		return m_view;
	}
	
	/// Returns the camera's projection matrix.
	[[nodiscard]] inline const math::fmat4& get_projection() const noexcept
	{
		return m_projection;
	}
	
	/// Returns the camera's view-projection matrix.
	[[nodiscard]] inline const math::fmat4& get_view_projection() const noexcept
	{
		return m_view_projection;
	}
	
	/// Returns the camera's inverse view-projection matrix.
	[[nodiscard]] inline const math::fmat4& get_inverse_view_projection() const noexcept
	{
		return m_inverse_view_projection;
	}
	
	/// Returns the camera's forward vector.
	[[nodiscard]] inline const math::fvec3& get_forward() const noexcept
	{
		return m_forward;
	}
	
	/// Returns the camera's up vector.
	[[nodiscard]] inline const math::fvec3& get_up() const noexcept
	{
		return m_up;
	}
	
	/// Returns the camera's view frustum.
	[[nodiscard]] inline const view_frustum_type& get_view_frustum() const noexcept
	{
		return m_view_frustum;
	}

private:
	virtual void transformed();
	void update_frustum();

	render::compositor* m_compositor{nullptr};
	int m_composite_index{0};
	
	bool m_orthographic{true};
	
	float m_clip_left{-1.0f};
	float m_clip_right{1.0f};
	float m_clip_bottom{-1.0f};
	float m_clip_top{1.0f};
	float m_clip_near{-1.0f};
	float m_clip_far{1.0f};
	float m_vertical_fov{math::half_pi<float>};
	float m_aspect_ratio{1.0f};
	float m_exposure_value{0.0f};
	float m_exposure_normalization{1.0f / 1.2f};
	
	math::fmat4 m_view{math::fmat4::identity()};
	math::fmat4 m_projection{math::fmat4::identity()};
	math::fmat4 m_view_projection{math::fmat4::identity()};
	math::fmat4 m_inverse_view_projection{math::fmat4::identity()};
	
	math::fvec3 m_forward{0.0f, 0.0f, -1.0f};
	math::fvec3 m_up{0.0f, 1.0f, 0.0f};
	
	view_frustum_type m_view_frustum;
	
	aabb_type m_bounds{{0, 0, 0}, {0, 0, 0}};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_CAMERA_HPP
