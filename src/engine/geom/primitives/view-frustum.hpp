// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_PRIMITIVES_VIEW_FRUSTUM_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_VIEW_FRUSTUM_HPP

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/geom/primitives/plane.hpp>
#include <engine/geom/primitives/box.hpp>
#include <engine/geom/primitives/sphere.hpp>

namespace geom {
namespace primitives {

/// View frustum.
/// @tparam T Real type.
template <class T>
struct view_frustum
{
	/// Vector type.
	using vector_type = math::vec3<T>;
	
	/// Plane type.
	using plane_type = geom::plane<T>;
	
	/// View-projection matrix type.
	using matrix_type = math::mat4<T>;
	
	/// Box type.
	using box_type = geom::box<T>;
	
	/// Sphere type.
	using sphere_type = geom::sphere<T>;
	
	/// Constructs a view frustum
	constexpr view_frustum() noexcept = default;
	
	/// Constructs a view frustum by extracting planes from view-projection matrix.
	/// @param matrix View-projection matrix from which to extract view frustum planes.
	inline explicit constexpr view_frustum(const matrix_type& matrix) noexcept
	{
		extract(matrix);
	}
	
	/// Returns the left clipping plane.
	[[nodiscard]] inline constexpr plane_type& left() noexcept
	{
		return planes[0];
	}

	/// @copydoc left()
	[[nodiscard]] inline constexpr const plane_type& left() const noexcept
	{
		return planes[0];
	}
	
	/// Returns the right clipping plane.
	[[nodiscard]] inline constexpr plane_type& right() noexcept
	{
		return planes[1];
	}

	/// @copydoc right()
	[[nodiscard]] inline constexpr const plane_type& right() const noexcept
	{
		return planes[1];
	}
	
	/// Returns the bottom clipping plane.
	[[nodiscard]] inline constexpr plane_type& bottom() noexcept
	{
		return planes[2];
	}

	/// @copydoc bottom()
	[[nodiscard]] inline constexpr const plane_type& bottom() const noexcept
	{
		return planes[2];
	}
	
	/// Returns the top clipping plane.
	[[nodiscard]] inline constexpr plane_type& top() noexcept
	{
		return planes[3];
	}

	/// @copydoc top()
	[[nodiscard]] inline constexpr const plane_type& top() const noexcept
	{
		return planes[3];
	}
	
	/// Returns the near clipping plane.
	[[nodiscard]] inline constexpr plane_type& near() noexcept
	{
		return planes[4];
	}

	/// @copydoc near()
	[[nodiscard]] inline constexpr const plane_type& near() const noexcept
	{
		return planes[4];
	}
	
	/// Returns the far clipping plane.
	[[nodiscard]] inline constexpr plane_type& far() noexcept
	{
		return planes[5];
	}

	/// @copydoc far()
	[[nodiscard]] inline constexpr const plane_type& far() const noexcept
	{
		return planes[5];
	}
	
	/// Extracts the view frustum planes from a view-projection matrix.
	/// @param matrix View-projection matrix from which to extract view frustum planes.
	void extract(const matrix_type& matrix) noexcept
	{
		for (std::size_t i = 0; i < 6; ++i)
		{
			plane_type& plane = planes[i];		
			const std::size_t j = i >> 1;
			
			// Extract plane coefficients
			if (i % 2)
			{
				plane.normal.x()   = matrix[0][3] - matrix[0][j];
				plane.normal.y()   = matrix[1][3] - matrix[1][j];
				plane.normal.z()   = matrix[2][3] - matrix[2][j];
				plane.constant     = matrix[3][3] - matrix[3][j];
			}
			else
			{
				plane.normal.x()   = matrix[0][3] + matrix[0][j];
				plane.normal.y()   = matrix[1][3] + matrix[1][j];
				plane.normal.z()   = matrix[2][3] + matrix[2][j];
				plane.constant     = matrix[3][3] + matrix[3][j];
			}
			
			// Normalize plane coefficients
			const T inv_length = math::inv_length(plane.normal);
			plane.normal *= inv_length;
			plane.constant *= inv_length;
		}
	}
	
	/// Tests for intersection between an axis-aligned box and the view frustum.
	/// @param box Box to test for intersection with the view frustum.
	/// @return `true` if the axis-aligned box intersects the view frustum, `false` otherwise.
	[[nodiscard]] bool intersects(const box_type& box) const noexcept
	{
		for (const auto& plane: planes)
		{
			const vector_type p
			{
				(plane.normal.x() > T{0}) ? box.max.x() : box.min.x(),
				(plane.normal.y() > T{0}) ? box.max.y() : box.min.y(),
				(plane.normal.z() > T{0}) ? box.max.z() : box.min.z()
			};
			
			if (plane.distance(p) < T{0})
			{
				return false;
			}
		}
		
		return true;
	}
	
	/// Tests for intersection between a sphere and the view frustum.
	/// @param sphere Sphere to test for intersection with the view frustum.
	/// @return `true` if the sphere intersects the view frustum, `false` otherwise.
	[[nodiscard]] bool intersects(const sphere_type& sphere) const noexcept
	{
		for (const auto& plane: planes)
		{
			if (plane.distance(sphere.center) < -sphere.radius)
			{
				return false;
			}
		}
		
		return true;
	}
	
	/// Tests whether a point is contained within this view frustum.
	/// @param point Point to test for containment.
	/// @return `true` if the point is contained within this view frustum, `false` otherwise.
	[[nodiscard]] constexpr bool contains(const vector_type& point) const noexcept
	{
		for (const auto& plane: planes)
		{
			if (plane.distance(point) < T{0})
			{
				return false;
			}
		}
		
		return true;
	}
	
	/// Checks if an axis-aligned box is completely contained within the view frustum.
	/// @param box Box to test for containment within the view frustum.
	/// @return `true` if the axis-aligned box is completely contained within the view frustum, `false` otherwise.
	[[nodiscard]] bool contains(const box_type& box) const noexcept
	{
		for (const auto& plane: planes)
		{
			const vector_type p
			{
				(plane.normal.x() > T{0}) ? box.max.x() : box.min.x(),
				(plane.normal.y() > T{0}) ? box.max.y() : box.min.y(),
				(plane.normal.z() > T{0}) ? box.max.z() : box.min.z()
			};
			
			const vector_type n
			{
				(plane.normal.x() < T{0}) ? box.max.x() : box.min.x(),
				(plane.normal.y() < T{0}) ? box.max.y() : box.min.y(),
				(plane.normal.z() < T{0}) ? box.max.z() : box.min.z()
			};
		
			if (plane.distance(p) < T{0} || plane.distance(n) < T{0})
			{
				return false;
			}
		}
		
		return true;
	}
	
	/// Checks if a sphere is completely contained within the view frustum.
	/// @param sphere Sphere to test for containment within the view frustum.
	/// @return `true` if the sphere is completely contained within the view frustum, `false` otherwise.
	[[nodiscard]] bool contains(const sphere_type& sphere) const noexcept
	{
		for (const auto& plane: planes)
		{
			if (plane.distance(sphere.center) < sphere.radius)
			{
				return false;
			}
		}
		
		return true;
	}
	
	/// View frustum clipping planes.
	/// 
	/// Clipping planes are stored in the following order:
	/// 
	/// 1. left
	/// 2. right
	/// 3. bottom
	/// 4. top
	/// 5. near
	/// 6. far
	plane_type planes[6];
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_VIEW_FRUSTUM_HPP
