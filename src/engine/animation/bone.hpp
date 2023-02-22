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
typedef std::uint32_t bone;

/// Mask to extract the index of a bone.
constexpr bone bone_index_mask = 0xffff;

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
	 * @return Comparison result.
	 */
	bool operator()(const bone& lhs, const bone& rhs) const;
};

/**
 * Constructs a bone identifier.
 *
 * @param index Index of the bone.
 * @param parent_index Index of the parent bone.
 * @return Bone identifier.
 */
bone make_bone(std::uint16_t index, std::uint16_t parent_index);

/**
 * Constructs an orphan bone identifier.
 *
 * @param index Index of the orphan bone.
 * @return Orphan bone identifier.
 */
bone make_bone(std::uint16_t index);

/**
 * Returns the index of a bone.
 *
 * @param x Bone identifier.
 * @return Index of the bone.
 */
std::uint16_t bone_index(bone x);

/**
 * Returns the parent index of a bone.
 *
 * @param x Bone identifier.
 * @return Index of the parent bone.
 */
std::uint16_t bone_parent_index(bone x);

/**
 * Returns `true` if a bone has a parent, `false` otherwise.
 *
 * @param x Bone identifier.
 * @return Bone parent status.
 */
bool bone_has_parent(bone x);

inline bool bone_index_compare::operator()(const bone& lhs, const bone& rhs) const
{
	return (lhs & bone_index_mask) < (rhs & bone_index_mask);
}

inline bone make_bone(std::uint16_t index, std::uint16_t parent_index)
{
	return (static_cast<std::uint32_t>(parent_index) << 16) | index;
}

inline bone make_bone(std::uint16_t index)
{
	return make_bone(index, index);
}

inline std::uint16_t bone_index(bone x)
{
	return static_cast<std::uint16_t>(x & bone_index_mask);
}

inline std::uint16_t bone_parent_index(bone x)
{
	return static_cast<std::uint16_t>(x >> 16);
}

inline bool bone_has_parent(bone x)
{
	return (x & bone_index_mask) != (x >> 16);
}

#endif // ANTKEEPER_ANIMATION_BONE_HPP
