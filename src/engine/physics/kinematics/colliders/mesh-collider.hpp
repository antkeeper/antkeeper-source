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

#ifndef ANTKEEPER_PHYSICS_MESH_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_MESH_COLLIDER_HPP

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/geom/bvh/bvh.hpp>
#include <engine/math/vector.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <memory>
#include <optional>

namespace physics {

/**
 * Mesh collision object.
 */
class mesh_collider: public collider
{
public:
	/// Collision mesh type.
	using mesh_type = geom::brep_mesh;
	
	/// Bounding volume hierarchy type.
	using bvh_type = geom::bvh;
	
	[[nodiscard]] inline constexpr collider_type type() const noexcept override
	{
		return collider_type::mesh;
	}
	
	/**
	 * Constructs a mesh collider from a mesh.
	 *
	 * @param mesh Collision mesh.
	 *
	 * @warning @p mesh must contain the math::fvec3 vertex attribute "position".
	 * @warning @p mesh must be a triangle mesh.
	 */
	explicit mesh_collider(std::shared_ptr<mesh_type> mesh);
	
	/// Constructs an empty mesh collider.
	constexpr mesh_collider() noexcept = default;
	
	/**
	 * Sets the collider's mesh.
	 *
	 * @param mesh Collision mesh.
	 *
	 * @warning @p mesh must contain the math::fvec3 vertex attribute "position".
	 * @warning @p mesh must be a triangle mesh.
	 */
	void set_mesh(std::shared_ptr<mesh_type> mesh);
	
	/// Returns the collision mesh.
	[[nodiscard]] inline constexpr const std::shared_ptr<mesh_type>& get_mesh() const noexcept
	{
		return m_mesh;
	}
	
	/// Returns the BVH of the collision mesh faces.
	[[nodiscard]] inline constexpr const bvh_type& get_bvh() const noexcept
	{
		return m_bvh;
	}
	
	/// Rebuilds the BVH of the collision mesh faces.
	void rebuild_bvh();
	
	/**
	 * Finds the nearest point of intersection between a ray and this collision mesh.
	 *
	 * @param ray Mesh-space ray.
	 *
	 * @return Tuple containing the distance along the ray to the nearest point of intersection, the index of the nearest mesh face, and the surface normal of the intersected face; or std::nullopt if no intersection occurred.
	 */
	[[nodiscard]] std::optional<std::tuple<float, std::uint32_t, math::fvec3>> intersection(const geom::ray<float, 3>& ray) const;
	
private:
	std::shared_ptr<mesh_type> m_mesh;
	const geom::brep_attribute<math::fvec3>* m_vertex_positions{};
	const geom::brep_attribute<math::fvec3>* m_face_normals{};
	bvh_type m_bvh;
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_MESH_COLLIDER_HPP
