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
	
	/** Number of columns. */
	static inline constexpr size_type column_count = N;
	
	/** Number of rows. */
	static inline constexpr size_type row_count = M;
	
	/** Number of elements. */
	static inline constexpr size_type element_count = column_count * row_count;
	
	/// @name Data members
	/// @{
	
	/** Matrix columns. */
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
	
	/// @private
	[[nodiscard]] static consteval matrix identity() noexcept
	{
		return identity(std::make_index_sequence<column_count>{});
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
	
	/// @{
	/**
	 * Returns a reference to the *i*-th column in the matrix.
	 *
	 * @param i Index of a column.
	 *
	 * @return Reference to the *i*-th column in the matrix.
	 */
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
	
	/// @{
	/** Returns a reference to the first column in the matrix. */
	[[nodiscard]] inline constexpr reference front() noexcept
	{
		return columns[0];
	}
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		return columns[0];
	}
	/// @}
	
	/// @{
	/** Returns a reference to the last column in the matrix. */
	[[nodiscard]] inline constexpr reference back() noexcept
	{
		return columns[column_count - 1];
	}
	[[nodiscard]] inline constexpr const_reference back() const noexcept
	{
		return columns[column_count - 1];
	}
	/// @}
	
	/// @{
	/** Returns a pointer to the first column in the matrix's column array. */
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
	
	/// @name Element access
	/// @{
	
	/// @{
	/**
	 * Returns a reference to the *i*-th element in the matrix.
	 *
	 * @param i Column-major index of an element.
	 *
	 * @return Reference to the *i*-th element in the matrix.
	 */
	[[nodiscard]] inline constexpr element_type& element(size_type i) noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	[[nodiscard]] inline constexpr const element_type& element(size_type i) const noexcept
	{
		return columns[i / row_count][i % row_count];
	}
	/// @}
	
	/// @}
	
	/// @name Iterators
	/// @{
	
	/// @{
	/** Returns an iterator to the first column in the matrix. */
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
	
	/// @{
	/** Returns an iterator to the column following the last column in the matrix. */
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
	
	/// @{
	/** Returns a reverse iterator to the first column in the reversed matrix. */
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
	
	/// @{
	/** Returns a reverse iterator to the column following the last column in the reversed matrix. */
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
	
	/** Returns `true` if the matrix is empty, `false` otherwise. */
	[[nodiscard]] inline constexpr bool empty() noexcept
	{
		return element_count;
	};
	
	/** Returns the number of columns in the matrix. */
	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return column_count;
	};
	
	/** Returns the number of columns in the matrix. */
	[[nodiscard]] inline constexpr size_type max_size() const noexcept
	{
		return column_count;
	};
	
	/** Returns the number of columns in the matrix. */
	[[nodiscard]] inline constexpr size_type size_columns() const noexcept
	{
		return column_count;
	};
	
	/** Returns the number of rows in the matrix. */
	[[nodiscard]] inline constexpr size_type size_rows() const noexcept
	{
		return row_count;
	};
	
	/** Returns the number of elements in the matrix. */
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
	 * @param value Value to assign to each column in the matrix.
	 */
	inline constexpr void fill(const column_type& value) noexcept
	{
		for (auto& c: columns)
		{
			c = value;
		}
	}
	
	/**
	 * Fills the matrix with the specified element value.
	 *
	 * @param value Value to assign to each element in the matrix.
	 */
	inline constexpr void fill(const element_type& value) noexcept
	{
		for (auto& c: columns)
		{
			c.fill(value);
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
};

/// @name Tuple-like interface
/// @{

/// @{
/**
 * Extracts the *i*-th column from a matrix using a tuple-like interface.
 *
 * @tparam I Index of a column.
 * @tparam T Element type.
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 *
 * @param m Matrix from which to extract a column.
 *
 * @return Reference to the *i*-th column of @p m.
 *
 * @relates matrix
 */
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr typename matrix<T, N, M>::column_type& get(matrix<T, N, M>& m) noexcept
{
	static_assert(I < N);
	return m[I];
}

/// @relates matrix
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr typename matrix<T, N, M>::column_type&& get(matrix<T, N, M>&& m) noexcept
{
	static_assert(I < N);
	return std::move(m[I]);
}

/// @relates matrix
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr const typename matrix<T, N, M>::column_type& get(const matrix<T, N, M>& m) noexcept
{
	static_assert(I < N);
	return m[I];
}

/// @relates matrix
template<std::size_t I, class T, std::size_t N, std::size_t M>
[[nodiscard]] inline constexpr const typename matrix<T, N, M>::column_type&& get(const matrix<T, N, M>&& m) noexcept
{
	static_assert(I < N);
	return std::move(m[I]);
}
/// @}

/// @}

/// @name Matrix types
/// @relates matrix
/// @{

/// @copydoc matrix
template <class T, std::size_t N, std::size_t M>
using mat = matrix<T, N, M>;
 
/**
 * 2x2 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat2x2 = mat<T, 2, 2>;

/**
 * 2x3 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat2x3 = mat<T, 2, 3>;

/**
 * 2x4 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat2x4 = mat<T, 2, 4>;

/**
 * 3x2 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat3x2 = mat<T, 3, 2>;

/**
 * 3x3 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat3x3 = mat<T, 3, 3>;

/**
 * 3x4 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat3x4 = mat<T, 3, 4>;

/**
 * 4x2 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat4x2 = mat<T, 4, 2>;

/**
 * 4x3 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat4x3 = mat<T, 4, 3>;

/**
 * 4x4 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat4x4 = mat<T, 4, 4>;

/**
 * 2x2 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat2 = mat2x2<T>;

/**
 * 3x3 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat3 = mat3x3<T>;

/**
 * 4x4 matrix.
 *
 * @tparam T Element type.
 */
template <class T>
using mat4 = mat4x4<T>;

/**
 * *n* by *m* matrix of single-precision floating-point values.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 */
template <std::size_t N, std::size_t M>
using fmat = mat<float, N, M>;

/** 2x2 matrix of single-precision floating-point values. */
using fmat2x2 = fmat<2, 2>;

/** 2x3 matrix of single-precision floating-point values. */
using fmat2x3 = fmat<2, 3>;

/** 2x4 matrix of single-precision floating-point values. */
using fmat2x4 = fmat<2, 4>;

/** 3x2 matrix of single-precision floating-point values. */
using fmat3x2 = fmat<3, 2>;

/** 3x3 matrix of single-precision floating-point values. */
using fmat3x3 = fmat<3, 3>;

/** 3x4 matrix of single-precision floating-point values. */
using fmat3x4 = fmat<3, 4>;

/** 4x2 matrix of single-precision floating-point values. */
using fmat4x2 = fmat<4, 2>;

/** 4x3 matrix of single-precision floating-point values. */
using fmat4x3 = fmat<4, 3>;

/** 4x4 matrix of single-precision floating-point values. */
using fmat4x4 = fmat<4, 4>;

/** 2x2 matrix of single-precision floating-point values. */
using fmat2 = fmat2x2;

/** 3x3 matrix of single-precision floating-point values. */
using fmat3 = fmat3x3;

/** 4x4 matrix of single-precision floating-point values. */
using fmat4 = fmat4x4;

/**
 * *n* by *m* matrix of double-precision floating-point values.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 */
template <std::size_t N, std::size_t M>
using dmat = mat<double, N, M>;

/** 2x2 matrix of double-precision floating-point values. */
using dmat2x2 = dmat<2, 2>;

/** 2x3 matrix of double-precision floating-point values. */
using dmat2x3 = dmat<2, 3>;

/** 2x4 matrix of double-precision floating-point values. */
using dmat2x4 = dmat<2, 4>;

/** 3x2 matrix of double-precision floating-point values. */
using dmat3x2 = dmat<3, 2>;

/** 3x3 matrix of double-precision floating-point values. */
using dmat3x3 = dmat<3, 3>;

/** 3x4 matrix of double-precision floating-point values. */
using dmat3x4 = dmat<3, 4>;

/** 4x2 matrix of double-precision floating-point values. */
using dmat4x2 = dmat<4, 2>;

/** 4x3 matrix of double-precision floating-point values. */
using dmat4x3 = dmat<4, 3>;

/** 4x4 matrix of double-precision floating-point values. */
using dmat4x4 = dmat<4, 4>;

/** 2x2 matrix of double-precision floating-point values. */
using dmat2 = dmat2x2;

/** 3x3 matrix of double-precision floating-point values. */
using dmat3 = dmat3x3;

/** 4x4 matrix of double-precision floating-point values. */
using dmat4 = dmat4x4;

/// @}

} // namespace types

// Bring math::types into math namespace
using namespace types;

} // namespace math

namespace std
{
	/**
	 * Provides access to the number of columns in a math::types::matrix as a compile-time constant expression.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 *
	 * @relates math::types::matrix
	 */
	template <class T, std::size_t N, std::size_t M>
	struct tuple_size<math::matrix<T, N, M>>:
		std::integral_constant<std::size_t, N>
	{};
	
	/**
	 * Provides compile-time indexed access to the type of the columns in a math::types::matrix using a tuple-like interface.
	 *
	 * @tparam I Index of a column.
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 *
	 * @relates math::types::matrix
	 */
	template <std::size_t I, class T, std::size_t N, std::size_t M>
	struct tuple_element<I, math::matrix<T, N, M>>
	{
		/** Matrix column type. */
		using type = math::matrix<T, N, M>::column_type;
	};
	
	/**
	 * Specialization of std::formatter for math::types::matrix.
	 *
	 * @tparam T Element type.
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 *
	 * @relates math::types::matrix
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
