// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <span>
#include <concepts>
#include <utility>
#include <nmmintrin.h>

namespace engine::math::simd
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

namespace engine::math::simd::inline types
{
	/// @name SIMD vector types
	/// @{

	/// *n*-dimensional SIMD vector.
	/// @tparam T Element type.
	/// @tparam N Number of elements.
	template <class T, usize N>
		requires (std::same_as<T, float> && (N == 3 || N == 4))
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

		/// Mask with the lowest `N` bits set to 1 for use with masked intrinsics.
		static inline constexpr __mmask8 zero_mask = (1u << N) - 1;

		/// Dot product mask for use with `_mm_dp_ps`.
		static inline constexpr int dot_mask = (zero_mask << 4) | 1;

		/// @private
		register_type m_data;

		/// @name Constructors
		/// @{
		
		/// Default constructor.
		inline constexpr vector() noexcept = default;

		/// Constructs a vector from a SIMD register.
		/// @param data SIMD register.
		inline constexpr explicit vector(register_type&& data) noexcept
			: m_data{std::move(data)}
		{
		}

		/// Constructs a vector from an array of elements.
		/// @param elements Array of elements to load.
		/// @warning Element array must be aligned on a `vector::alignment`-byte boundary with `alignas(vector::alignment)`.
		inline explicit vector(const_span elements) noexcept
		{
			load(std::move(elements));
		}

		/// Constructs a vector from individual element values.
		/// @tparam Args Parameter pack of element types.
		/// @param args Parameter pack of element values.
		/// @todo Find a way to separate declaration and definition for fvec3 and fvec4 specializations.
		template <std::convertible_to<element_type>... Args>
			requires (sizeof...(Args) == N)
		inline vector(Args&&... args) noexcept
		{
			if constexpr (std::is_same_v<element_type, float>)
			{
				if constexpr (N == 4)
				{
					m_data = _mm_setr_ps(static_cast<element_type>(std::forward<Args>(args))...);
				}
				else if constexpr (N == 3)
				{
					m_data = _mm_setr_ps(static_cast<element_type>(std::forward<Args>(args))..., {});
				}
			}
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
		void load(const_span elements) noexcept;

		/// Loads vector elements from an array.
		/// @param elements Array of elements to load.
		void load_unaligned(const_span elements) noexcept;

		/// Stores the vector elements into an array.
		/// @param[out] elements Array into which elements will be stored.
		/// @warning Element array must be aligned on a `vector::alignment`-byte boundary with `alignas(vector::alignment)`.
		void store(span elements) const noexcept;

		/// Stores the vector elements into an array.
		/// @param[out] elements Array into which elements will be stored.
		void store_unaligned(span elements) const noexcept;

		/// @}

		/// @name Subscript operators
		/// @{

		/// Returns the element at the specified index.
		/// @param i Index of an element.
		/// @return Element at the specified index.
		[[nodiscard]] const element_type operator[](usize i) const noexcept;

		/// @}
	};

	/// @copybrief vector
	template <class T, usize N>
	using vec = vector<T, N>;

	/// 3-dimensional SIMD vector.
	/// @tparam T Element type.
	template <class T>
	using vec3 = vec<T, 3>;

	/// 4-dimensional SIMD vector.
	/// @tparam T Element type.
	template <class T>
	using vec4 = vec<T, 4>;

	/// *n*-dimensional SIMD vector of single-precision floating-point values.
	/// @tparam N Number of elements.
	template <usize N>
	using fvec = vec<float, N>;

	/// 3-dimensional SIMD vector of single-precision floating-point values.
	using fvec3 = fvec<3>;

	/// 4-dimensional SIMD vector of single-precision floating-point values.
	using fvec4 = fvec<4>;
}
