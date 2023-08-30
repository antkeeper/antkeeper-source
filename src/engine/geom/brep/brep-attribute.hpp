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

#ifndef ANTKEEPER_GEOM_BREP_ATTRIBUTE_HPP
#define ANTKEEPER_GEOM_BREP_ATTRIBUTE_HPP

#include <engine/utility/hash/fnv1a.hpp>
#include <memory>
#include <vector>

namespace geom {

/**
 * Abstract base class for B-rep element attributes.
 */
class brep_attribute_base
{
public:
	/// Returns the name of the attribute.
	[[nodiscard]] inline constexpr hash::fnv1a32_t name() const noexcept
	{
		return m_name;
	}

protected:
	inline explicit constexpr brep_attribute_base(hash::fnv1a32_t name) noexcept:
		m_name(name)
	{}
	
private:
	friend class brep_attribute_map;
	template <class T>
	friend class brep_element_container;
	
	/**
	 * Erases the attribute value of an element with the given index.
	 *
	 * @param i Index of an element.
	 */
	virtual void erase(std::size_t i) = 0;
	
	/**
	 * Appends a new attribute value to the end of the container.
	 */
	virtual void emplace_back() = 0;
	
	/// Returns a copy of this attribute.
	[[nodiscard]] virtual std::unique_ptr<brep_attribute_base> clone() const = 0;
	
	hash::fnv1a32_t m_name;
};

/**
 * Per-element B-rep data.
 *
 * @tparam T Data type.
 */
template <class T>
class brep_attribute: public brep_attribute_base
{
public:
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = std::vector<value_type>::iterator;
	using const_iterator = std::vector<value_type>::const_iterator;
	using reverse_iterator = std::vector<value_type>::reverse_iterator;
	using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;
	
	/**
	 * Constructs an attribute.
	 *
	 * @param name Name of the attribute.
	 * @param element_count Number of elements.
	 */
	brep_attribute(hash::fnv1a32_t name, std::size_t element_count):
		brep_attribute_base(name),
		m_values(element_count)
	{}
	
	/// @name Attribute access
	/// @{
	
	/**
	 * Returns a reference to the attribute value of an element.
	 *
	 * @param i Index of an element.
	 *
	 * @return Reference to the attribute value of the element at index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr const_reference operator[](std::size_t i) const
	{
		return m_values[i];
	}
	[[nodiscard]] inline constexpr reference operator[](std::size_t i)
	{
		return m_values[i];
	}
	/// @}
	
	/// Returns a reference to the attribute value of the first element.
	/// @{
	[[nodiscard]] inline constexpr const_reference front() const
	{
		return m_values.front();
	}
	[[nodiscard]] inline constexpr reference front()
	{
		return m_values.front();
	}
	/// @}
	
	/// Returns a reference to the attribute value of the last element.
	/// @{
	[[nodiscard]] inline constexpr const_reference back() const
	{
		return m_values.back();
	}
	[[nodiscard]] inline constexpr reference back()
	{
		return m_values.back();
	}
	/// @}
	
	/// Returns a pointer to the underlying array serving as attribute value storage.
	/// @{
	[[nodiscard]] inline constexpr const value_type* data() const noexcept
	{
		return m_values.data();
	}
	[[nodiscard]] inline constexpr value_type* data() noexcept
	{
		return m_values.data();
	}
	/// @}
	
	/// @}
	/// @name Iterators
	/// @{
	
	/// Returns an iterator to the attribute value of the first element.
	/// @{
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return m_values.begin();
	}
	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return m_values.begin();
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return m_values.begin();
	}
	/// @}
	
	/// Returns an iterator to the attribute value of the element following the last element.
	/// @{
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return m_values.end();
	}
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return m_values.end();
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return m_values.end();
	}
	/// @}
	
	/// Returns a reverse iterator to the attribute value of the first element of the reversed container.
	/// @{
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return m_values.rbegin();
	}
	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return m_values.rbegin();
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return m_values.rbegin();
	}
	/// @}
	
	/// Returns a reverse iterator to the attribute value of the element following the last element of the reversed container.
	/// @{
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return m_values.rend();
	}
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return m_values.rend();
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return m_values.rend();
	}
	/// @}
	
	/// @}
	/// @name Capacity
	/// @{
	
	/// Returns `true` if the container is empty, `false` otherwise.
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return m_values.empty();
	}
	
	/// Returns the number of attribute values in the container.
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return m_values.size();
	}
	
	/// @}
	
private:
	void erase(std::size_t i) override
	{
		m_values[i] = std::move(m_values.back());
		m_values.pop_back();
	}
	
	void emplace_back() override
	{
		m_values.emplace_back();
	}
	
	[[nodiscard]] std::unique_ptr<brep_attribute_base> clone() const override
	{
		auto copy = std::make_unique<brep_attribute<T>>(name(), 0);
		copy->m_values = m_values;
		return copy;
	}
	
	std::vector<value_type> m_values;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BREP_ATTRIBUTE_HPP
