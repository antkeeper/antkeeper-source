// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

/// Contains sized integer and floating-point types.
namespace engine::inline sized_types
{
	/// Signed integer type used for array indexing and pointer arithmetic.
	/// The size of this type is architecture-dependent (32-bit on 32-bit architectures and 64-bit on 64-bit architectures).
	using isize = decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));

	/// Unsigned integer type used for array indexing and loop counting.
	/// The size of this type is architecture-dependent (32-bit on 32-bit architectures and 64-bit on 64-bit architectures).
	using usize = decltype(sizeof(int));

	template <usize Size>
	consteval auto sized_int()
	{
		if constexpr (Size == sizeof(char))
		{
			return static_cast<char>(0);
		}
		else if constexpr (Size == sizeof(short))
		{
			return static_cast<short>(0);
		}
		else if constexpr (Size == sizeof(int))
		{
			return static_cast<int>(0);
		}
		else if constexpr (Size == sizeof(long))
		{
			return static_cast<long>(0);
		}
		else if constexpr (Size == sizeof(long long))
		{
			return static_cast<long long>(0);
		}
		else
		{
			static_assert(false, "No signed integer type with requested size.");
		}
	}

	template <usize Size>
	consteval auto sized_uint()
	{
		if constexpr (Size == sizeof(unsigned char))
		{
			return static_cast<unsigned char>(0);
		}
		else if constexpr (Size == sizeof(unsigned short))
		{
			return static_cast<unsigned short>(0);
		}
		else if constexpr (Size == sizeof(unsigned int))
		{
			return static_cast<unsigned int>(0);
		}
		else if constexpr (Size == sizeof(unsigned long))
		{
			return static_cast<unsigned long>(0);
		}
		else if constexpr (Size == sizeof(unsigned long long))
		{
			return static_cast<unsigned long long>(0);
		}
		else
		{
			static_assert(false, "No unsigned integer type with requested size.");
		}
	}

	template <usize Size>
	consteval auto sized_float()
	{
		if constexpr (Size == sizeof(float))
		{
			return static_cast<float>(0.0);
		}
		else if constexpr (Size == sizeof(double))
		{
			return static_cast<double>(0.0);
		}
		else if constexpr (Size == sizeof(long double))
		{
			return static_cast<long double>(0.0);
		}
		else
		{
			static_assert(false, "No floating-point type with requested size.");
		}
	}

	/// Signed integer type with a specified size.
	/// @tparam Size Size of the type, in bytes.
	template <usize Size>
	using sized_int_t = decltype(sized_int<Size>());

	/// Unsigned integer type with a specified size.
	/// @tparam Size Size of the type, in bytes.
	template <usize Size>
	using sized_uint_t = decltype(sized_uint<Size>());

	/// Floating-point type with a specified size.
	/// @tparam Size Size of the type, in bytes.
	template <usize Size>
	using sized_float_t = decltype(sized_float<Size>());

	/// 8-bit signed integer type.
	using i8 = sized_int_t<1>;

	/// 16-bit signed integer type.
	using i16 = sized_int_t<2>;

	/// 32-bit signed integer type.
	using i32 = sized_int_t<4>;

	/// 64-bit signed integer type.
	using i64 = sized_int_t<8>;

	/// 8-bit unsigned integer type.
	using u8 = sized_uint_t<1>;

	/// 16-bit unsigned integer type.
	using u16 = sized_uint_t<2>;

	/// 32-bit unsigned integer type.
	using u32 = sized_uint_t<4>;

	/// 64-bit unsigned integer type.
	using u64 = sized_uint_t<8>;

	/// 32-bit floating-point type.
	using f32 = sized_float_t<4>;

	/// 64-bit floating-point type.
	using f64 = sized_float_t<8>;

	/// isize literal operator.
	[[nodiscard]] consteval isize operator ""_isize(unsigned long long x)
	{
		return static_cast<isize>(x);
	}

	/// usize literal operator.
	[[nodiscard]] consteval usize operator ""_usize(unsigned long long x)
	{
		return static_cast<usize>(x);
	}

	/// 8-bit signed integer literal operator.
	[[nodiscard]] consteval i8 operator ""_i8(unsigned long long x)
	{
		return static_cast<i8>(x);
	}

	/// 16-bit signed integer literal operator.
	[[nodiscard]] consteval i16 operator ""_i16(unsigned long long x)
	{
		return static_cast<i16>(x);
	}

	/// 32-bit signed integer literal operator.
	[[nodiscard]] consteval i32 operator ""_i32(unsigned long long x)
	{
		return static_cast<i32>(x);
	}

	/// 64-bit signed integer literal operator.
	[[nodiscard]] consteval i64 operator ""_i64(unsigned long long x)
	{
		return static_cast<i64>(x);
	}

	/// 8-bit unsigned integer literal operator.
	[[nodiscard]] consteval u8 operator ""_u8(unsigned long long x)
	{
		return static_cast<u8>(x);
	}

	/// 16-bit unsigned integer literal operator.
	[[nodiscard]] consteval u16 operator ""_u16(unsigned long long x)
	{
		return static_cast<u16>(x);
	}

	/// 32-bit unsigned integer literal operator.
	[[nodiscard]] consteval u32 operator ""_u32(unsigned long long x)
	{
		return static_cast<u32>(x);
	}

	/// 64-bit unsigned integer literal operator.
	[[nodiscard]] consteval u64 operator ""_u64(unsigned long long x)
	{
		return static_cast<u64>(x);
	}

	/// 32-bit floating-point literal operator.
	[[nodiscard]] consteval f32 operator ""_f32(long double x)
	{
		return static_cast<f32>(x);
	}

	/// 64-bit floating-point literal operator.
	[[nodiscard]] consteval f64 operator ""_f64(long double x)
	{
		return static_cast<f64>(x);
	}
}
