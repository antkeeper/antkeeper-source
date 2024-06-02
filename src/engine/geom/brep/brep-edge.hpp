// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BREP_EDGE_HPP
#define ANTKEEPER_GEOM_BREP_EDGE_HPP

#include <engine/geom/brep/brep-loop.hpp>
#include <engine/geom/brep/brep-element-container.hpp>
#include <array>
#include <cstddef>
#include <iterator>

namespace geom {

class brep_vertex;
class brep_loop;
template <class T>
class brep_element_container;

/**
 * List of B-rep loops that share a common edge.
 */
class brep_edge_loop_list
{
public:
	friend class brep_mesh;
	friend class brep_face_container;
	
	struct const_iterator
	{
	public:
		friend class brep_edge_loop_list;
		
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_concept = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = brep_loop*;
		using pointer = const value_type*;
		using reference = const value_type&;
		
		[[nodiscard]] inline constexpr value_type operator*() const noexcept
		{
			return m_loop;
		}
		
		[[nodiscard]] inline constexpr value_type operator->() const noexcept
		{
			return m_loop;
		}
		
		inline const_iterator& operator++() noexcept
		{
			m_loop = m_loop->m_edge_next;
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
			m_loop = m_loop->m_edge_previous;
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
		brep_loop* m_loop{};
		std::ptrdiff_t m_position{};
	};
	
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	
	/// @name Element access
	/// @{
	
	/** Returns the first loop. */
	[[nodiscard]] inline brep_loop* front() const noexcept
	{
		return m_head;
	}
	
	/** Returns the last loop. */
	[[nodiscard]] inline brep_loop* back() const noexcept
	{
		return m_head->m_edge_previous;
	}
	
	/// @}
	/// @name Iterators
	/// @{
	
	/// @{
	/** Returns an iterator to the first loop. */
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		const_iterator it;
		it.m_loop = m_head;
		it.m_position = 0;
		
		return it;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return begin();
	}
	/// @}
	
	/// @{
	/** Returns an iterator to the loop following the last loop. */
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		const_iterator it;
		it.m_loop = m_head;
		it.m_position = static_cast<std::ptrdiff_t>(m_size);
		
		return it;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return end();
	}
	/// @}
	
	/// @{
	/** Returns a reverse iterator to the first loop of the reversed list. */
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return std::make_reverse_iterator(end());
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}
	/// @}
	
	/// @{
	/** Returns a reverse iterator to the loop following the last loop of the reversed list. */
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
	
	/** Returns `true` if the list is empty, `false` otherwise. */
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return !m_size;
	}
	
	/** Returns the number of loops in the list. */
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return m_size;
	}
	
	/// @}
	/// @name Modifiers
	/// @{
	
	/**
	 * Appends a loop to the end of the list.
	 *
	 * @param loop Pointer to the loop to append.
	 */
	void push_back(brep_loop* loop);
	
	/**
	 * Removes an loop from the list.
	 *
	 * @param loop Pointer to the loop to remove.
	 */
	void remove(brep_loop* loop);
	
	/// @}
	
private:
	brep_loop* m_head{};
	std::size_t m_size{};
};

/**
 * Curve segment bounded by two vertices.
 */
class brep_edge
{
public:
	friend class brep_mesh;
	friend class brep_vertex_edge_list;
	friend class brep_element_container<brep_edge>;
	friend class brep_edge_container;
	friend class brep_face_container;
	
	/**
	 * Returns the index of this edge in the mesh edge array.
	 *
	 * @warning This index may change if any edges are removed from the mesh.
	 */
	[[nodiscard]] inline constexpr std::size_t index() const noexcept
	{
		return m_index;
	}
	
	/// Returns the pair of vertices that bound this edge.
	[[nodiscard]] inline constexpr const std::array<brep_vertex*, 2>& vertices() const noexcept
	{
		return m_vertices;
	}
	
	/// Returns the list of loops that share this edge.
	[[nodiscard]] inline constexpr const brep_edge_loop_list& loops() const noexcept
	{
		return m_loops;
	}
	
private:
	std::size_t m_index{};
	std::array<brep_vertex*, 2> m_vertices{};
	std::array<brep_edge*, 2> m_vertex_next{};
	std::array<brep_edge*, 2> m_vertex_previous{};
	brep_edge_loop_list m_loops;
};

/**
 * B-rep edge container.
 */
class brep_edge_container: public brep_element_container<brep_edge>
{
public:
	/** Destructs a B-rep edge container. */
	~brep_edge_container() override = default;
	
	/**
	 * Appends a new edge to the end of the container.
	 *
	 * @param a First bounding vertex of the edge.
	 * @param b Second bounding vertex of the edge.
	 *
	 * @return Pointer to the new edge.
	 */
	brep_edge* emplace_back(brep_vertex* a, brep_vertex* b);
	
	/**
	 * Erases an edge and all dependent loops and faces.
	 *
	 * @param edge Pointer to the edge to erase.
	 *
	 * @warning Invalidates iterators and indices of edges, loops, and faces.
	 */
	void erase(brep_edge* edge) override;
	
	/**
	 * Erases all edges and their dependent loops and faces.
	 */
	void clear() noexcept;
	
	/**
	 * Finds an edge bounded by vertices @p a and @p b (in any order).
	 *
	 * @param a First (or second) bounding vertex of the edge.
	 * @param b Second (or first) bounding vertex of the edge.
	 *
	 * @return Pointer to an edge bounded by vertices @p a and @p b, or `nullptr` if no such edge was found.
	 */
	[[nodiscard]] brep_edge* find(brep_vertex* a, brep_vertex* b) const;
	
private:
	friend class brep_mesh;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_EDGE_HPP
