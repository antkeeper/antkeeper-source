// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <string>

namespace engine
{
	/// CPU feature flags.
	struct cpu_flags
	{
		/// @name Basic features
		/// @{

		bool mmx;     /// MMX.
		bool sse;     /// SSE.
		bool sse2;    /// SSE2.
		bool sse3;    /// SSE3.
		bool ssse3;   /// SSSE3.
		bool sse41;   /// SSE4.1.
		bool sse42;   /// SSE4.2.
		bool fma;     /// FMA.
		bool avx;     /// AVX.
		bool aes;     /// AES.
		bool popcnt;  /// POPCNT.

		/// @}

		/// @name Extended features
		/// @{

		bool avx2;     /// AVX2.
		bool bmi1;     /// BMI1.
		bool bmi2;     /// BMI2.
		bool avx512f;  /// AVX512F.
		bool avx512dq; /// AVX512DQ.
		bool avx512cd; /// AVX512CD.
		bool avx512bw; /// AVX512BW.
		bool avx512vl; /// AVX512VL.
		bool sha;      /// SHA.

		/// @}

		/// @name Extended function features
		/// @{

		bool lzcnt; /// LZCNT.

		/// @}
	};

	/// Queries the host processor for CPU feature support.
	/// @return CPU feature flags.
	[[nodiscard]] cpu_flags query_cpu_flags() noexcept;

	/// Converts CPU flags to a string representation.
	/// @param flags CPU feature flags.
	/// @return Space-delimited string of present CPU flags, in lexical order.
	[[nodiscard]] std::string cpu_flags_to_string(const cpu_flags& flags);
}
