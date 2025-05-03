// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.simd.vector:type;
import engine.utility.sized_types;
export import <span>;
import <concepts>;
import <utility>;

export namespace engine::math::simd
{
	/// Selects the appropriate SIMD vector register type and alignment for the given element type and number of elements.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	template <class T, usize N>
	struct vector_register;

	/// Helper type alias for selecting a SIMD vector register type.
	template <class T, usize N>
	using vector_register_t = typename vector_register<T, N>::type;
	
	/// Helper type alias for selecting a SIMD vector alignment value.
	template <typename T, usize N>
	inline constexpr usize vector_alignment_v = vector_register<T, N>::alignment;
}

export namespace engine::math::simd::inline types
{
	/// @name SIMD vector types
	/// @{

	/// *n*-dimensional SIMD vector.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	template <class T, usize N>
		requires (std::same_as<T, float> && N == 4)
	struct vector
	{
		using element_type = T;
		using register_type = vector_register_t<T, N>;
		using span = std::span<element_type, N>;
		using const_span = std::span<const element_type, N>;

		/// Number of elements.
		static inline constexpr usize size = N;

		/// Byte boundary on which load/store operations are aligned.
		static inline constexpr usize alignment = vector_alignment_v<T, N>;

		/// @private
		register_type m_data{};

		/// @name Constructors
		/// @{
		
		/// Default constructor.
		constexpr vector() noexcept = default;

		/// Constructs a vector from a SIMD register.
		/// @param data SIMD register.
		inline explicit vector(register_type&& data) noexcept
			: m_data{std::move(data)}
		{
		}

		/// Constructs a vector from an array of elements.
		/// @param elements Array of elements to load.
		/// @warning Element array must be aligned on a `vector::alignment`-byte boundary with `alignas(vector::alignment)`.
		inline explicit vector(const_span elements)
		{
			load(std::move(elements));
		}

		/// Constructs a vector from individual element values.
		/// @tparam Args Parameter pack of element types.
		/// @param args Parameter pack of element values.
		template <std::convertible_to<element_type>... Args>
			requires (sizeof...(Args) == N)
		inline vector(Args&&... args)
		{
			alignas(alignment) const element_type elements[N]{std::forward<Args>(args)...};
			load(elements);
		}

		/// Constructs a vector by broadcasting a single value to all elements.
		/// @param value Value to broadcast.
		explicit vector(element_type value) noexcept;

		/// @}

		/// @name Loading and storing
		/// @{
		
		/// Loads vector elements from an array.
		/// @param elements Array of elements to load.
		/// @warning Element array must be aligned on a `vector::alignment`-byte boundary with `alignas(vector::alignment)`.
		void load(const_span elements);

		/// Loads vector elements from an array.
		/// @param elements Array of elements to load.
		void load_unaligned(const_span elements) noexcept;

		/// Stores the vector elements into an array.
		/// @param[out] elements Array into which elements will be stored.
		/// @warning Element array must be aligned on a `vector::alignment`-byte boundary with `alignas(vector::alignment)`.
		void store(span elements);

		/// Stores the vector elements into an array.
		/// @param[out] elements Array into which elements will be stored.
		void store_unaligned(span elements) noexcept;

		/// @}

		/// @name Subscript operators
		/// @{

		/// Returns the element at the specified index.
		/// @param i Index of an element.
		/// @return Element at the specified index.
		[[nodiscard]] const element_type operator[](usize i) const noexcept;

		/// @}

		/// @name Unary operators
		/// @{

		/// Increments each element of the vector by 1.
		/// @return Reference to the incremented vector.
		vector& operator++() noexcept;

		/// Increments each element of the vector by 1.
		/// @return Copy of the vector before it was incremented.
		[[nodiscard]] vector operator++(int) noexcept;

		/// Decrements each element of the vector by 1.
		/// @return Reference to the decremented vector.
		vector& operator--() noexcept;

		/// Decrements each element of the vector by 1.
		/// @return Copy of the vector before it was decremented.
		[[nodiscard]] vector operator--(int) noexcept;

		/// Returns a copy of the vector with negated elements.
		/// @return Copy of the vector with negated elements.
		[[nodiscard]] vector operator-() const noexcept;

		/// @}

		/// @name Binary operators
		/// @{

		/// Adds two vectors.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the addition.
		friend vector operator+(const vector& lhs, const vector& rhs) noexcept;

		/// Adds a scalar to all elements of a vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Result of the addition.
		friend vector operator+(const vector& lhs, element_type rhs) noexcept;

		/// Adds a scalar to all elements of a vector.
		/// @param lhs Scalar on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the addition.
		friend vector operator+(element_type lhs, const vector& rhs) noexcept;

		/// Subtracts two vectors.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the subtraction.
		friend vector operator-(const vector& lhs, const vector& rhs) noexcept;

		/// Subtracts a scalar from all elements of a vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Result of the subtraction.
		friend vector operator-(const vector& lhs, element_type rhs) noexcept;

		/// Subtracts each element of a vector from a scalar.
		/// @param lhs Scalar on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the subtraction.
		friend vector operator-(element_type lhs, const vector& rhs) noexcept;

		/// Multiplies the elements of two vectors.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the multiplication.
		friend vector operator*(const vector& lhs, const vector& rhs) noexcept;

		/// Multiplies each element of a vector by a scalar.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Result of the multiplication.
		friend vector operator*(const vector& lhs, element_type rhs) noexcept;

		/// Multiplies each element of a vector by a scalar.
		/// @param lhs Scalar on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the multiplication.
		friend vector operator*(element_type lhs, const vector& rhs) noexcept;

		/// Divides the elements of one vector by the elements of another vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the division.
		friend vector operator/(const vector& lhs, const vector& rhs) noexcept;

		/// Divides each element of a vector by a scalar.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Result of the division.
		friend vector operator/(const vector& lhs, element_type rhs) noexcept;

		/// Divides a scalar by each element of a vector.
		/// @param lhs Scalar on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Result of the division.
		friend vector operator/(element_type lhs, const vector& rhs) noexcept;

		/// @}

		/// @name Compound assignment operators
		/// @{

		/// Adds the elements of two vectors and stores the result in the first vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator+=(vector& lhs, const vector& rhs) noexcept;

		/// Adds a scalar to each element of a vector and stores the result in the vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator+=(vector& lhs, element_type rhs) noexcept;

		/// Subtracts the elements of two vectors and stores the result in the first vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator-=(vector& lhs, const vector& rhs) noexcept;

		/// Subtracts a scalar from each element of a vector and stores the result in the vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator-=(vector& lhs, element_type rhs) noexcept;

		/// Multiplies the elements of two vectors and stores the result in the first vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator*=(vector& lhs, const vector& rhs) noexcept;

		/// Multiplies each element of a vector by a scalar and stores the result in the vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator*=(vector& lhs, element_type rhs) noexcept;

		/// Divides the elements of one vector by the elements of another vector and stores the result in the first vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Vector on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator/=(vector& lhs, const vector& rhs) noexcept;

		/// Divides each element of a vector by a scalar and stores the result in the vector.
		/// @param lhs Vector on the left-hand side.
		/// @param rhs Scalar on the right-hand side.
		/// @return Reference to the vector on the left-hand side.
		friend vector& operator/=(vector& lhs, element_type rhs) noexcept;

		/// @}
	};

	template <class T, usize N>
	using vec = vector<T, N>;

	template <class T>
	using vec4 = vec<T, 4>;

	template <usize N>
	using fvec = vec<float, N>;

	using fvec4 = fvec<4>;
}
