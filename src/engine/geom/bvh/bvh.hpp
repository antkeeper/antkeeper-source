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

#ifndef ANTKEEPER_GEOM_BVH_HPP
#define ANTKEEPER_GEOM_BVH_HPP

#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/bvh/bvh-primitive.hpp>
#include <engine/geom/bvh/bvh-node.hpp>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace geom {

class brep_mesh;

/**
 * Bounding volume hierarchy (BVH).
 */
class bvh
{
public:
	using visitor_type = std::function<void(std::uint32_t)>;
	
	/**
	 * Constructs a BVH from a set of primitives.
	 *
	 * @param primitives Axis-aligned bounding boxes.
	 */
	explicit bvh(std::span<const bvh_primitive> primitives);
	
	/**
	 * Constructs a BVH from a B-rep mesh.
	 *
	 * @param mesh B-rep mesh from which to build the BVH.
	 */
	explicit bvh(const brep_mesh& mesh);
	
	/// Constructs an empty BVH.
	constexpr bvh() noexcept = default;
	
	/**
	 * Constructs a BVH from a set of primitives.
	 *
	 * @param primitives BVH primitives.
	 */
	void build(std::span<const bvh_primitive> primitives);
	
	/**
	 * Constructs a BVH from a B-rep mesh.
	 *
	 * @param mesh B-rep mesh from which to build the BVH.
	 */
	void build(const brep_mesh& mesh);
	
	/**
	 * Visits the primitive indices of all BVH nodes that intersect a ray.
	 *
	 * @param ray Query ray.
	 * @param f Unary visitor function which operates on a BVH primitive index.
	 */
	inline void visit(const geom::ray<float, 3>& ray, const visitor_type& f) const
	{
		if (m_node_count)
		{
			visit(m_nodes.front(), ray, f);
		}
	}
	
	/// Returns the BVH nodes.
	[[nodiscard]] inline constexpr const std::vector<bvh_node>& nodes() const noexcept
	{
		return m_nodes;
	}
	
private:
	void update_bounds(bvh_node& node, const std::span<const bvh_primitive>& primitives);
	
	/**
	 * Builds the BVH through recursive subdivision.
	 *
	 * @param node Current node.
	 * @param primitives BVH primitives.
	 */
	void subdivide(bvh_node& node, const std::span<const bvh_primitive>& primitives);
	void visit(const bvh_node& node, const geom::ray<float, 3>& ray, const visitor_type& f) const;
	
	std::vector<std::uint32_t> m_primitive_indices;
	std::vector<bvh_node> m_nodes;
	std::uint32_t m_node_count{};
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BVH_HPP
