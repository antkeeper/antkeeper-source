// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/cpu.hpp>
#include <cstring>

#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
	#include <cpuid.h>
#endif

namespace
{
	void cpuid(int cpu_info[4], int func_id, int subfunc_id) noexcept
	{
		std::memset(cpu_info, 0, sizeof(int) * 4);

		#if defined(_MSC_VER)
			__cpuidex(cpu_info, func_id, subfunc_id);
		#elif defined(__GNUC__) || defined(__clang__)
			__cpuid_count(func_id, subfunc_id, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
		#endif
	}
}

namespace engine
{
	cpu_flags query_cpu_flags() noexcept
	{
		cpu_flags flags{};

		int cpu_info[4]{};

		// Basic feature flags
		cpuid(cpu_info, 1, 0);
		flags.mmx    = cpu_info[3] & (1 << 23);
		flags.sse    = cpu_info[3] & (1 << 25);
		flags.sse2   = cpu_info[3] & (1 << 26);
		flags.sse3   = cpu_info[2] & (1 <<  0);
		flags.ssse3  = cpu_info[2] & (1 <<  9);
		flags.sse41  = cpu_info[2] & (1 << 19);
		flags.sse42  = cpu_info[2] & (1 << 20);
		flags.fma    = cpu_info[2] & (1 << 12);
		flags.avx    = cpu_info[2] & (1 << 28);
		flags.aes    = cpu_info[2] & (1 << 25);
		flags.popcnt = cpu_info[2] & (1 << 23);

		// Extended feature flags
		cpuid(cpu_info, 7, 0);
		flags.avx2     = cpu_info[1] & (1 <<  5);
		flags.bmi1     = cpu_info[1] & (1 <<  3);
		flags.bmi2     = cpu_info[1] & (1 <<  8);
		flags.avx512f  = cpu_info[1] & (1 << 16);
		flags.avx512dq = cpu_info[1] & (1 << 17);
		flags.avx512cd = cpu_info[1] & (1 << 28);
		flags.avx512bw = cpu_info[1] & (1 << 30);
		flags.avx512vl = cpu_info[1] & (1 << 31);
		flags.sha      = cpu_info[1] & (1 << 29);

		// Extended function flags
		cpuid(cpu_info, 0x80000001, 0);
		flags.lzcnt = cpu_info[2] & (1 << 5);

		return flags;
	}

	std::string cpu_flags_to_string(const cpu_flags& flags)
	{
		std::string result;

		if (flags.aes)      result += "AES ";
		if (flags.avx)      result += "AVX ";
		if (flags.avx2)     result += "AVX2 ";
		if (flags.avx512bw) result += "AVX512BW ";
		if (flags.avx512cd) result += "AVX512CD ";
		if (flags.avx512dq) result += "AVX512DQ ";
		if (flags.avx512f)  result += "AVX512F ";
		if (flags.avx512vl) result += "AVX512VL ";
		if (flags.bmi1)     result += "BMI1 ";
		if (flags.bmi2)     result += "BMI2 ";
		if (flags.fma)      result += "FMA ";
		if (flags.lzcnt)    result += "LZCNT ";
		if (flags.mmx)      result += "MMX ";
		if (flags.popcnt)   result += "POPCNT ";
		if (flags.sha)      result += "SHA ";
		if (flags.sse)      result += "SSE ";
		if (flags.sse2)     result += "SSE2 ";
		if (flags.sse3)     result += "SSE3 ";
		if (flags.sse41)    result += "SSE4.1 ";
		if (flags.sse42)    result += "SSE4.2 ";
		if (flags.ssse3)    result += "SSSE3 ";

		if (!result.empty())
		{
			result.pop_back(); // Remove trailing space
		}

		return result;
	}
}
