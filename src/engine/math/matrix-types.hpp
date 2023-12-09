// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_MATRIX_TYPES_HPP
#define ANTKEEPER_MATH_MATRIX_TYPES_HPP

#include <engine/math/vector.hpp>
#include <format>
#include <iterator>
#include <type_traits>
#include <utility>

// export module math.matrix:type;
// import math.vector;
// import <format>;
// import <iterator>;
// import <type_traits>;
// import <utility>;

namespace math {
namespace types {

/**
 * *n* by *m* column-major matrix.
 *
 * @tparam T Element type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 *
 * @see https://en.wikipedia.org/wiki/Row-_and_column-major_order
 */
template <class T, std::size_t N, std::size_t M>
struct matrix
{
	/// @name Member types
	/// @{
	using element_type = T;
	using column_type = vector<T, M>;
	using row_type = vector<T, N>;
	using value_type = column_type;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = column_type&;
	using const_reference = const column_type&;
	using pointer = column_type*;
	using const_pointer = const column_type*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	/// @}
	
	/// Number of columns.
	static inline constexpr size_type column_count = N;
	
	/// Number of rows.
	static inline constexpr size_type row_count = M;
	
	/// Number of elements.
	static inline constexpr size_type element_count = column_count * row_count;
	
	/// @name Data members
	/// @{
	
	/// Matrix columns.
	/// @private
	column_type columns[column_count];
	
	/// @}
	
	/// @name Conversion
	/// @{
	
	/// @private
	template <class U, std::size_t... I>
	[[nodiscard]] inline constexpr matrix<U, N, M> type_cast(std::index_sequence<I...>) const noexcept
	{
		return {vector<U, M>(columns[I])...};
	}
	
	/**
	 * Type-casts the elements of this matrix using `static_cast`.
	 *
	 * @tparam U Target element type.
	 *
	 * @return Matrix containing the type-casted elements.
	 */
	template <class U>
	[[nodiscard]] inline constexpr explicit operator matrix<U, N, M>() const noexcept
	{
		return type_cast<U>(std::make_index_sequence<N>{});
	}
	
	/// @private
	template <std::size_t P, std::size_t O, std::size_t... I>
	[[nodiscard]] inline constexpr matrix<T, P, O> size_cast(std::index_sequence<I...>) const noexcept
	{
		if constexpr (O == M)
		{
			return {((I < N) ? columns[I] : matrix<T, P, O>::identity()[I]) ...};
		}
		else
		{
			return {((I < N) ? vector<T, O>(columns[I]) : matrix<T, P, O>::identity()[I]) ...};
		}
	}
	
	/**
	 * Size-casts this matrix to a matrix with different dimensions. Casting to greater dimensions causes new elements to be set to identity matrix elements.
	 *
	 * @tparam P Target number of columns.
	 * @tparam O Target number of rows.
	 *
	 * @return *p* by *o* matrix.
	 */
	template <std::size_t P, std::size_t O>
	[[nodiscard]] inline constexpr explicit operator matrix<T, P, O>() const noexcept
	{
		return size_cast<P, O>(std::make_index_sequence<P>{});
	}
	
	/// @}
	
	/// @name Column access
	/// @{
	
	/**
	 * Returns a reference to the column at a given index.
	 *
	 * @param i Index of a column.
	 *
	 * @return Reference to the column at index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference operator[](size_type i) noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr const_reference operator[](size_type i) const noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr column_type& column(size_type i) noexcept
	{
		return columns[i];
	}
	[[nodiscard]] inline constexpr const column_type& column(size_type i) const noexcept
	{
		return columns[i];
	}
	/// @}
	
	/**
	 * Returns a reference to the first column.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference front() noexcept
	{
		return columns[0];
	}
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		return columns[0];
	}
	/// @}
	
	/**
	 * Returns a reference to the last column.
	 */
	/// @{
	[[nodiscard]] inline constexpr reference back() noexcept
	{
		return columns[column_count - 1];
	}
	[[nodiscard]] inline constexpr const_reference back() const noexcept
	{
		return columns[column_count - 1];
	}
	/// @}
	
	/// @}
	
	/// @name Element access
	/// @{
	
	/**
	 * Returns a reference to the element at a given column-major index.
	 *
	 * @param i Column-major index of a matrix element.
	 *
	 * @return Reference to the element at column-major index @p i.
	 */
	/// @{
	[[nodiscard]] inline constexpr element_type& element(size_type i) noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	[[nodiscard]] inline constexpr const element_type& element(size_type i) const noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	/// @}
	
	/**
	 * Returns a pointer to the first column.
	 */
	/// @{
	[[nodiscard]] inline constexpr pointer data() noexcept
	{
		return columns;
	};
	[[nodiscard]] inline constexpr const_pointer data() const noexcept
	{
		return columns;
	};
	/// @}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/**
	 * Returns an iterator to the first column.
	 */
	/// @{
	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return columns;
	}
	/// @}
	
	/**
	 * Returns an iterator to the column following the last column.
	 */
	/// @{
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return columns + column_count;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the first column of the reversed matrix.
	 */
	/// @{
	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return columns + column_count;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return columns + column_count;
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the column following the last column of the reversed matrix.
	 */
	/// @{
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return columns;
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return columns;
	}
	/// @}
	
	/// @}
	
	/// @name Capacity
	/// @{
	
	/**
	 * Returns `true` if the matrix is empty, `false` otherwise.
	 */
	[[nodiscard]] inline constexpr bool empty() noexcept
	{
		return element_count;
	};
	
	/// Returns the number of columns in the matrix.
	/// @{
	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return column_count;
	};
	[[nodiscard]] inline constexpr size_type max_size() const noexcept
	{
		return column_count;
	};
	[[nodiscard]] inline constexpr size_type size_columns() const noexcept
	{
		return column_count;
	};
	/// @}
	
	/// Returns the number of rows in the matrix.
	[[nodiscard]] inline constexpr size_type size_rows() const noexcept
	{
		return row_count;
	};
	
	/// Returns the number of elements in the matrix.
	[[nodiscard]] inline constexpr size_type size_elements() const noexcept
	{
		return element_count;
	};
	
	/// @}
	
	/// @name Operations
	/// @{
	
	/**
	 * Fills the matrix with the specified column value.
	 *
	 * @param value Value to assign to each column.
	 */
	inline constexpr void fill(const column_type& value) noexcept
	{
		for (auto& c: columns)
		{
			c = value;
		}
	}
	
	/**
	 * Exchanges the elements of this matrix with the elements of another.
	 *
	 * @param other Matrix with which to exchange elements.
	 */
	[[nodiscard]] inline constexpr void swap(matrix& other) noexcept
	{
		std::swap(columns, other.columns);
	};
	
	/// @}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static consteval column_type identity_column(std::size_t i, std::index_sequence<I...>) noexcept
	{
		return {(I == i ? T{1} : T{0}) ...};
	}
	
	/// @private
	template <std::size_t... I>
	[[nodiscard]] static consteval matrix identity(std::index_sequence<I...>) noexcept
	{
		return {identity_column(I, std::make_index_sequence<row_count>{}) ...};
	}
	
	/**
	 * Returns an identity matrix, with ones on the main diagonal and zeros elsewhere.
	 */
	[[nodiscard]] static consteval matrix identity() noexcept
	{
		return identity(std::make_index_sequence<column_count>{});
	}
};

/**
 * *n* by *m* matrix.
 *
 * @tparam T Element type.
 */
/// @{
template <class T>
using mat2x2 = matrix<T, 2, 2>;
template <class T>
using mat2x3 = matrix<T, 2, 3>;
template <class T>
using mat2x4 = matrix<T, 2, 4>;
template <class T>
using mat3x2 = matrix<T, 3, 2>;
template <class T>
using mat3x3 = matrix<T, 3, 3>;
template <class T>
using mat3x4 = matrix<T, 3, 4>;
template <class T>
using mat4x2 = matrix<T, 4, 2>;
template <class T>
using mat4x3 = matrix<T, 4, 3>;
template <class T>
using mat4x4 = matrix<T, 4, 4>;
/// @}

/**
 * *n* by *n* square matrix.
 *
 * @tparam T Element type.
 */
/// @{
template <class T>
using mat2 = mat2x2<T>;
template <class T>
using mat3 = mat3x3<T>;
template <class T>
using mat4 = mat4x4<T>;
/// @}

/**
 * *n* by *m* matrix of single-precision floating-point numbers.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 */
/// @{
template <std::size_t N, std::size_t M>
using fmat = matrix<float, N, M>;
using fmat2x2 = fmat<2, 2>;
using fmat2x3 = fmat<2, 3>;
using fmat2x4 = fmat<2, 4>;
using fmat3x2 = fmat<3, 2>;
using fmat3x3 = fmat<3, 3>;
using fmat3x4 = fmat<3, 4>;
using fmat4x2 = fmat<4, 2>;
using fmat4x3 = fmat<4, 3>;
using fmat4x4 = fmat<4, 4>;
/// @}

/// *n* by *n* square matrix of single-precision floating-point numbers.
/// @{
using fmat2 = fmat2x2;
using fmat3 = fmat3x3;
using fmat4 = fmat4x4;
/// @}

/**
 * *n* by *m* matrix of double-precision floating-point numbers.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 */
/// @{
template <std::size_t N, std::size_t M>
using dmat = matrix<double, N, M>;
using dmat2x2 = dmat<2, 2>;
using dmat2x3 = dmat<2, 3>;
using dmat2x4 = dmat<2, 4>;
using dmat3x2 = dmat<3, 2>;
using dmat3x3 = dmat<3, 3>;
using dmat3x4 = dmat<3, 4>;
using dmat4x2 = dmat<4, 2>;
using dmat4x3 = dmat<4, 3>;
using dmat4x4 = dmat<4, 4>;
/// @}

/// *n* by *n* square matrix of double-precision floating-point numbers.
/// @{
using dmat2 = dmat2x2;
using dmat3 = dmat3x3;
using dmat4 = dmat4x4;
/// @}

} // namespace types

// Bring math::types into math namespace
using namespace types;

} // namespace math

namespace std
{
	/**
	 * Provides access to the number of columns in a matrix as a compile-time constant expression.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	template<class T, std::size_t N, std::size_t M>
	struct tuple_size<math::matrix<T, N, M>>:
		std::integral_constant<std::size_t, N>
	{};
	
	/**
	 * Provides compile-time indexed access to the type of the columns in a matrix using a tuple-like interface.
	 *
	 * @tparam I Index of a column.
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	template<std::size_t I, class T, std::size_t N, std::size_t M>
	struct tuple_element<I, math::matrix<T, N, M>>
	{
		/// Matrix column type.
		using type = math::matrix<T, N, M>::column_type;
	};
	
	/**
	 * Specialization of std::formatter for math::matrix.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
    template <class T, std::size_t N, std::size_t M>
    struct formatter<math::matrix<T, N, M>>: formatter<math::vector<T, M>>
    {
		auto format(const math::matrix<T, N, M>& t, format_context& fc) const
		{
			auto&& out = fc.out();
			format_to(out, "{{");
			
			if constexpr (N)
			{
				formatter<math::vector<T, M>>::format(t[0], fc);
			}
			
			for (std::size_t i = 1; i < N; ++i)
			{
				format_to(out, ", ");
				formatter<math::vector<T, M>>::format(t[i], fc);
			}
			
			return format_to(out, "}}");
		}
    };
}

// Ensure matrices are POD types
static_assert(std::is_standard_layout_v<math::fmat3>);
static_assert(std::is_trivial_v<math::fmat3>);

#endif // ANTKEEPER_MATH_MATRIX_TYPES_HPP
