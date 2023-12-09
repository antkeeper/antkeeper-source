// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_VECTOR_TYPE_HPP
#define ANTKEEPER_MATH_VECTOR_TYPE_HPP

#include <cstddef>
#include <format>
#include <iterator>
#include <type_traits>
#include <utility>

// export module math.vector:type;
// import <cstddef>;
// import <iterator>;
// import <type_traits>;
// import <utility>;

namespace math {

/**
 * *n*-dimensional vector.
 *
 * @tparam T Element type.
 * @tparam N Number of elements.
 */
template <class T, std::size_t N>
struct vector
{
	/// @name Member types
	/// @{
	using element_type = T;
	using value_type = std::remove_cv_t<T>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	/// @}
	
	/// Number of elements.
	static inline constexpr size_type element_count = N;
	
	/// @name Data members
	/// @{
	
	/// Vector elements.
	/// @private
	T elements[N];
	
	/// @}
	
	/// @name Conversion
	/// @{
	
	/// @private
	template <class U, std::size_t... I>
	[[nodiscard]] inline constexpr vector<U, N> type_cast(std::index_sequence<I...>) const noexcept
	{
		return {static_cast<U>(elements[I])...};
	}
	
	/**
	 * Type-casts the elements of this vector using `static_cast`.
	 *
	 * @tparam U Target element type.
	 *
	 * @return Vector containing the type-casted elements.
	 */
	template <class U>
	[[nodiscard]] inline constexpr explicit operator vector<U, N>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t M, std::size_t... I>
	[[nodiscard]] inline constexpr vector<T, M> size_cast(std::index_sequence<I...>) const noexcept
	{
		return {(I < N) ? elements[I] : T{} ...};
	}
	
	/**
	 * Size-casts this vector to a vector with a different number of elements. Casting to a greater number of elements causes new elements to be set to zero.
	 *
	 * @tparam M Target number of elements.
	 *
	 * @return *m*-dimensional vector.
	 */
	template <std::size_t M>
	[[nodiscard]] inline constexpr explicit operator vector<T, M>() const noexcept
	{
		return size_cast<M>(std::make_index_sequence<M>{});
	}
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/**
	 * Returns a reference to the element at a given index.
	 *
	 * @param i Index of an element.
	 *
	 * @return Reference to the element at index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference operator[](size_type i) noexcept
	{
		return elements[i];
	}
	[[nodiscard]] inline constexpr const_reference operator[](size_type i) const noexcept
	{
		return elements[i];
	}
	/// @}
	
	/**
	 * Returns a reference to the first element.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference front() noexcept
	{
		return elements[0];
	}
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		return elements[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the last element.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference back() noexcept
	{
		return elements[N - 1];
	}
	[[nodiscard]] inline constexpr const_reference back() const noexcept
	{
		return elements[N - 1];
	}
	/// @}
	
	/**
	 * Returns a pointer to the element array.
	 */
	/// @{
	[[nodiscard]] inline constexpr pointer data() noexcept
	{
		return elements;
	};
	[[nodiscard]] inline constexpr const_pointer data() const noexcept
	{
		return elements;
	};
	/// @}
	
	/// Returns a reference to the first element.
	/// @{
	[[nodiscard]] inline constexpr reference x() noexcept
	{
		static_assert(N > 0);
		return elements[0];
	}
	[[nodiscard]] inline constexpr const_reference x() const noexcept
	{
		static_assert(N > 0);
		return elements[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the second element.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference y() noexcept
	{
		static_assert(N > 1);
		return elements[1];
	}
	[[nodiscard]] inline constexpr const_reference y() const noexcept
	{
		static_assert(N > 1);
		return elements[1];
	}
	/// @}
	
	/**
	 * Returns a reference to the third element.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference z() noexcept
	{
		static_assert(N > 2);
		return elements[2];
	}
	[[nodiscard]] inline constexpr const_reference z() const noexcept
	{
		static_assert(N > 2);
		return elements[2];
	}
	/// @}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/**
	 * Returns an iterator to the first element.
	 */
	/// @{
	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return elements;
	}
	/// @}
	
	/**
	 * Returns an iterator to the element following the last element.
	 */
	/// @{
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return elements + N;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the first element of the reversed vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return elements + N;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return elements + N;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the element following the last element of the reversed vector.
	 */
	/// @{
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return elements;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return elements;
	}
	/// @}
	
	/// @}
	
	/// @name Capacity
	/// @{
	
	/**
	 * Returns `true` if the vector is empty, `false` otherwise.
	 */
	[[nodiscard]] inline static constexpr bool empty() noexcept
	{
		return N;
	};
	
	/**
	 * Returns the number of elements in the vector.
	 */
	/// @{
	[[nodiscard]] inline static constexpr size_type size() noexcept
	{
		return N;
	};
	[[nodiscard]] inline static constexpr size_type max_size() noexcept
	{
		return N;
	};
	/// @}
	
	/// @}
	
	/// @name Operations
	/// @{
	
	/**
	 * Fills the vector with the specified value.
	 *
	 * @param value Value to assign to each element.
	 */
	inline constexpr void fill(const T& value) noexcept
	{
		for (size_type i = 0; i < N; ++i)
		{
			elements[i] = value;
		}
	}
	
	/**
	 * Exchanges the elements of this vector with the elements of another.
	 *
	 * @param other Vector with which to exchange elements.
	 */
	[[nodiscard]] inline constexpr void swap(vector& other) noexcept
	{
		std::swap(elements, other.elements);
	};
	
	/// @}
};

/**
 * Extracts the Ith element from a vector using a tuple-like interface.
 *
 * @tparam I Index of an element.
 * @tparam T Element type.
 * @tparam N Number of elements.
 *
 * @param v Vector from which to extract an element.
 *
 * @return Reference to the Ith element of @p v.
 */
/// @{
template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr T& get(vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v.elements[I];
}

template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr T&& get(vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v.elements[I]);
}

template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr const T& get(const vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v.elements[I];
}

template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr const T&& get(const vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v.elements[I]);
}
/// @}

/// Vector types.
namespace vector_types {

/**
 * *n*-dimensional vector.
 *
 * @tparam T Element type.
 * @tparam N Number of elements.
 */
template <class T, std::size_t N>
using vec = vector<T, N>;

/**
 * 2-dimensional vector.
 *
 * @tparam T Element type.
 */
template <class T>
using vec2 = vector<T, 2>;

/**
 * 3-dimensional vector.
 *
 * @tparam T Element type.
 */
template <class T>
using vec3 = vector<T, 3>;

/**
 * 4-dimensional vector.
 *
 * @tparam T Element type.
 */
template <class T>
using vec4 = vector<T, 4>;

/**
 * *n*-dimensional vector of Boolean values.
 *
 * @tparam N Number of elements.
 */
/// @{
template <std::size_t N>
using bvec = vector<bool, N>;
using bvec2 = bvec<2>;
using bvec3 = bvec<3>;
using bvec4 = bvec<4>;
/// @}

/**
 * *n*-dimensional vector of signed integers.
 *
 * @tparam N Number of elements.
 */
/// @{
template <std::size_t N>
using ivec = vector<int, N>;
using ivec2 = ivec<2>;
using ivec3 = ivec<3>;
using ivec4 = ivec<4>;
/// @}

/**
 * *n*-dimensional vector of unsigned integers.
 *
 * @tparam N Number of elements.
 */
/// @{
template <std::size_t N>
using uvec = vector<unsigned int, N>;
using uvec2 = uvec<2>;
using uvec3 = uvec<3>;
using uvec4 = uvec<4>;
/// @}

/**
 * *n*-dimensional vector of single-precision floating-point numbers.
 *
 * @tparam N Number of elements.
 */
/// @{
template <std::size_t N>
using fvec = vector<float, N>;
using fvec2 = fvec<2>;
using fvec3 = fvec<3>;
using fvec4 = fvec<4>;
/// @}

/**
 * *n*-dimensional vector of double-precision floating-point numbers.
 *
 * @tparam N Number of elements.
 */
/// @{
template <std::size_t N>
using dvec = vector<double, N>;
using dvec2 = dvec<2>;
using dvec3 = dvec<3>;
using dvec4 = dvec<4>;
/// @}

} // namespace vector_types

// Bring vector types into math::types namespace
namespace types { using namespace math::vector_types; }

// Bring vector types into math namespace
using namespace vector_types;

} // namespace math

namespace std
{
	/**
	 * Provides access to the number of elements in a math::vector as a compile-time constant expression.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of elements.
	 */
	template<class T, std::size_t N>
	struct tuple_size<math::vector<T, N>>:
		std::integral_constant<std::size_t, N>
	{};
	
	/**
	 * Provides compile-time indexed access to the type of the elements in a math::vector using a tuple-like interface.
	 *
	 * @tparam I Index of an element.
	 * @tparam T Element type.
	 * @tparam N Number of elements.
	 */
	template<std::size_t I, class T, std::size_t N>
	struct tuple_element<I, math::vector<T, N>>
	{
		/// Vector element type.
		using type = T;
	};
	
	/**
	 * Specialization of std::formatter for math::vector.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of elements.
	 */
    template <class T, std::size_t N>
    struct formatter<math::vector<T, N>>: formatter<T>
    {
		auto format(const math::vector<T, N>& t, format_context& fc) const
		{
			auto&& out = fc.out();
			format_to(out, "{{");
			
			if constexpr (N)
			{
				formatter<T>::format(t[0], fc);
			}
			
			for (std::size_t i = 1; i < N; ++i)
			{
				format_to(out, ", ");
				formatter<T>::format(t[i], fc);
			}
			
			return format_to(out, "}}");
		}
    };
}

// Ensure floating-point vectors are POD types
static_assert(std::is_standard_layout_v<math::fvec3>);
static_assert(std::is_trivial_v<math::fvec3>);

#endif // ANTKEEPER_MATH_VECTOR_TYPE_HPP
