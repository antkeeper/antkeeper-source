// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>
#include <format>
#include <iterator>
#include <type_traits>
#include <utility>

namespace engine::math::inline types
{
	/// @name Matrix types
	/// @relates matrix
	/// @{

	/// *n* by *m* column-major matrix.
	/// @tparam T Element type.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	/// @see https://en.wikipedia.org/wiki/Row-_and_column-major_order
	template <class T, usize N, usize M>
	struct matrix
	{
		/// @name Member types
		/// @{
		using element_type = T;
		using column_type = vector<T, M>;
		using row_type = vector<T, N>;
		using value_type = column_type;
		using size_type = usize;
		using difference_type = isize;
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
		template <class U, usize... I>
		[[nodiscard]] inline constexpr matrix<U, N, M> type_cast(std::index_sequence<I...>) const noexcept
		{
			return {vector<U, M>(columns[I])...};
		}
	
		/// Type-casts the elements of this matrix using `static_cast`.
		/// @tparam U Target element type.
		/// @return Matrix containing the type-casted elements.
		template <class U>
		[[nodiscard]] inline constexpr explicit operator matrix<U, N, M>() const noexcept
		{
			return type_cast<U>(std::make_index_sequence<N>{});
		}
	
		/// @private
		template <usize... I>
		[[nodiscard]] static consteval column_type identity_column(usize i, std::index_sequence<I...>)
		{
			return {(I == i ? T{1} : T{0}) ...};
		}
	
		/// @private
		template <usize... I>
		[[nodiscard]] static consteval matrix identity(std::index_sequence<I...>)
		{
			return {identity_column(I, std::make_index_sequence<row_count>{}) ...};
		}
	
		/// @private
		[[nodiscard]] static consteval matrix identity()
		{
			return identity(std::make_index_sequence<column_count>{});
		}
	
		/// @private
		template <usize P, usize O, usize... I>
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
	
		/// Size-casts this matrix to a matrix with different dimensions. Casting to greater dimensions causes new elements to be set to identity matrix elements.
		/// @tparam P Target number of columns.
		/// @tparam O Target number of rows.
		/// @return *p* by *o* matrix.
		template <usize P, usize O>
		[[nodiscard]] inline constexpr explicit operator matrix<T, P, O>() const noexcept
		{
			return size_cast<P, O>(std::make_index_sequence<P>{});
		}
	
		/// @}
	
		/// @name Column access
		/// @{
	
		/// Returns a reference to the *i*-th column in the matrix.
		/// @param i Index of a column.
		/// @return Reference to the *i*-th column in the matrix.
		[[nodiscard]] inline constexpr reference operator[](size_type i) noexcept
		{
			return columns[i];
		}

		/// @copydoc operator[](size_type)
		[[nodiscard]] inline constexpr const_reference operator[](size_type i) const noexcept
		{
			return columns[i];
		}

		/// @copydoc operator[](size_type)
		[[nodiscard]] inline constexpr column_type& column(size_type i) noexcept
		{
			return columns[i];
		}

		/// @copydoc operator[](size_type)
		[[nodiscard]] inline constexpr const column_type& column(size_type i) const noexcept
		{
			return columns[i];
		}
	
		/// Returns a reference to the first column in the matrix.
		[[nodiscard]] inline constexpr reference front() noexcept
		{
			return columns[0];
		}

		/// @copydoc front()
		[[nodiscard]] inline constexpr const_reference front() const noexcept
		{
			return columns[0];
		}
	
		/// Returns a reference to the last column in the matrix.
		[[nodiscard]] inline constexpr reference back() noexcept
		{
			return columns[column_count - 1];
		}

		/// @copydoc back()
		[[nodiscard]] inline constexpr const_reference back() const noexcept
		{
			return columns[column_count - 1];
		}
	
		/// Returns a pointer to the first column in the matrix's column array.
		[[nodiscard]] inline constexpr pointer data() noexcept
		{
			return columns;
		};

		/// @copydoc data()
		[[nodiscard]] inline constexpr const_pointer data() const noexcept
		{
			return columns;
		};
	
		/// @}
	
		/// @name Element access
		/// @{
	
		/// Returns a reference to the *i*-th element in the matrix.
		/// @param i Column-major index of an element.
		/// @return Reference to the *i*-th element in the matrix.
		[[nodiscard]] inline constexpr element_type& element(size_type i) noexcept
		{
			return columns[i / row_count][i % row_count];
		}

		/// @copydoc element(size_type)
		[[nodiscard]] inline constexpr const element_type& element(size_type i) const noexcept
		{
			return columns[i / row_count][i % row_count];
		}
	
		/// @}
	
		/// @name Iterators
		/// @{
	
		/// Returns an iterator to the first column in the matrix.
		[[nodiscard]] inline constexpr iterator begin() noexcept
		{
			return columns;
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator begin() const noexcept
		{
			return columns;
		}

		/// @copydoc begin()
		[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
		{
			return columns;
		}
	
		/// Returns an iterator to the column following the last column in the matrix.
		[[nodiscard]] inline constexpr iterator end() noexcept
		{
			return columns + column_count;
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator end() const noexcept
		{
			return columns + column_count;
		}

		/// @copydoc end()
		[[nodiscard]] inline constexpr const_iterator cend() const noexcept
		{
			return columns + column_count;
		}
	
		/// Returns a reverse iterator to the first column in the reversed matrix.
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
	
		/// Returns a reverse iterator to the column following the last column in the reversed matrix.
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
	
		/// Returns `true` if the matrix has no columns, `false` otherwise.
		[[nodiscard]] static consteval bool empty()
		{
			return !column_count;
		};
	
		/// Returns the number of columns in the matrix.
		[[nodiscard]] static consteval size_type size()
		{
			return column_count;
		};
	
		/// Returns the number of columns in the matrix.
		[[nodiscard]] static consteval size_type max_size()
		{
			return column_count;
		};
	
		/// Returns the number of columns in the matrix.
		[[nodiscard]] static consteval size_type size_columns()
		{
			return column_count;
		};
	
		/// Returns the number of rows in the matrix.
		[[nodiscard]] static consteval size_type size_rows()
		{
			return row_count;
		};
	
		/// Returns the number of elements in the matrix.
		[[nodiscard]] static consteval size_type size_elements()
		{
			return element_count;
		};
	
		/// @}
	
		/// @name Operations
		/// @{
	
		/// Fills the matrix with the specified column value.
		/// @param value Value to assign to each column in the matrix.
		inline constexpr void fill(const column_type& value) noexcept
		{
			for (auto& c: columns)
			{
				c = value;
			}
		}
	
		/// Fills the matrix with the specified element value.
		/// @param value Value to assign to each element in the matrix.
		inline constexpr void fill(const element_type& value) noexcept
		{
			for (auto& c: columns)
			{
				c.fill(value);
			}
		}
	
		/// Exchanges the elements of this matrix with the elements of another.
		/// @param other Matrix with which to exchange elements.
		inline constexpr void swap(matrix& other) noexcept
		{
			std::swap(columns, other.columns);
		};
	
		/// @}
	
		/// @name Comparison
		/// @{
	
		/// Tests two matrices for equality.
		/// @return `true` if the two matrices are equivalent, `false` otherwise.
		[[nodiscard]] inline constexpr friend bool operator==(const matrix&, const matrix&) noexcept = default;
	
		/// Compares the columns of two matrices lexicographically.
		/// @return Lexicographical ordering of the two matrices.
		[[nodiscard]] inline constexpr friend auto operator<=>(const matrix&, const matrix&) noexcept = default; 
	
		/// @}
	};

	/// @copybrief matrix
	template <class T, usize N, usize M>
	using mat = matrix<T, N, M>;

	/// 2x2 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat2x2 = mat<T, 2, 2>;

	/// 2x3 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat2x3 = mat<T, 2, 3>;

	/// 2x4 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat2x4 = mat<T, 2, 4>;

	/// 3x2 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat3x2 = mat<T, 3, 2>;

	/// 3x3 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat3x3 = mat<T, 3, 3>;

	/// 3x4 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat3x4 = mat<T, 3, 4>;

	/// 4x2 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat4x2 = mat<T, 4, 2>;

	/// 4x3 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat4x3 = mat<T, 4, 3>;

	/// 4x4 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat4x4 = mat<T, 4, 4>;

	/// 2x2 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat2 = mat2x2<T>;

	/// 3x3 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat3 = mat3x3<T>;

	/// 4x4 matrix.
	/// @tparam T Element type.
	template <class T>
	using mat4 = mat4x4<T>;

	/// *n* by *m* matrix of Boolean values.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	template <usize N, usize M>
	using bmat = mat<bool, N, M>;

	/// 2x2 matrix of Boolean values.
	using bmat2x2 = bmat<2, 2>;

	/// 2x3 matrix of Boolean values.
	using bmat2x3 = bmat<2, 3>;

	/// 2x4 matrix of Boolean values.
	using bmat2x4 = bmat<2, 4>;

	/// 3x2 matrix of Boolean values.
	using bmat3x2 = bmat<3, 2>;

	/// 3x3 matrix of Boolean values.
	using bmat3x3 = bmat<3, 3>;

	/// 3x4 matrix of Boolean values.
	using bmat3x4 = bmat<3, 4>;

	/// 4x2 matrix of Boolean values.
	using bmat4x2 = bmat<4, 2>;

	/// 4x3 matrix of Boolean values.
	using bmat4x3 = bmat<4, 3>;

	/// 4x4 matrix of Boolean values.
	using bmat4x4 = bmat<4, 4>;

	/// 2x2 matrix of Boolean values.
	using bmat2 = bmat2x2;

	/// 3x3 matrix of Boolean values.
	using bmat3 = bmat3x3;

	/// 4x4 matrix of Boolean values.
	using bmat4 = bmat4x4;

	/// *n* by *m* matrix of signed integer values.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	template <usize N, usize M>
	using imat = mat<int, N, M>;

	/// 2x2 matrix of signed integer values.
	using imat2x2 = imat<2, 2>;

	/// 2x3 matrix of signed integer values.
	using imat2x3 = imat<2, 3>;

	/// 2x4 matrix of signed integer values.
	using imat2x4 = imat<2, 4>;

	/// 3x2 matrix of signed integer values.
	using imat3x2 = imat<3, 2>;

	/// 3x3 matrix of signed integer values.
	using imat3x3 = imat<3, 3>;

	/// 3x4 matrix of signed integer values.
	using imat3x4 = imat<3, 4>;

	/// 4x2 matrix of signed integer values.
	using imat4x2 = imat<4, 2>;

	/// 4x3 matrix of signed integer values.
	using imat4x3 = imat<4, 3>;

	/// 4x4 matrix of signed integer values.
	using imat4x4 = imat<4, 4>;

	/// 2x2 matrix of signed integer values.
	using imat2 = imat2x2;

	/// 3x3 matrix of signed integer values.
	using imat3 = imat3x3;

	/// 4x4 matrix of signed integer values.
	using imat4 = imat4x4;

	/// *n* by *m* matrix of unsigned integer values.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	template <usize N, usize M>
	using umat = mat<unsigned int, N, M>;

	/// 2x2 matrix of unsigned integer values.
	using umat2x2 = umat<2, 2>;

	/// 2x3 matrix of unsigned integer values.
	using umat2x3 = umat<2, 3>;

	/// 2x4 matrix of unsigned integer values.
	using umat2x4 = umat<2, 4>;

	/// 3x2 matrix of unsigned integer values.
	using umat3x2 = umat<3, 2>;

	/// 3x3 matrix of unsigned integer values.
	using umat3x3 = umat<3, 3>;

	/// 3x4 matrix of unsigned integer values.
	using umat3x4 = umat<3, 4>;

	/// 4x2 matrix of unsigned integer values.
	using umat4x2 = umat<4, 2>;

	/// 4x3 matrix of unsigned integer values.
	using umat4x3 = umat<4, 3>;

	/// 4x4 matrix of unsigned integer values.
	using umat4x4 = umat<4, 4>;

	/// 2x2 matrix of unsigned integer values.
	using umat2 = umat2x2;

	/// 3x3 matrix of unsigned integer values.
	using umat3 = umat3x3;

	/// 4x4 matrix of unsigned integer values.
	using umat4 = umat4x4;

	/// *n* by *m* matrix of single-precision floating-point values.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	template <usize N, usize M>
	using fmat = mat<float, N, M>;

	/// 2x2 matrix of single-precision floating-point values.
	using fmat2x2 = fmat<2, 2>;

	/// 2x3 matrix of single-precision floating-point values.
	using fmat2x3 = fmat<2, 3>;

	/// 2x4 matrix of single-precision floating-point values.
	using fmat2x4 = fmat<2, 4>;

	/// 3x2 matrix of single-precision floating-point values.
	using fmat3x2 = fmat<3, 2>;

	/// 3x3 matrix of single-precision floating-point values.
	using fmat3x3 = fmat<3, 3>;

	/// 3x4 matrix of single-precision floating-point values.
	using fmat3x4 = fmat<3, 4>;

	/// 4x2 matrix of single-precision floating-point values.
	using fmat4x2 = fmat<4, 2>;

	/// 4x3 matrix of single-precision floating-point values.
	using fmat4x3 = fmat<4, 3>;

	/// 4x4 matrix of single-precision floating-point values.
	using fmat4x4 = fmat<4, 4>;

	/// 2x2 matrix of single-precision floating-point values.
	using fmat2 = fmat2x2;

	/// 3x3 matrix of single-precision floating-point values.
	using fmat3 = fmat3x3;

	/// 4x4 matrix of single-precision floating-point values.
	using fmat4 = fmat4x4;

	/// *n* by *m* matrix of double-precision floating-point values.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	template <usize N, usize M>
	using dmat = mat<double, N, M>;

	/// 2x2 matrix of double-precision floating-point values.
	using dmat2x2 = dmat<2, 2>;

	/// 2x3 matrix of double-precision floating-point values.
	using dmat2x3 = dmat<2, 3>;

	/// 2x4 matrix of double-precision floating-point values.
	using dmat2x4 = dmat<2, 4>;

	/// 3x2 matrix of double-precision floating-point values.
	using dmat3x2 = dmat<3, 2>;

	/// 3x3 matrix of double-precision floating-point values.
	using dmat3x3 = dmat<3, 3>;

	/// 3x4 matrix of double-precision floating-point values.
	using dmat3x4 = dmat<3, 4>;

	/// 4x2 matrix of double-precision floating-point values.
	using dmat4x2 = dmat<4, 2>;

	/// 4x3 matrix of double-precision floating-point values.
	using dmat4x3 = dmat<4, 3>;

	/// 4x4 matrix of double-precision floating-point values.
	using dmat4x4 = dmat<4, 4>;

	/// 2x2 matrix of double-precision floating-point values.
	using dmat2 = dmat2x2;

	/// 3x3 matrix of double-precision floating-point values.
	using dmat3 = dmat3x3;

	/// 4x4 matrix of double-precision floating-point values.
	using dmat4 = dmat4x4;

	/// @}

	/// @name Tuple-like interface
	/// @{

	/// Extracts the *i*-th column from a matrix using a tuple-like interface.
	/// @tparam I Index of a column.
	/// @tparam T Element type.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	/// @param m Matrix from which to extract a column.
	/// @return Reference to the *i*-th column of @p m.
	/// @relates matrix
	template<usize I, class T, usize N, usize M>
	[[nodiscard]] inline constexpr typename matrix<T, N, M>::column_type& get(matrix<T, N, M>& m) noexcept
	{
		static_assert(I < N);
		return m[I];
	}

	/// @copydoc get(matrix<T, N, M>&)
	/// @relates matrix
	template<usize I, class T, usize N, usize M>
	[[nodiscard]] inline constexpr typename matrix<T, N, M>::column_type&& get(matrix<T, N, M>&& m) noexcept
	{
		static_assert(I < N);
		return std::move(m[I]);
	}

	/// @copydoc get(matrix<T, N, M>&)
	/// @relates matrix
	template<usize I, class T, usize N, usize M>
	[[nodiscard]] inline constexpr const typename matrix<T, N, M>::column_type& get(const matrix<T, N, M>& m) noexcept
	{
		static_assert(I < N);
		return m[I];
	}

	/// @copydoc get(matrix<T, N, M>&)
	/// @relates matrix
	template<usize I, class T, usize N, usize M>
	[[nodiscard]] inline constexpr const typename matrix<T, N, M>::column_type&& get(const matrix<T, N, M>&& m) noexcept
	{
		static_assert(I < N);
		return std::move(m[I]);
	}

	/// @}
}

namespace std
{
	/// Provides access to the number of columns in a engine::math::matrix as a compile-time constant expression.
	/// @tparam T Element type.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	/// @relates engine::math::matrix
	template <class T, engine::usize N, engine::usize M>
	struct tuple_size<engine::math::matrix<T, N, M>>:
		std::integral_constant<engine::usize, N>
	{};
	
	/// Provides compile-time indexed access to the type of the columns in a engine::math::matrix using a tuple-like interface.
	/// @tparam I Index of a column.
	/// @tparam T Element type.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	/// @relates engine::math::matrix
	template <engine::usize I, class T, engine::usize N, engine::usize M>
	struct tuple_element<I, engine::math::matrix<T, N, M>>
	{
		/// Matrix column type.
		using type = engine::math::matrix<T, N, M>::column_type;
	};
	
	/// Specialization of std::formatter for engine::math::matrix.
	/// @tparam T Element type.
	/// @tparam N Number of columns.
	/// @tparam M Number of rows.
	/// @relates engine::math::matrix
    template <class T, engine::usize N, engine::usize M>
    struct formatter<engine::math::matrix<T, N, M>>: formatter<engine::math::vector<T, M>>
    {
		auto format(const engine::math::matrix<T, N, M>& t, format_context& fc) const
		{
			auto&& out = fc.out();
			format_to(out, "{{");
			
			if constexpr (N)
			{
				formatter<engine::math::vector<T, M>>::format(t[0], fc);
			}
			
			for (engine::usize i = 1; i < N; ++i)
			{
				format_to(out, ", ");
				formatter<engine::math::vector<T, M>>::format(t[i], fc);
			}
			
			return format_to(out, "}}");
		}
    };
}
