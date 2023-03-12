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

#ifndef ANTKEEPER_RENDER_OPERATION_HPP
#define ANTKEEPER_RENDER_OPERATION_HPP

#include <engine/utility/fundamental-types.hpp>
#include <engine/animation/pose.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/render/material.hpp>
#include <cstdint>
#include <memory>
#include <span>

namespace render {

/**
 * Atomic render operation.
 */
struct operation
{
	const gl::vertex_array* vertex_array{nullptr};
	gl::drawing_mode drawing_mode{gl::drawing_mode::triangles};
	std::size_t start_index{0};
	std::size_t index_count{0};
	std::shared_ptr<render::material> material;
	
	float4x4 transform{float4x4::identity()};
	float depth{0.0f};
	
	std::size_t instance_count{0};
	std::span<const float4x4> skinning_palette{};
};

} // namespace render

#endif // ANTKEEPER_RENDER_OPERATION_HPP
