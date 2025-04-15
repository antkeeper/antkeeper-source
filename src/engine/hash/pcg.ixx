// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.hash.pcg;
export import engine.math.vector;
import engine.hash.make_uint;
import engine.utility.sized_types;
import <type_traits>;

namespace engine::hash
{
	/// @private
	template <class T>
	inline constexpr T pcg_multiplier = 0;
	template <>
	inline constexpr u8 pcg_multiplier<u8> = 141U;
	template <>
	inline constexpr u16 pcg_multiplier<u16> = 12829U;
	template <>
	inline constexpr u32 pcg_multiplier<u32> = 747796405UL;
	template <>
	inline constexpr u64 pcg_multiplier<u64> = 6364136223846793005ULL;

	/// @private
	template <class T>
	inline constexpr T pcg_increment = 0;
	template <>
	inline constexpr u8 pcg_increment<u8> = 77U;
	template <>
	inline constexpr u16 pcg_increment<u16> = 47989U;
	template <>
	inline constexpr u32 pcg_increment<u32> = 2891336453UL;
	template <>
	inline constexpr u64 pcg_increment<u64> = 1442695040888963407ULL;

	/// @private
	template <class T>
	inline constexpr T mcg_multiplier = 0;
	template <>
	inline constexpr u8 mcg_multiplier<u8> = 217U;
	template <>
	inline constexpr u16 mcg_multiplier<u16> = 62169U;
	template <>
	inline constexpr u32 mcg_multiplier<u32> = 277803737UL;
	template <>
	inline constexpr u64 mcg_multiplier<u64> = 12605985483714917081ULL;

	/// @private
	template <class T>
	[[nodiscard]] constexpr T pcg_uint(T x) noexcept
	{
		static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
		static_assert(sizeof(T) <= 8);
	
		x = x * pcg_multiplier<T> + pcg_increment<T>;
		x = (x ^ (x >> ((x >> ((sizeof(T) * 8) - (sizeof(T) + 1))) + (sizeof(T) + 1)))) * mcg_multiplier<T>;
		return x ^ (x >> ((sizeof(T) * 16 + 2) / 3));
	}

	/// @private
	template <class T>
	[[nodiscard]] inline constexpr math::vector<T, 1> pcg_uvec1(math::vector<T, 1> x) noexcept
	{
		static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
		static_assert(sizeof(T) <= 8);

		x[0] = T(x[0]);
	
		return x;
	}

	/// @private
	template <class T>
	[[nodiscard]] constexpr math::vector<T, 2> pcg_uvec2(math::vector<T, 2> x) noexcept
	{
		static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
		static_assert(sizeof(T) <= 8);
	
		x = x * pcg_multiplier<T> + pcg_increment<T>;
	
		x[0] += x[1] * pcg_multiplier<T>;
		x[1] += x[0] * pcg_multiplier<T>;

		x[0] ^= x[0] >> (sizeof(T) * 4);
		x[1] ^= x[1] >> (sizeof(T) * 4);
	
		x[0] += x[1] * pcg_multiplier<T>;
		x[1] += x[0] * pcg_multiplier<T>;
	
		x[0] ^= x[0] >> (sizeof(T) * 4);
		x[1] ^= x[1] >> (sizeof(T) * 4);
	
		return x;
	}

	/// @private
	template <class T>
	[[nodiscard]] constexpr math::vector<T, 3> pcg_uvec3(math::vector<T, 3> x) noexcept
	{
		static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
		static_assert(sizeof(T) <= 8);
	
		x = x * pcg_multiplier<T> + pcg_increment<T>;
	
		x[0] += x[1] * x[2];
		x[1] += x[2] * x[0];
		x[2] += x[0] * x[1];
	
		x[0] ^= x[0] >> (sizeof(T) * 4);
		x[1] ^= x[1] >> (sizeof(T) * 4);
		x[2] ^= x[2] >> (sizeof(T) * 4);
	
		x[0] += x[1] * x[2];
		x[1] += x[2] * x[0];
		x[2] += x[0] * x[1];
	
		return x;
	}

	/// @private
	template <class T>
	[[nodiscard]] constexpr math::vector<T, 4> pcg_uvec4(math::vector<T, 4> x) noexcept
	{
		static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
		static_assert(sizeof(T) <= 8);
	
		x = x * pcg_multiplier<T> + pcg_increment<T>;
	
		x[0] += x[1] * x[3];
		x[1] += x[2] * x[0];
		x[2] += x[0] * x[1];
		x[3] += x[1] * x[2];
	
		x[0] ^= x[0] >> (sizeof(T) * 4);
		x[1] ^= x[1] >> (sizeof(T) * 4);
		x[2] ^= x[2] >> (sizeof(T) * 4);
		x[3] ^= x[3] >> (sizeof(T) * 4);
	
		x[0] += x[1] * x[3];
		x[1] += x[2] * x[0];
		x[2] += x[0] * x[1];
		x[3] += x[1] * x[2];
	
		return x;
	}

	export
	{
		/// PCG hash function.
		/// @param x Input value.
		/// @return Unsigned pseudorandom output value.
		/// @warning Floating point and signed input values will be converted to unsigned integers via `static_cast`.
		/// @warning Vectors with more than 4 elements are not supported.
		/// 
		/// @see https://en.wikipedia.org/wiki/Permuted_congruential_generator
		/// @see O'Neill, M.E. (2014). PCG : A Family of Simple Fast Space-Efficient Statistically Good Algorithms for Random Number Generation.
		/// @see Mark Jarzynski and Marc Olano, Hash Functions for GPU Rendering, Journal of Computer Graphics Techniques (JCGT), vol. 9, no. 3, 21-38, 2020.
		[[nodiscard]] inline constexpr u8 pcg(u8 x) noexcept
		{
			return pcg_uint<u8>(x);
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u16 pcg(u16 x) noexcept
		{
			return pcg_uint<u16>(x);
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u32 pcg(u32 x) noexcept
		{
			return pcg_uint<u32>(x);
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u64 pcg(u64 x) noexcept
		{
			return pcg_uint<u64>(x);
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u8 pcg(i8 x) noexcept
		{
			return pcg_uint<u8>(static_cast<u8>(x));
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u16 pcg(i16 x) noexcept
		{
			return pcg_uint<u16>(static_cast<u16>(x));
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u32 pcg(i32 x) noexcept
		{
			return pcg_uint<u32>(static_cast<u32>(x));
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u64 pcg(i64 x) noexcept
		{
			return pcg_uint<u64>(static_cast<u64>(x));
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u32 pcg(float x) noexcept
		{
			return pcg_uint<u32>(static_cast<u32>(x));
		}

		/// @copydoc pcg(u8)
		[[nodiscard]] inline constexpr u64 pcg(double x) noexcept
		{
			return pcg_uint<u64>(static_cast<u64>(x));
		}

		/// @copydoc pcg(u8)
		template <class T, usize N>
		[[nodiscard]] inline constexpr math::vector<make_uint_t<T>, N> pcg(const math::vector<T, N>& x) noexcept
		{
			static_assert(N > 0 && N < 5, "PCG hash only supports vectors with 1-4 elements.");

			if constexpr (N == 1)
			{
				return pcg_uvec1<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
			}
			else if constexpr (N == 2)
			{
				return pcg_uvec2<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
			}
			else if constexpr (N == 3)
			{
				return pcg_uvec3<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
			}
			else
			{
				return pcg_uvec4<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
			}
		}
	}
}
