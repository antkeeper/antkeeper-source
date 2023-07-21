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

#ifndef ANTKEEPER_GEOM_BVH_NODE_HPP
#define ANTKEEPER_GEOM_BVH_NODE_HPP

#include <engine/geom/primitives/box.hpp>
#include <cstdint>

namespace geom {

/**
 * Single node in a bounding volume hierarchy.
 */
struct bvh_node
{
	/// Returns `true` if the node is a leaf node, `false` otherwise.
	[[nodiscard]] inline constexpr bool is_leaf() const noexcept
	{
		return size;
	}
	
	/// Node bounds.
	geom::box<float> bounds;
	
	/// Number of primitives in the node.
	std::uint32_t size;
	
	/// Offset to the first child node (non-leaf) or primitive (leaf).
	std::uint32_t offset;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BVH_NODE_HPP
