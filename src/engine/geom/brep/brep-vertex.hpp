// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_VERTEX_HPP
#define ANTKEEPER_GEOM_BREP_VERTEX_HPP

#include <engine/geom/brep/brep-edge.hpp>
#include <engine/geom/brep/brep-element-container.hpp>
#include <cstddef>
#include <iterator>

namespace geom {

class brep_mesh;
class brep_edge;
class brep_vertex;
template <class T>
class brep_element_container;

/**
 * List of B-rep edges bounded by a common vertex.
 */
class brep_vertex_edge_list
{
public:
	friend class brep_mesh;
	friend class brep_vertex_container;
	friend class brep_edge_container;
	
	struct const_iterator
	{
	public:
		friend class brep_vertex_edge_list;
		
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = brep_edge*;
		using pointer = const value_type*;
		using reference = const value_type&;
		
		[[nodiscard]] inline constexpr value_type operator*() const noexcept
		{
			return m_edge;
		}
		
		[[nodiscard]] inline constexpr value_type operator->() const noexcept
		{
			return m_edge;
		}
		
		inline const_iterator& operator++() noexcept
		{
			m_edge = m_edge->m_vertex_next[m_edge->vertices()[1] == m_vertex];
			++m_position;
			return *this;
		}
		
		[[nodiscard]] inline const_iterator operator++(int) noexcept
		{
			const_iterator tmp = *this;
			++(*this);
			return tmp;
		}
		
		inline const_iterator& operator--() noexcept
		{
			m_edge = m_edge->m_vertex_previous[m_edge->vertices()[1] == m_vertex];
			--m_position;
			return *this;
		}
		
		[[nodiscard]] inline const_iterator operator--(int) noexcept
		{
			const_iterator tmp = *this;
			--(*this);
			return tmp;
		}
		
		[[nodiscard]] inline bool operator==(const const_iterator& other) const noexcept
		{
			return m_position == other.m_position;
		};
		
        [[nodiscard]] inline std::weak_ordering operator<=>(const const_iterator& other) const noexcept
		{
			return m_position <=> other.m_position;
        }
		
		[[nodiscard]] inline difference_type operator-(const const_iterator& rhs) const noexcept
		{
			return m_position - rhs.m_position;
		}
		
	private:
		brep_vertex* m_vertex;
		brep_edge* m_edge;
		std::ptrdiff_t m_position;
	};
	
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	
	/// @name Element access
	/// @{
	
	/// Returns the first edge.
	[[nodiscard]] inline brep_edge* front() const noexcept
	{
		return m_head;
	}
	
	/// Returns the last edge.
	[[nodiscard]] inline brep_edge* back() const noexcept
	{
		return m_head->m_vertex_previous[m_head->vertices()[1] == m_vertex];
	}
	
	/// @}
	/// @name Iterators
	/// @{
	
	/// Returns an iterator to the first edge.
	/// @{
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		const_iterator it;
		it.m_vertex = m_vertex;
		it.m_edge = m_head;
		it.m_position = 0;
		
		return it;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return begin();
	}
	/// @}
	
	/// Returns an iterator to the edge following the last edge.
	/// @{
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		const_iterator it;
		it.m_vertex = m_vertex;
		it.m_edge = m_head;
		it.m_position = static_cast<std::ptrdiff_t>(m_size);
		
		return it;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return end();
	}
	/// @}
	
	/// Returns a reverse iterator to the first edge of the reversed list.
	/// @{
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return std::make_reverse_iterator(end());
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}
	/// @}
	
	/// Returns a reverse iterator to the edge following the last edge of the reversed list.
	/// @{
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return std::make_reverse_iterator(begin());
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return rend();
	}
	/// @}
	
	/// @}
	/// @name Capacity
	/// @{
	
	/// Returns `true` if the list is empty, `false` otherwise.
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return !m_size;
	}
	
	/// Returns the number of edges in the list.
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return m_size;
	}
	
	/// @}
	/// @name Modifiers
	/// @{
	
	/**
	 * Appends an edge to the end of the list.
	 *
	 * @param edge Pointer to the edge to append.
	 */
	void push_back(brep_edge* edge);
	
	/**
	 * Removes an edge from the list.
	 *
	 * @param edge Pointer to the edge to remove.
	 */
	void remove(brep_edge* edge);
	
	/// @}
	
private:
	brep_vertex* m_vertex{};
	brep_edge* m_head{};
	std::size_t m_size{};
};

/**
 * A point in space.
 */
class brep_vertex
{
public:
	friend class brep_mesh;
	friend class brep_element_container<brep_vertex>;
	friend class brep_vertex_container;
	friend class brep_edge_container;
	
	/**
	 * Returns the index of this vertex in the mesh vertex array.
	 *
	 * @warning This index may change if any vertices are removed from the mesh.
	 */
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the list of edges bounded by this vertex.
	[[nodiscard]] inline constexpr const brep_vertex_edge_list& edges() const noexcept
	{
		return m_edges;
	}
	
private:
	std::size_t m_index;
	brep_vertex_edge_list m_edges;
};

/**
 * B-rep vertex container.
 */
class brep_vertex_container: public brep_element_container<brep_vertex>
{
public:
	/** Destructs a B-rep vertex container. */
	~brep_vertex_container() override = default;
	
	/**
	 * Appends a new vertex to the end of the container.
	 *
	 * @return Pointer to the new vertex.
	 */
	brep_vertex* emplace_back() override;
	
	/**
	 * Erases a vertex and all dependent edges, loops, and faces.
	 *
	 * @param vertex Pointer to the vertex to erase.
	 *
	 * @warning Invalidates iterators and indices of vertices, edges, loops, and faces.
	 */
	void erase(brep_vertex* vertex) override;
	
	/**
	 * Erases all vertices and their dependent edges, loops, and faces.
	 */
	void clear() noexcept;
	
private:
	friend class brep_mesh;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_VERTEX_HPP
