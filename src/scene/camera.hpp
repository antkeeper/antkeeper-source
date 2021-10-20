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

#ifndef ANTKEEPER_SCENE_CAMERA_HPP
#define ANTKEEPER_SCENE_CAMERA_HPP

#include "scene/object.hpp"
#include "geom/view-frustum.hpp"
#include "utility/fundamental-types.hpp"

class compositor;

namespace scene {

/**
 *
 */
class camera: public object<camera>
{
public:
	typedef geom::view_frustum<float> view_frustum_type;
	
	camera();

	/**
	 * Maps object coordinates to window coordinates.
	 *
	 * @param object Object coordinates.
	 * @param viewport Vector containing the `x`, `y`, `w`, and `h` of the viewport.
	 * @return Projected window coordinates.
	 */
	float3 project(const float3& object, const float4& viewport) const;

	/**
	 * Maps window coordinates to object coordinates.
	 *
	 * @param window Window coordinates.
	 * @param viewport Vector containing the `x`, `y`, `w`, and `h` of the viewport.
	 * @return Unprojected object coordinates.
	 */
	float3 unproject(const float3& window, const float4& viewport) const;

	/**
	 * Sets the camera's projection matrix using perspective projection.
	 *
	 * @param fov Vertical field of view.
	 * @param aspect_ratio Aspect ratio.
	 * @param clip_near Distance to near clipping plane.
	 * @param clip_far Distance to far clipping plane.
	 */
	void set_perspective(float fov, float aspect_ratio, float clip_near, float clip_far);

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
	 * Sets the camera's exposure.
	 *
	 * @param exposure Exposure factor.
	 */
	void set_exposure(float exposure);

	void set_compositor(compositor* compositor);
	void set_composite_index(int index);
	
	
	virtual const bounding_volume_type& get_local_bounds() const;
	virtual const bounding_volume_type& get_world_bounds() const;

	float is_orthographic() const;
	float get_clip_left() const;
	float get_clip_right() const;
	float get_clip_bottom() const;
	float get_clip_top() const;
	float get_clip_near() const;
	float get_clip_far() const;
	float get_fov() const;
	float get_aspect_ratio() const;

	/// Returns the camera's view matrix.
	const float4x4& get_view() const;

	/// Returns the camera's projection matrix.
	const float4x4& get_projection() const;

	/// Returns the camera's view-projection matrix.
	const float4x4& get_view_projection() const;
	
	/// Returns the camera's view frustum.
	const view_frustum_type& get_view_frustum() const;
	
	/// Returns the camera's exposure.
	float get_exposure() const;

	const compositor* get_compositor() const;
	compositor* get_compositor();
	int get_composite_index() const;

	const tween<float>& get_clip_left_tween() const;
	const tween<float>& get_clip_right_tween() const;
	const tween<float>& get_clip_bottom_tween() const;
	const tween<float>& get_clip_top_tween() const;
	const tween<float>& get_clip_near_tween() const;
	const tween<float>& get_clip_far_tween() const;
	const tween<float>& get_fov_tween() const;
	const tween<float>& get_aspect_ratio_tween() const;
	const tween<float4x4>& get_view_tween() const;
	const tween<float4x4>& get_projection_tween() const;
	const tween<float4x4>& get_view_projection_tween() const;
	const tween<float>& get_exposure_tween() const;

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();

	compositor* compositor;
	int composite_index;
	bool orthographic;
	tween<float> clip_left;
	tween<float> clip_right;
	tween<float> clip_bottom;
	tween<float> clip_top;
	tween<float> clip_near;
	tween<float> clip_far;
	tween<float> fov;
	tween<float> aspect_ratio;
	tween<float4x4> view;
	tween<float4x4> projection;
	tween<float4x4> view_projection;
	tween<float> exposure;
	view_frustum_type view_frustum;
};

inline const typename object_base::bounding_volume_type& camera::get_local_bounds() const
{
	/// @TODO: return local bounds, not world bounds
	return view_frustum.get_bounds();
}

inline const typename object_base::bounding_volume_type& camera::get_world_bounds() const
{
	return view_frustum.get_bounds();
}

inline float camera::is_orthographic() const
{
	return orthographic;
}

inline float camera::get_clip_left() const
{	
	return clip_left[1];
}

inline float camera::get_clip_right() const
{
	return clip_right[1];
}

inline float camera::get_clip_bottom() const
{
	return clip_bottom[1];
}

inline float camera::get_clip_top() const
{
	return clip_top[1];
}

inline float camera::get_clip_near() const
{
	return clip_near[1];
}

inline float camera::get_clip_far() const
{
	return clip_far[1];
}

inline float camera::get_fov() const
{
	return fov[1];
}

inline float camera::get_aspect_ratio() const
{
	return aspect_ratio[1];
}

inline const float4x4& camera::get_view() const
{
	return view[1];
}

inline const float4x4& camera::get_projection() const
{
	return projection[1];
}

inline const float4x4& camera::get_view_projection() const
{
	return view_projection[1];
}

inline const typename camera::view_frustum_type& camera::get_view_frustum() const
{
	return view_frustum;
}

inline float camera::get_exposure() const
{
	return exposure[1];
}


inline const compositor* camera::get_compositor() const
{
	return compositor;
}

inline compositor* camera::get_compositor()
{
	return compositor;
}

inline int camera::get_composite_index() const
{
	return composite_index;
}

inline const tween<float>& camera::get_clip_left_tween() const
{
	return clip_left;
}

inline const tween<float>& camera::get_clip_right_tween() const
{
	return clip_right;
}

inline const tween<float>& camera::get_clip_bottom_tween() const
{
	return clip_bottom;
}

inline const tween<float>& camera::get_clip_top_tween() const
{
	return clip_top;
}

inline const tween<float>& camera::get_clip_near_tween() const
{
	return clip_near;
}

inline const tween<float>& camera::get_clip_far_tween() const
{
	return clip_far;
}

inline const tween<float>& camera::get_fov_tween() const
{
	return fov;
}

inline const tween<float>& camera::get_aspect_ratio_tween() const
{
	return aspect_ratio;
}

inline const tween<float4x4>& camera::get_view_tween() const
{
	return view;
}

inline const tween<float4x4>& camera::get_projection_tween() const
{
	return projection;
}

inline const tween<float4x4>& camera::get_view_projection_tween() const
{
	return view_projection;
}

inline const tween<float>& camera::get_exposure_tween() const
{
	return exposure;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_CAMERA_HPP
