// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/edge.hpp>
#include <engine/utility/sized-types.hpp>
#include <iterator>

namespace engine::geom::brep
{
	class vertex;

	/// List of B-rep edges bounded by a common vertex.
	class vertex_edge_list
	{
	public:
		friend class mesh;
		friend class vertex_container;
		friend class edge_container;

		/// B-rep edge list constant iterator.
		struct const_iterator
		{
		public:
			friend class vertex_edge_list;

			using iterator_category = std::bidirectional_iterator_tag;
			using iterator_concept = std::bidirectional_iterator_tag;
			using difference_type = isize;
			using value_type = edge*;
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
			vertex* m_vertex{};
			edge* m_edge{};
			isize m_position{};
		};

		/// B-rep edge list reverse constant iterator.
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		/// @name Element access
		/// @{

		/// Returns the first edge.
		[[nodiscard]] inline edge* front() const noexcept
		{
			return m_head;
		}

		/// Returns the last edge.
		[[nodiscard]] inline edge* back() const noexcept
		{
			return m_head->m_vertex_previous[m_head->vertices()[1] == m_vertex];
		}

		/// @}
		/// @name Iterators
		/// @{

		/// Returns an iterator to the first edge.
		[[nodiscard]] inline constexpr const_iterator begin() const noexcept
		{
			const_iterator it;
			it.m_vertex = m_vertex;
			it.m_edge = m_head;
			it.m_position = 0;

			return it;
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
		{
			return begin();
		}

		/// Returns an iterator to the edge following the last edge.
		[[nodiscard]] inline constexpr const_iterator end() const noexcept
		{
			const_iterator it;
			it.m_vertex = m_vertex;
			it.m_edge = m_head;
			it.m_position = static_cast<isize>(m_size);

			return it;
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator cend() const noexcept
		{
			return end();
		}

		/// Returns a reverse iterator to the first edge of the reversed list.
		[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
		{
			return std::make_reverse_iterator(end());
		}

		/// @copydoc rbegin()
		[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		/// Returns a reverse iterator to the edge following the last edge of the reversed list.
		[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
		{
			return std::make_reverse_iterator(begin());
		}

		/// @copydoc rend()
		[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
		{
			return rend();
		}

		/// @}
		/// @name Capacity
		/// @{

		/// Returns `true` if the list is empty, `false` otherwise.
		[[nodiscard]] inline constexpr bool empty() const noexcept
		{
			return !m_size;
		}

		/// Returns the number of edges in the list.
		[[nodiscard]] inline constexpr usize size() const noexcept
		{
			return m_size;
		}

		/// @}
		/// @name Modifiers
		/// @{

		/// Appends an edge to the end of the list.
		/// @param edge Pointer to the edge to append.
		void push_back(brep::edge* edge);

		/// Removes an edge from the list.
		/// @param edge Pointer to the edge to remove.
		void remove(brep::edge* edge);

		/// @}

	private:
		vertex* m_vertex{};
		edge* m_head{};
		usize m_size{};
	};
}
