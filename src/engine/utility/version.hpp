// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <compare>
#include <format>
#include <string>

namespace engine
{
	/// Semantic version.
	/// @see https://semver.org/
	struct version
	{
		usize major{}; /// Major version.
		usize minor{}; /// Minor version.
		usize patch{}; /// Patch version.
		std::string prerelease; /// Optional pre-release identifier(s).
		std::string metadata;   /// Optional metadata.

		/// Compares two version numbers for equality.
		/// @param lhs Left-hand side version.
		/// @param rhs Right-hand side version.
		/// @return `true` if the two versions are equal, `false` otherwise.
		[[nodiscard]] constexpr friend bool operator==(const version& lhs, const version& rhs) noexcept
		{
			return (lhs.major == rhs.major)
				&& (lhs.minor == rhs.minor)
				&& (lhs.patch == rhs.patch)
				&& (lhs.prerelease == rhs.prerelease);
		}

		/// Compares two version numbers using three-way comparison.
		/// @param lhs Left-hand side version.
		/// @param rhs Right-hand side version.
		/// @return Result of the comparison.
		friend std::strong_ordering operator<=>(const version& lhs, const version& rhs) noexcept;
	};
}

namespace std
{
	/// Specialization of std::formatter for engine::version.
	/// @relates engine::version
	template <>
	struct formatter<engine::version>
	{
		constexpr auto parse(format_parse_context& fc)
		{
			return fc.begin();
		}

		auto format(const engine::version& v, format_context& fc) const
		{
			auto it = format_to(fc.out(), "{}.{}.{}", v.major, v.minor, v.patch);
			if (!v.prerelease.empty())
			{
				it = format_to(it, "-{}", v.prerelease);
			}
			if (!v.metadata.empty())
			{
				it = format_to(it, "+{}", v.metadata);
			}
			return it;
		}
	};
}
