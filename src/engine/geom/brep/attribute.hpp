// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace engine::geom::brep
{
	/// Abstract base class for B-rep element attributes.
	class attribute_base
	{
	public:
		/// Destructs a B-rep attribute base.
		virtual ~attribute_base() = default;

		/// Returns the name of the attribute.
		[[nodiscard]] inline constexpr const std::string& name() const noexcept
		{
			return m_name;
		}

	protected:
		inline explicit constexpr attribute_base(const std::string_view& name) noexcept:
			m_name(name)
		{
		}

	private:
		friend class attribute_map;
		template <class T>
		friend class element_container;

		/// Erases the attribute value of an element with the given index.
		/// @param i Index of an element.
		virtual void erase(usize i) = 0;

		/// Appends a new attribute value to the end of the container.
		virtual void emplace_back() = 0;

		/// Returns a copy of this attribute.
		[[nodiscard]] virtual std::unique_ptr<attribute_base> clone() const = 0;

		std::string m_name;
	};

	/// Per-element B-rep data.
	/// @tparam T Data type.
	template <class T>
	class attribute: public attribute_base
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

		/// Constructs a B-rep attribute.
		/// @param name Name of the attribute.
		/// @param element_count Number of elements.
		attribute(const std::string_view& name, usize element_count):
			attribute_base(name),
			m_values(element_count)
		{
		}

		/// Destructs a B-rep attribute.
		~attribute() override = default;

		/// @name Attribute access
		/// @{

		/// Returns a reference to the attribute value of an element.
		/// @param i Index of an element.
		/// @return Reference to the attribute value of the element at index @p i.
		[[nodiscard]] inline constexpr reference operator[](usize i)
		{
			return m_values[i];
		}

		/// @copydoc operator[](usize)
		[[nodiscard]] inline constexpr const_reference operator[](usize i) const
		{
			return m_values[i];
		}

		/// Returns a reference to the attribute value of the first element.
		[[nodiscard]] inline constexpr reference front()
		{
			return m_values.front();
		}

		/// @copydoc front()
		[[nodiscard]] inline constexpr const_reference front() const
		{
			return m_values.front();
		}

		/// Returns a reference to the attribute value of the last element.
		[[nodiscard]] inline constexpr reference back()
		{
			return m_values.back();
		}

		/// @copydoc back()
		[[nodiscard]] inline constexpr const_reference back() const
		{
			return m_values.back();
		}

		/// Returns a pointer to the underlying array serving as attribute value storage.
		[[nodiscard]] inline constexpr value_type* data() noexcept
		{
			return m_values.data();
		}

		/// @copydoc data()
		[[nodiscard]] inline constexpr const value_type* data() const noexcept
		{
			return m_values.data();
		}

		/// @}
		/// @name Iterators
		/// @{

		/// Returns an iterator to the attribute value of the first element.
		[[nodiscard]] inline constexpr iterator begin() noexcept
		{
			return m_values.begin();
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator begin() const noexcept
		{
			return m_values.begin();
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
		{
			return m_values.begin();
		}

		/// Returns an iterator to the attribute value of the element following the last element.
		[[nodiscard]] inline constexpr iterator end() noexcept
		{
			return m_values.end();
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator end() const noexcept
		{
			return m_values.end();
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator cend() const noexcept
		{
			return m_values.end();
		}

		/// Returns a reverse iterator to the attribute value of the first element of the reversed container.
		[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
		{
			return m_values.rbegin();
		}

		/// @copydoc rbegin()
		[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
		{
			return m_values.rbegin();
		}

		/// @copydoc rbegin()
		[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
		{
			return m_values.rbegin();
		}

		/// Returns a reverse iterator to the attribute value of the element following the last element of the reversed container.
		[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
		{
			return m_values.rend();
		}

		/// @copydoc rend()
		[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
		{
			return m_values.rend();
		}

		/// @copydoc rend()
		[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
		{
			return m_values.rend();
		}

		/// @}
		/// @name Capacity
		/// @{

		/// Returns `true` if the container is empty, `false` otherwise.
		[[nodiscard]] inline constexpr bool empty() const noexcept
		{
			return m_values.empty();
		}

		/// Returns the number of attribute values in the container.
		[[nodiscard]] inline constexpr usize size() const noexcept
		{
			return m_values.size();
		}

		/// @}

	private:
		void erase(usize i) override
		{
			m_values[i] = std::move(m_values.back());
			m_values.pop_back();
		}

		void emplace_back() override
		{
			m_values.emplace_back();
		}

		[[nodiscard]] std::unique_ptr<attribute_base> clone() const override
		{
			auto copy = std::make_unique<attribute<T>>(name(), 0);
			copy->m_values = m_values;
			return copy;
		}

		std::vector<value_type> m_values;
	};
}
