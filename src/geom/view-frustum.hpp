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

#ifndef ANTKEEPER_GEOM_VIEW_FRUSTUM_HPP
#define ANTKEEPER_GEOM_VIEW_FRUSTUM_HPP

#include "geom/convex-hull.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include <array>

namespace geom {

/**
 * View frustum.
 */
template <class T>
class view_frustum
{
public:
	typedef math::vector<T, 3> vector_type;
	typedef math::matrix<T, 4, 4> matrix_type;
	typedef plane<T> plane_type;
	
	/**
	 * Creates a view frustum from a view-projection matrix.
	 *
	 * @param view_projection View-projection matrix.
	 */
	view_frustum(const matrix_type& view_projection);
	
	/// Creates an uninitialized view frustum.
	view_frustum();
	
	/**
	 * Recalculates the view frustum from a view-projection matrix.
	 *
	 * @param view_projection View-projection matrix.
	 */
	void set_matrix(const matrix_type& view_projection);
	
	/// Returns a convex hull which describes the bounds of the view frustum.
	const convex_hull<T>& get_bounds() const;
	
	/// Returns the left clipping plane.
	const plane_type& get_left() const;
	
	/// Returns the right clipping plane.
	const plane_type& get_right() const;
	
	/// Returns the bottom clipping plane.
	const plane_type& get_bottom() const;
	
	/// Returns the top clipping plane.
	const plane_type& get_top() const;
	
	/// Returns the near clipping plane.
	const plane_type& get_near() const;
	
	/// Returns the far clipping plane.
	const plane_type& get_far() const;
	
	/**
	 * Returns an array containing the corners of the view frustum bounds.
	 *
	 * @return Array containing the corners of the view frustum bounds. Corners are stored in the following order: NTL, NTR, NBL, NBR, FTL, FTR, FBL, FBR; where N is near, F is far, T is top, B is bottom, L is left, and R is right, therefore NTL refers to the corner shared between the near, top, and left clipping planes.
	 */
	const std::array<vector_type, 8>& get_corners() const;

private:
	void recalculate_planes(const matrix_type& view_projection);
	void recalculate_corners();
	
	convex_hull<T> bounds;
	std::array<vector_type, 8> corners;
};

template <class T>
view_frustum<T>::view_frustum(const matrix_type& view_projection):
	bounds(6)
{
	set_matrix(view_projection);
}

template <class T>
view_frustum<T>::view_frustum():
	view_frustum(math::matrix4<T>::identity())
{}

template <class T>
void view_frustum<T>::set_matrix(const matrix_type& view_projection)
{
	recalculate_planes(view_projection);
	recalculate_corners();
}

template <class T>
inline const convex_hull<T>& view_frustum<T>::get_bounds() const
{
	return bounds;
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_left() const
{
	return bounds.planes[0];
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_right() const
{
	return bounds.planes[1];
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_bottom() const
{
	return bounds.planes[2];
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_top() const
{
	return bounds.planes[3];
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_near() const
{
	return bounds.planes[4];
}

template <class T>
inline const typename view_frustum<T>::plane_type& view_frustum<T>::get_far() const
{
	return bounds.planes[5];
}

template <class T>
inline const std::array<typename view_frustum<T>::vector_type, 8>& view_frustum<T>::get_corners() const
{
	return corners;
}

template <class T>
void view_frustum<T>::recalculate_planes(const matrix_type& view_projection)
{
	matrix_type transpose = math::transpose(view_projection);
	bounds.planes[0] = plane_type(transpose[3] + transpose[0]);
	bounds.planes[1] = plane_type(transpose[3] - transpose[0]);
	bounds.planes[2] = plane_type(transpose[3] + transpose[1]);
	bounds.planes[3] = plane_type(transpose[3] - transpose[1]);
	bounds.planes[4] = plane_type(transpose[3] + transpose[2]);
	bounds.planes[5] = plane_type(transpose[3] - transpose[2]);
}

template <class T>
void view_frustum<T>::recalculate_corners()
{
	/// @TODO THESE CORNERS MAY BE INCORRECT!!!!!!!!!!!
	corners[0] = plane_type::intersection(get_near(), get_top(), get_left());
	corners[1] = plane_type::intersection(get_near(), get_top(), get_right());
	corners[2] = plane_type::intersection(get_near(), get_bottom(), get_left());
	corners[3] = plane_type::intersection(get_near(), get_bottom(), get_right());
	corners[4] = plane_type::intersection(get_far(), get_top(), get_left());
	corners[5] = plane_type::intersection(get_far(), get_top(), get_right());
	corners[6] = plane_type::intersection(get_far(), get_bottom(), get_left());
	corners[7] = plane_type::intersection(get_far(), get_bottom(), get_right());
}

} // namespace geom

#endif // ANTKEEPER_GEOM_VIEW_FRUSTUM_HPP

