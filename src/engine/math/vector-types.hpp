// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_VECTOR_TYPES_HPP
#define ANTKEEPER_MATH_VECTOR_TYPES_HPP

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

/// *n*-dimensional vector.
/// @tparam T Element type.
/// @tparam N Number of elements.
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
	
	/// Type-casts the elements of this vector using `static_cast`.
	/// @tparam U Target element type.
	/// @return Vector containing the type-casted elements.
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
	
	/// Size-casts this vector to a vector with a different number of elements. Casting to a greater number of elements causes new elements to be set to zero.
	/// @tparam M Target number of elements.
	/// @return *m*-dimensional vector.
	template <std::size_t M>
	[[nodiscard]] inline constexpr explicit operator vector<T, M>() const noexcept
	{
		return size_cast<M>(std::make_index_sequence<M>{});
	}
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/// Returns a reference to the *i*-th element in the vector.
	/// @param i Index of a vector element.
	/// @return Reference to the *i*-th element in the vector.
	/// @warning No bounds checking is performed.
	[[nodiscard]] inline constexpr reference operator[](size_type i) noexcept
	{
		return elements[i];
	}

	/// @copydoc operator[](size_type)
	[[nodiscard]] inline constexpr const_reference operator[](size_type i) const noexcept
	{
		return elements[i];
	}
	
	/// Returns a reference to the first element in the vector.
	[[nodiscard]] inline constexpr reference front() noexcept
	{
		return elements[0];
	}

	/// @copydoc front()
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		return elements[0];
	}
	
	/// Returns a reference to the last element in the vector.
	[[nodiscard]] inline constexpr reference back() noexcept
	{
		return elements[N - 1];
	}

	/// @copydoc back()
	[[nodiscard]] inline constexpr const_reference back() const noexcept
	{
		return elements[N - 1];
	}
	
	/// Returns a pointer to the first element in the vector's element array.
	[[nodiscard]] inline constexpr pointer data() noexcept
	{
		return elements;
	};

	/// @copydoc data()
	[[nodiscard]] inline constexpr const_pointer data() const noexcept
	{
		return elements;
	};
	
	/// Returns a reference to the first element in the vector.
	[[nodiscard]] inline constexpr reference x() noexcept
	{
		static_assert(N > 0);
		return elements[0];
	}

	/// @copydoc x()
	[[nodiscard]] inline constexpr const_reference x() const noexcept
	{
		static_assert(N > 0);
		return elements[0];
	}
	
	/// Returns a reference to the second element in the vector.
	[[nodiscard]] inline constexpr reference y() noexcept
	{
		static_assert(N > 1);
		return elements[1];
	}

	/// @copydoc y()
	[[nodiscard]] inline constexpr const_reference y() const noexcept
	{
		static_assert(N > 1);
		return elements[1];
	}
	
	/// Returns a reference to the third element in the vector.
	[[nodiscard]] inline constexpr reference z() noexcept
	{
		static_assert(N > 2);
		return elements[2];
	}

	/// @copydoc z()
	[[nodiscard]] inline constexpr const_reference z() const noexcept
	{
		static_assert(N > 2);
		return elements[2];
	}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/// Returns an iterator to the first element in the vector.
	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return elements;
	}

	/// @copydoc begin()
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return elements;
	}

	/// @copydoc begin()
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return elements;
	}
	
	/// Returns an iterator to the element following the last element in the vector.
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return elements + N;
	}

	/// @copydoc end()
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return elements + N;
	}

	/// @copydoc end()
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return elements + N;
	}
	
	/// Returns a reverse iterator to the first element in the reversed vector.
	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	/// @copydoc rbegin()
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	/// @copydoc rbegin()
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}
	
	/// Returns a reverse iterator to the element following the last element in the reversed vector.
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	/// @copydoc rend()
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

	/// @copydoc rend()
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(begin());
	}
	
	/// @}
	
	/// @name Capacity
	/// @{
	
	/// Returns `true` if the vector is empty, `false` otherwise.
	[[nodiscard]] inline static consteval bool empty() noexcept
	{
		return !N;
	};
	
	/// Returns the number of elements in the vector.
	[[nodiscard]] inline static consteval size_type size() noexcept
	{
		return N;
	};
	
	/// Returns the number of elements in the vector.
	[[nodiscard]] inline static consteval size_type max_size() noexcept
	{
		return N;
	};
	
	/// @}
	
	/// @name Operations
	/// @{
	
	/// Fills the vector with the specified value.
	/// @param value Value to assign to each element.
	inline constexpr void fill(const T& value) noexcept
	{
		for (size_type i = 0; i < N; ++i)
		{
			elements[i] = value;
		}
	}
	
	/// Exchanges the elements of this vector with the elements of another.
	/// @param other Vector with which to exchange elements.
	[[nodiscard]] inline constexpr void swap(vector& other) noexcept
	{
		std::swap(elements, other.elements);
	};
	
	/// @}
	
	/// @name Comparison
	/// @{
	
	/// Tests two vectors for equality.
	/// @return `true` if the two vectors are equivalent, `false` otherwise.
	[[nodiscard]] inline constexpr friend bool operator==(const vector&, const vector&) noexcept = default;
	
	/// Compares the elements of two vectors lexicographically.
	/// @return Lexicographical ordering of the two vectors.
	[[nodiscard]] inline constexpr friend auto operator<=>(const vector&, const vector&) noexcept = default; 
	
	/// @}
};

/// @name Tuple-like interface
/// @relates vector
/// @{

/// Extracts the *i*-th element from a vector using a tuple-like interface.
/// @tparam I Index of an element.
/// @tparam T Element type.
/// @tparam N Number of elements.
/// @param v Vector from which to extract an element.
/// @return Reference to the *i*-th element of @p v.
template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr T& get(vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v[I];
}

/// @copydoc get(vector<T, N>&)
template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr T&& get(vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v[I]);
}

/// @copydoc get(vector<T, N>&)
template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr const T& get(const vector<T, N>& v) noexcept
{
	static_assert(I < N);
	return v[I];
}

/// @copydoc get(vector<T, N>&)
template <std::size_t I, class T, std::size_t N>
[[nodiscard]] inline constexpr const T&& get(const vector<T, N>&& v) noexcept
{
	static_assert(I < N);
	return std::move(v[I]);
}

/// @}

/// @name Vector types
/// @relates vector
/// @{

/// @copydoc vector
template <class T, std::size_t N>
using vec = vector<T, N>;

/// 2-dimensional vector.
/// @tparam T Element type.
template <class T>
using vec2 = vec<T, 2>;

/// 3-dimensional vector.
/// @tparam T Element type.
template <class T>
using vec3 = vec<T, 3>;

/// 4-dimensional vector.
/// @tparam T Element type.
template <class T>
using vec4 = vec<T, 4>;

/// *n*-dimensional vector of Boolean values.
/// @tparam N Number of elements.
template <std::size_t N>
using bvec = vec<bool, N>;

/// 2-dimensional vector of Boolean values.
using bvec2 = bvec<2>;

/// 3-dimensional vector of Boolean values.
using bvec3 = bvec<3>;

/// 4-dimensional vector of Boolean values.
using bvec4 = bvec<4>;

/// *n*-dimensional vector of signed integers.
/// @tparam N Number of elements.
template <std::size_t N>
using ivec = vec<int, N>;

/// 2-dimensional vector of signed integers.
using ivec2 = ivec<2>;

/// 3-dimensional vector of signed integers.
using ivec3 = ivec<3>;

/// 4-dimensional vector of signed integers.
using ivec4 = ivec<4>;

/// *n*-dimensional vector of unsigned integers.
/// @tparam N Number of elements.
template <std::size_t N>
using uvec = vec<unsigned int, N>;

/// 2-dimensional vector of unsigned integers.
using uvec2 = uvec<2>;

/// 3-dimensional vector of unsigned integers.
using uvec3 = uvec<3>;

/// 4-dimensional vector of unsigned integers.
using uvec4 = uvec<4>;

/// *n*-dimensional vector of single-precision floating-point values.
/// @tparam N Number of elements.
template <std::size_t N>
using fvec = vec<float, N>;

/// 2-dimensional vector of single-precision floating-point values.
using fvec2 = fvec<2>;

/// 3-dimensional vector of single-precision floating-point values.
using fvec3 = fvec<3>;

/// 4-dimensional vector of single-precision floating-point values.
using fvec4 = fvec<4>;

/// *n*-dimensional vector of double-precision floating-point values.
/// @tparam N Number of elements.
template <std::size_t N>
using dvec = vec<double, N>;

/// 2-dimensional vector of double-precision floating-point values.
using dvec2 = dvec<2>;

/// 3-dimensional vector of double-precision floating-point values.
using dvec3 = dvec<3>;

/// 4-dimensional vector of double-precision floating-point values.
using dvec4 = dvec<4>;

/// @}

namespace types
{
	using math::vector;
	using math::vec;
	using math::vec2;
	using math::vec3;
	using math::vec4;
	using math::bvec;
	using math::bvec2;
	using math::bvec3;
	using math::bvec4;
	using math::ivec;
	using math::ivec2;
	using math::ivec3;
	using math::ivec4;
	using math::uvec;
	using math::uvec2;
	using math::uvec3;
	using math::uvec4;
	using math::fvec;
	using math::fvec2;
	using math::fvec3;
	using math::fvec4;
	using math::dvec;
	using math::dvec2;
	using math::dvec3;
	using math::dvec4;
} // namespace types

} // namespace math

namespace std
{
	/// Provides access to the number of elements in a math::vector as a compile-time constant expression.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	/// @relates math::vector
	template <class T, std::size_t N>
	struct tuple_size<math::vector<T, N>>:
		std::integral_constant<std::size_t, N>
	{};
	
	/// Provides compile-time indexed access to the type of the elements in a math::vector using a tuple-like interface.
	/// @tparam I Index of an element.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	/// @relates math::vector
	template <std::size_t I, class T, std::size_t N>
	struct tuple_element<I, math::vector<T, N>>
	{
		/// Vector element type.
		using type = T;
	};
	
	/// Specialization of std::formatter for math::vector.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	/// @relates math::vector
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

#endif // ANTKEEPER_MATH_VECTOR_TYPES_HPP
