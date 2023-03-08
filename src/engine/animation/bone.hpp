/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_ANIMATION_BONE_HPP
#define ANTKEEPER_ANIMATION_BONE_HPP

#include <cstdint>

/**
 * Skeletal animation bone identifier, consisting of a bone index in the lower half, and a parent bone index in the upper half.
 */
using bone = std::uint32_t;

/// Mask to extract the index of a bone.
inline constexpr bone bone_index_mask = 0xffff;

/**
 * Bone index comparison function object.
 */
struct bone_index_compare
{
	/**
	 * Compares the indices of two bones.
	 *
	 * @param lhs First bone.
	 * @param rhs Second bone.
	 *
	 * @return Comparison result.
	 */
	[[nodiscard]] inline bool operator()(const bone& lhs, const bone& rhs) const noexcept
	{
		return (lhs & bone_index_mask) < (rhs & bone_index_mask);
	}
};

/**
 * Constructs a bone identifier.
 *
 * @param index Index of the bone.
 * @param parent_index Index of the parent bone.
 *
 * @return Bone identifier.
 */
[[nodiscard]] inline bone make_bone(std::uint16_t index, std::uint16_t parent_index) noexcept
{
	return (static_cast<std::uint32_t>(parent_index) << 16) | index;
}

/**
 * Constructs an orphan bone identifier.
 *
 * @param index Index of the orphan bone.
 *
 * @return Orphan bone identifier.
 */
[[nodiscard]] inline bone make_bone(std::uint16_t index) noexcept
{
	return make_bone(index, index);
}

/**
 * Returns the index of a bone.
 *
 * @param x Bone identifier.
 *
 * @return Index of the bone.
 */
[[nodiscard]] inline std::uint16_t bone_index(bone x) noexcept
{
	return static_cast<std::uint16_t>(x & bone_index_mask);
}

/**
 * Returns the parent index of a bone.
 *
 * @param x Bone identifier.
 *
 * @return Index of the parent bone.
 */
[[nodiscard]] inline std::uint16_t bone_parent_index(bone x) noexcept
{
	return static_cast<std::uint16_t>(x >> 16);
}

/**
 * Returns `true` if a bone has a parent, `false` otherwise.
 *
 * @param x Bone identifier.
 *
 * @return Bone parent status.
 */
[[nodiscard]] inline bool bone_has_parent(bone x) noexcept
{
	return (x & bone_index_mask) != (x >> 16);
}

#endif // ANTKEEPER_ANIMATION_BONE_HPP
