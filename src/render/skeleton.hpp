/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_RENDER_SKELETON_HPP
#define ANTKEEPER_RENDER_SKELETON_HPP

#include "render/bone.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace render {

/**
 * Skeletal animation skeleton.
 */
struct skeleton
{
	/// Collection of bones.
	std::vector<bone> bones;
	
	/// Maps bone names to bone indices.
	std::unordered_map<std::string, std::uint16_t> bone_map;
	
	/**
	 * Calculates the global transform of a bone.
	 *
	 * @param index Index of the bone.
	 * @return Global transform of the bone.
	 */
	math::transform<float> concatenate(std::uint16_t index) const;
};

} // namespace render

#endif // ANTKEEPER_RENDER_SKELETON_HPP
