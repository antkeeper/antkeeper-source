// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BVH_NODE_HPP
#define ANTKEEPER_GEOM_BVH_NODE_HPP

#include <engine/geom/primitives/box.hpp>
#include <cstdint>

namespace geom {

/// Single node in a bounding volume hierarchy.
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
