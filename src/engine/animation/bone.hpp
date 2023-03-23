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

#include <engine/math/transform.hpp>
#include <cstdint>

/**
 * Bone index type.
 *
 * @note The maximum number of bones per skeleton is 65,536.
 */
using bone_index_type = std::uint16_t;

/**
 * Bone transform type.
 */
using bone_transform_type = math::transform<float>;

/**
 * Bone skinning matrix type.
 */
using bone_matrix_type = bone_transform_type::matrix_type;

#endif // ANTKEEPER_ANIMATION_BONE_HPP
