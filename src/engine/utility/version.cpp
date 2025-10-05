// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/version.hpp>
#include <algorithm>
#include <cctype>

namespace engine
{
	[[nodiscard]] std::strong_ordering operator<=>(const version& lhs, const version& rhs) noexcept
	{
		if (auto cmp = lhs.major <=> rhs.major; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = lhs.minor <=> rhs.minor; cmp != 0)
		{
			return cmp;
		}
		if (auto cmp = lhs.patch <=> rhs.patch; cmp != 0)
		{
			return cmp;
		}

		if (lhs.prerelease == rhs.prerelease)
		{
			return std::strong_ordering::equal;
		}
		else if (lhs.prerelease.empty())
		{
			return std::strong_ordering::greater; // lhs is a release, rhs is a pre-release
		}
		else if (rhs.prerelease.empty())
		{
			return std::strong_ordering::less; // lhs is a pre-release, rhs is a release
		}
		else
		{
			// Both are pre-releases, compare identifiers
			usize pos_l = 0;
			usize pos_r = 0;
			do
			{
				// Extract next identifier from each pre-release string
				const auto dot_l = lhs.prerelease.find('.', pos_l);
				const auto dot_r = rhs.prerelease.find('.', pos_r);
				const auto end_l = (dot_l == std::string::npos) ? lhs.prerelease.size() : dot_l;
				const auto end_r = (dot_r == std::string::npos) ? rhs.prerelease.size() : dot_r;
				const auto id_l = lhs.prerelease.substr(pos_l, end_l - pos_l);
				const auto id_r = rhs.prerelease.substr(pos_r, end_r - pos_r);

				if (id_l.empty() != id_r.empty())
				{
					return id_l <=> id_r; // One identifier is empty, the other is not
				}

				if (id_l != id_r)
				{
					const bool is_num_l = !id_l.empty() && std::all_of(id_l.begin(), id_l.end(), [](char c) { return std::isdigit(c); });
					const bool is_num_r = !id_r.empty() && std::all_of(id_r.begin(), id_r.end(), [](char c) { return std::isdigit(c); });
					if (is_num_l && is_num_r)
					{
						// Both are numeric
						const auto num_l = std::stoull(id_l);
						const auto num_r = std::stoull(id_r);
						if (num_l != num_r)
						{
							return num_l <=> num_r;
						}
					}
					else if (is_num_l)
					{
						return std::strong_ordering::less; // Left is numeric, right is alphanumeric
					}
					else if (is_num_r)
					{
						return std::strong_ordering::greater; // Left is alphanumeric, right is numeric
					}
					else
					{
						return id_l <=> id_r; // Both are alphanumeric, compare lexically
					}
				}

				pos_l = (dot_l == std::string::npos) ? lhs.prerelease.size() : dot_l + 1;
				pos_r = (dot_r == std::string::npos) ? rhs.prerelease.size() : dot_r + 1;
			}
			while (pos_l < lhs.prerelease.size() || pos_r < rhs.prerelease.size());

			return pos_l <=> pos_r;
		}
	}
}
