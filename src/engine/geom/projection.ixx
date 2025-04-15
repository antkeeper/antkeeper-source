// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.projection;
export import engine.math.vector;

export namespace engine::geom
{
	/// Project a vector onto a plane.
	/// @tparam T Real type.
	/// @param v Vector to project.
	/// @param p Point on the plane.
	/// @param n Normal of the plane.
	/// @return Projected vector.
	template <class T>
	[[nodiscard]] math::vec3<T> project_on_plane(const math::vec3<T>& v, const math::vec3<T>& p, const math::vec3<T>& n) noexcept
	{
		return v - n * math::dot(v - p, n);
	}
}
