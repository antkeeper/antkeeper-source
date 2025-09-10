// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/brep/attribute.hpp>
#include <engine/utility/sized-types.hpp>
#include <iterator>
#include <string>
#include <utility>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace engine::geom::brep
{
	/// Maps names to B-rep attributes.
	class attribute_map
	{
	public:
		template <class Iter, bool Const>
		class iterator_template
		{
		public:
			using iterator_type = Iter;
			using iterator_category = std::bidirectional_iterator_tag;
			using iterator_concept = std::bidirectional_iterator_tag;
			using difference_type = std::iter_difference_t<Iter>;
			using value_type = attribute_base;
			using pointer = std::conditional<Const, const value_type*, value_type*>::type;
			using reference = std::conditional<Const, const value_type&, value_type&>::type;

			[[nodiscard]] inline constexpr reference operator*() const noexcept
			{
				return *m_it->second;
			}

			[[nodiscard]] inline constexpr pointer operator->() const noexcept
			{
				return &(*m_it->second);
			}

			[[nodiscard]] inline constexpr reference operator[](difference_type i) const noexcept
			{
				return *(m_it[i]);
			}

			inline iterator_template& operator++() noexcept
			{
				++m_it;
				return *this;
			}

			[[nodiscard]] inline iterator_template operator++(int) noexcept
			{
				iterator_template tmp = *this;
				++(*this);
				return tmp;
			}

			inline iterator_template& operator--() noexcept
			{
				--m_it;
				return *this;
			}

			[[nodiscard]] inline iterator_template operator--(int) noexcept
			{
				iterator_template tmp = *this;
				--(*this);
				return tmp;
			}

			inline iterator_template& operator+=(difference_type n) noexcept
			{
				m_it += n;
				return *this;
			}

			inline iterator_template& operator-=(difference_type n) noexcept
			{
				m_it -= n;
				return *this;
			}

			[[nodiscard]] inline bool operator==(const iterator_template& other) const noexcept
			{
				return m_it == other.m_it;
			};

			[[nodiscard]] inline std::weak_ordering operator<=>(const iterator_template& other) const noexcept
			{
				return m_it <=> other.m_it;
			}

			[[nodiscard]] inline difference_type operator-(const iterator_template& rhs) const noexcept
			{
				return m_it - rhs.m_it;
			}

			[[nodiscard]] inline iterator_template operator+(difference_type n) const noexcept
			{
				return iterator_template{m_it + n};
			}

			[[nodiscard]] inline iterator_template operator-(difference_type n) const noexcept
			{
				return iterator_template{m_it - n};
			}

			[[nodiscard]] friend iterator_template operator+(difference_type lhs, const iterator_template& rhs) noexcept
			{
				return iterator_template{lhs + rhs.m_it};
			}

			[[nodiscard]] friend iterator_template operator-(difference_type lhs, const iterator_template& rhs) noexcept
			{
				return iterator_template{lhs - rhs.m_it};
			}

		private:
			friend class attribute_map;

			iterator_type m_it;
		};

		using iterator = iterator_template<std::unordered_map<std::string, std::unique_ptr<attribute_base>>::iterator, false>;
		using const_iterator = iterator_template<std::unordered_map<std::string, std::unique_ptr<attribute_base>>::const_iterator, true>;

		/// @name Iterators
		/// @{

		/// Returns an iterator to the first attribute.
		[[nodiscard]] inline iterator begin() noexcept
		{
			iterator it;
			it.m_it = m_attributes.begin();
			return it;
		}

		/// @copydoc begin()
		[[nodiscard]] inline const_iterator begin() const noexcept
		{
			const_iterator it;
			it.m_it = m_attributes.begin();
			return it;
		}

		/// @copydoc begin()
		[[nodiscard]] inline const_iterator cbegin() const noexcept
		{
			return begin();
		}

		/// Returns an iterator to the attribute following the last attribute.
		[[nodiscard]] inline iterator end() noexcept
		{
			iterator it;
			it.m_it = m_attributes.end();
			return it;
		}

		/// @copydoc end()
		[[nodiscard]] inline const_iterator end() const noexcept
		{
			const_iterator it;
			it.m_it = m_attributes.end();
			return it;
		}

		/// @copydoc end()
		[[nodiscard]] inline const_iterator cend() const noexcept
		{
			return end();
		}

		/// @}
		/// @name Capacity
		/// @{

		/// Returns `true` if the container is empty, `false` otherwise.
		[[nodiscard]] inline bool empty() const noexcept
		{
			return m_attributes.empty();
		}

		/// Returns the number of attributes in the container.
		[[nodiscard]] inline usize size() const noexcept
		{
			return m_attributes.size();
		}

		/// @}
		/// @name Modifiers
		/// @{

		/// Removes all attributes from the container.
		inline void clear() noexcept
		{
			m_attributes.clear();
		}

		/// Constructs a new attribute. If an attribute with the given name exists, it will be replaced.
		/// @tparam T Attribute data type.
		/// @param name Name of the new attribute.
		/// @return Iterator to the new attribute.
		template <class T>
		iterator emplace(const std::string& name)
		{
			if (auto i = m_attributes.find(name); i != m_attributes.end())
			{
				i->second.reset();
				i->second = std::make_unique<attribute<T>>(name, m_element_count);

				iterator it;
				it.m_it = i;
				return it;
			}

			iterator it;
			it.m_it = m_attributes.emplace(name, std::make_unique<attribute<T>>(name, m_element_count)).first;
			return it;
		}

		/// Removes an attribute from the container.
		/// @param pos Iterator to the attribute to remove.
		/// @return Iterator following the erased attribute.
		inline iterator erase(iterator pos)
		{
			iterator it;
			it.m_it = m_attributes.erase(pos.m_it);
			return it;
		}

		/// Removes an attribute from the container.
		/// @param name Name of the attribute to remove.
		/// @return Number of attributes removed (0 or 1).
		inline usize erase(const std::string& name)
		{
			return m_attributes.erase(name);
		}

		/// Constructs a new attribute if an attribute with the given name does not exist.
		/// @tparam T Attribute data type.
		/// @param name Name of the new attribute.
		/// @return Pair consisting of an iterator to the new or pre-existing attribute, and a Boolean value that's `true` if the new attribute was constructed, or `false` if an attribute with the given name pre-existed.
		template <class T>
		std::pair<iterator, bool> try_emplace(const std::string& name)
		{
			if (auto i = m_attributes.find(name); i != m_attributes.end())
			{
				iterator it;
				it.m_it = i;
				return {it, false};
			}

			auto pair = m_attributes.emplace(name, std::make_unique<attribute<T>>(name, m_element_count));
			iterator it;
			it.m_it = pair.first;
			return {it, pair.second};
		}

		/// @}
		/// @name Lookup
		/// @{

		/// Returns a reference to the attribute with the given name. If no such attribute exists, an exception of type std::out_of_range is thrown.
		/// @tparam T Attribute data type.
		/// @param name Attribute name.
		/// @return Reference to the attribute with the given name.
		/// @exception std::out_of_range B-rep attribute not found.
		template <class T>
		[[nodiscard]] attribute<T>& at(const std::string& name)
		{
			auto it = find(name);
			if (it == end())
			{
				throw std::out_of_range("B-rep attribute not found");
			}

			return static_cast<attribute<T>&>(*it);
		}

		/// @copydoc at(const std::string&)
		template <class T>
		[[nodiscard]] const attribute<T>& at(const std::string& name) const
		{
			auto it = find(name);
			if (it == end())
			{
				throw std::out_of_range("B-rep attribute not found");
			}

			return static_cast<const attribute<T>&>(*it);
		}

		/// Finds an attribute with the given name.
		/// @param name Attribute name.
		/// @return Iterator to the attribute with the given name. If no such attribute is found, an end iterator is returned.
		[[nodiscard]] inline iterator find(const std::string& name)
		{
			iterator it;
			it.m_it = m_attributes.find(name);
			return it;
		}

		/// @copydoc find(const std::string&)
		[[nodiscard]] inline const_iterator find(const std::string& name) const
		{
			const_iterator it;
			it.m_it = m_attributes.find(name);
			return it;
		}

		/// Checks if there is an attribute with a given name in the container.
		/// @param name Attribute name.
		/// @return `true` if an attribute with the given name was found, `false` otherwise.
		[[nodiscard]] inline bool contains(const std::string& name) const
		{
			return m_attributes.contains(name);
		}

		/// @}

	private:
		template <class T>
		friend class element_container;
		friend class mesh;

		attribute_map& operator=(const attribute_map& other)
		{
			m_element_count = other.m_element_count;
			m_attributes.clear();
			for (const auto& [key, value] : other.m_attributes)
			{
				m_attributes.emplace(key, value->clone());
			}

			return *this;
		}

		usize m_element_count{};
		std::unordered_map<std::string, std::unique_ptr<attribute_base>> m_attributes;
	};
}
