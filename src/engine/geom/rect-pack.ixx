// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.rect_pack;
export import engine.geom.primitives.rectangle;
import <memory>;

export namespace engine::geom
{
	/// Packs 2D rectangles.
	/// @tparam T Scalar type.
	/// @see http://www.blackpawn.com/texts/lightmaps/
	template <class T>
	class rect_pack
	{
	public:
		/// Scalar type.
		using scalar_type = T;

		/// Vector type.
		using vector_type = math::vec2<T>;

		/// Rectangle type.
		using rect_type = rectangle<T>;

		/// Node type.
		struct node_type
		{
			/// Pointers to the two children of the node, if any.
			std::unique_ptr<node_type> children[2];

			/// Bounds of the node.
			rect_type bounds{};

			/// Whether the node is occupied.
			bool occupied{false};
		};

		/// Constructs a rect pack.
		/// @param dimensions Dimensions of the root node.
		explicit rect_pack(const vector_type& dimensions = {})
		{
			m_root_node.bounds.max = dimensions;
		}

		/// Clear the pack, deallocating all non-root nodes.
		void clear()
		{
			m_root_node.children[0].reset();
			m_root_node.children[1].reset();
			m_root_node.occupied = false;
		}

		/// Resizes the pack, deallocating all non-root nodes.
		/// @param dimensions New dimensions of the root node.
		void resize(const vector_type& dimensions)
		{
			clear();
			m_root_node.bounds.max = dimensions;
		}

		/// Inserts a rect.
		/// @param dimensions Dimensions of the rect.
		/// @return Pointer to the node in which the rect was inserted, or `nullptr` if the rect could not be inserted.
		inline const node_type* insert(const vector_type& dimensions)
		{
			return insert(m_root_node, dimensions);
		}

		/// Returns a reference to the root node.
		[[nodiscard]] inline constexpr const node_type& front() const noexcept
		{
			return m_root_node;
		}

	private:
		[[nodiscard]] node_type* insert(node_type& node, const vector_type& dimensions)
		{
			// If not a leaf node
			if (node.children[0] && node.children[1])
			{
				// Attempt to insert into first child
				if (auto result = insert(*node.children[0], dimensions))
				{
					return result;
				}

				// Insertion into first child failed, attempt to insert into second child
				return insert(*node.children[1], dimensions);
			}

			// If leaf node is occupied, insertion fails
			if (node.occupied)
			{
				return nullptr;
			}

			// Calculate node dimensions
			const auto node_dimensions = node.bounds.max - node.bounds.min;

			// If rect is larger than leaf node, insertion fails
			if (dimensions.x() > node_dimensions.x() || dimensions.y() > node_dimensions.y())
			{
				return nullptr;
			}

			// Check for a perfect fit
			if (dimensions == node_dimensions)
			{
				node.occupied = true;
				return &node;
			}

			// Split leaf node
			node.children[0] = std::make_unique<node_type>();
			node.children[1] = std::make_unique<node_type>();
			node.children[0]->bounds.min = node.bounds.min;
			node.children[1]->bounds.max = node.bounds.max;

			// Determine split direction
			if (node_dimensions.x() - dimensions.x() > node_dimensions.y() - dimensions.y())
			{
				node.children[0]->bounds.max = {node.bounds.min.x() + dimensions.x(), node.bounds.max.y()};
				node.children[1]->bounds.min = {node.bounds.min.x() + dimensions.x(), node.bounds.min.y()};
			}
			else
			{
				node.children[0]->bounds.max = {node.bounds.max.x(), node.bounds.min.y() + dimensions.y()};
				node.children[1]->bounds.min = {node.bounds.min.x(), node.bounds.min.y() + dimensions.y()};
			}

			// Insert into first child
			return insert(*node.children[0], dimensions);
		}

		node_type m_root_node{};
	};
}
