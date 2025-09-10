// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/primitives/box.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <engine/utility/sized-types.hpp>
#include <functional>
#include <span>
#include <vector>

namespace engine::geom
{
	/// Bounding volume hierarchy (BVH).
	class bvh
	{
	public:
		/// BVH node.
		struct node
		{
			/// Returns `true` if the node is a leaf node, `false` otherwise.
			[[nodiscard]] inline constexpr bool is_leaf() const noexcept
			{
				return size;
			}

			/// Node bounds.
			box<float> bounds{};

			/// Number of primitives in the node.
			u32 size{};

			/// Offset to the first child node (non-leaf) or primitive (leaf).
			u32 offset{};
		};

		/// BVH primitive.
		struct primitive
		{
			/// Geometric center of the primitive.
			math::fvec3 centroid;

			/// Axis-aligned bounding box containing the primitive.
			box<float> bounds;
		};

		using visitor_type = std::function<void(u32)>;

		/// Constructs a BVH from a set of primitives.
		/// @param primitives Axis-aligned bounding boxes.
		explicit bvh(std::span<const bvh::primitive> primitives);

		/// Constructs a BVH from a B-rep mesh.
		/// @param mesh B-rep mesh from which to build the BVH.
		explicit bvh(const brep::mesh& mesh);

		/// Constructs an empty BVH.
		constexpr bvh() noexcept = default;

		/// Constructs a BVH from a set of primitives.
		/// @param primitives BVH primitives.
		void build(std::span<const bvh::primitive> primitives);

		/// Constructs a BVH from a B-rep mesh.
		/// @param mesh B-rep mesh from which to build the BVH.
		void build(const brep::mesh& mesh);

		/// Clears the BVH.
		void clear();

		/// Visits the primitive indices of all BVH nodes that intersect a ray.
		/// @param ray Query ray.
		/// @param f Unary visitor function which operates on a BVH primitive index.
		inline void visit(const ray<float, 3>& ray, const visitor_type& f) const
		{
			if (m_node_count)
			{
				visit(m_nodes.front(), ray, f);
			}
		}

		/// Returns the BVH nodes.
		[[nodiscard]] inline constexpr const std::vector<bvh::node>& nodes() const noexcept
		{
			return m_nodes;
		}

	private:
		void update_bounds(bvh::node& node, const std::span<const bvh::primitive>& primitives);

		/// Builds the BVH through recursive subdivision.
		/// @param node Current node.
		/// @param primitives BVH primitives.
		void subdivide(bvh::node& node, const std::span<const bvh::primitive>& primitives);
		void visit(const bvh::node& node, const ray<float, 3>& ray, const visitor_type& f) const;

		std::vector<u32> m_primitive_indices;
		std::vector<bvh::node> m_nodes;
		u32 m_node_count{};
	};
}
