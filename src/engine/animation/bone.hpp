// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
