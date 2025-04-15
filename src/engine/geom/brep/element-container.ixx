// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.brep.element_container;
export import engine.geom.brep.attribute_map;
import engine.utility.sized_types;
export import <iterator>;
import <memory>;
import <vector>;

export namespace engine::geom::brep
{
	class mesh;

	/// Container for B-rep elements.
	/// @tparam T Element type.
	template <class T>
	class element_container
	{
	public:
		using element_type = T;

		struct const_iterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using iterator_concept = std::random_access_iterator_tag;
			using difference_type = isize;
			using value_type = element_type*;
			using pointer = const value_type*;
			using reference = const value_type&;

			[[nodiscard]] inline constexpr value_type operator*() const noexcept
			{
				return m_it->get();
			}

			[[nodiscard]] inline constexpr value_type operator->() const noexcept
			{
				return m_it->get();
			}

			[[nodiscard]] inline constexpr value_type operator[](difference_type i) const noexcept
			{
				return m_it[i]->get();
			}

			inline const_iterator& operator++() noexcept
			{
				++m_it;
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
				--m_it;
				return *this;
			}

			[[nodiscard]] inline const_iterator operator--(int) noexcept
			{
				const_iterator tmp = *this;
				--(*this);
				return tmp;
			}

			inline const_iterator& operator+=(difference_type n) noexcept
			{
				m_it += n;
				return *this;
			}

			inline const_iterator& operator-=(difference_type n) noexcept
			{
				m_it -= n;
				return *this;
			}

			[[nodiscard]] inline bool operator==(const const_iterator& other) const noexcept
			{
				return m_it == other.m_it;
			};

			[[nodiscard]] inline std::weak_ordering operator<=>(const const_iterator& other) const noexcept
			{
				return m_it <=> other.m_it;
			}

			[[nodiscard]] inline difference_type operator-(const const_iterator& rhs) const noexcept
			{
				return m_it - rhs.m_it;
			}

			[[nodiscard]] inline const_iterator operator+(difference_type n) const noexcept
			{
				return const_iterator{m_it + n};
			}

			[[nodiscard]] inline const_iterator operator-(difference_type n) const noexcept
			{
				return const_iterator{m_it - n};
			}

			[[nodiscard]] friend const_iterator operator+(difference_type lhs, const const_iterator& rhs) noexcept
			{
				return const_iterator{lhs + rhs.m_it};
			}

			[[nodiscard]] friend const_iterator operator-(difference_type lhs, const const_iterator& rhs) noexcept
			{
				return const_iterator{lhs - rhs.m_it};
			}

		private:
			friend class element_container;

			std::vector<std::unique_ptr<element_type>>::const_iterator m_it;
		};

		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		/// @name Element access
		/// @{

		/// Returns a pointer to the element at the specified index, with bounds checking.
		/// @param i Index of an element.
		/// @return Pointer to the element at index @p i.
		/// @exception std::out_of_range if @p i >= size().
		[[nodiscard]] inline constexpr element_type* at(usize i) const
		{
			return m_elements.at(i).get();
		}

		/// Returns a pointer to the element at the specified index.
		/// @param i Index of an element.
		/// @return Pointer to the element at index @p i.
		[[nodiscard]] inline constexpr element_type* operator[](usize i) const
		{
			return m_elements[i].get();
		}

		/// Returns the first element.
		[[nodiscard]] inline constexpr element_type* front() const noexcept
		{
			return m_elements.front().get();
		}

		/// Returns the last element.
		[[nodiscard]] inline constexpr element_type* back() const noexcept
		{
			return m_elements.back().get();
		}

		/// @}
		/// @name Iterators
		/// @{

		/// Returns an iterator to the first element.
		[[nodiscard]] inline constexpr const_iterator begin() const noexcept
		{
			const_iterator it;
			it.m_it = m_elements.begin();
			return it;
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
		{
			return begin();
		}

		/// Returns an iterator to the element following the last element.
		[[nodiscard]] inline constexpr const_iterator end() const noexcept
		{
			const_iterator it;
			it.m_it = m_elements.end();
			return it;
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator cend() const noexcept
		{
			return end();
		}

		/// Returns a reverse iterator to the first element of the reversed container.
		[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
		{
			return std::make_reverse_iterator(end());
		}

		/// @copydoc rbegin()
		[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
		{
			return rbegin();
		}

		/// Returns a reverse iterator to the element following the last element of the reversed container.
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

		/// Returns `true` if the container is empty, `false` otherwise.
		[[nodiscard]] inline constexpr bool empty() const noexcept
		{
			return m_elements.empty();
		}

		/// Returns the number of elements in the container.
		[[nodiscard]] inline constexpr usize size() const noexcept
		{
			return m_elements.size();
		}

		/// @}
		/// @name Attributes
		/// @{

		/// Returns the element attribute map.
		[[nodiscard]] attribute_map& attributes() noexcept
		{
			return m_attribute_map;
		}

		/// @copydoc attributes()
		[[nodiscard]] const attribute_map& attributes() const noexcept
		{
			return m_attribute_map;
		}

		/// @}

	protected:
		friend class mesh;

		/// Constructs a B-rep element container.
		constexpr element_container() noexcept = default;

		/// Destructs a B-rep element container.
		virtual ~element_container() = default;

		/// Associates the B-rep element container with a mesh.
		/// @param mesh Mesh with which to associate the container.
		inline void set_mesh(mesh* mesh) noexcept
		{
			m_mesh = mesh;
		}

		/// @name Modifiers
		/// @{

		/// Erases an element from the container.
		/// @param element Pointer to the element to erase.
		virtual void erase(element_type* element)
		{
			const auto index = element->m_index;

			for (auto& [key, values] : m_attribute_map.m_attributes)
			{
				values->erase(index);
			}
			--m_attribute_map.m_element_count;

			m_elements.back()->m_index = index;
			m_elements[index] = std::move(m_elements.back());
			m_elements.pop_back();
		}

		/// Appends a new element to the end of the container.
		/// @return Pointer to the new element.
		virtual element_type* emplace_back()
		{
			for (auto& [key, values] : m_attribute_map.m_attributes)
			{
				values->emplace_back();
			}
			++m_attribute_map.m_element_count;

			return m_elements.emplace_back(std::make_unique<element_type>()).get();
		}

		/// @}

		mesh* m_mesh{};

	private:
		std::vector<std::unique_ptr<element_type>> m_elements;
		attribute_map m_attribute_map;
	};
}
