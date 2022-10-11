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

#ifndef ANTKEEPER_GAME_COMPONENT_SPRING_HPP
#define ANTKEEPER_GAME_COMPONENT_SPRING_HPP

#include "animation/spring.hpp"
#include "utility/fundamental-types.hpp"
#include <functional>

namespace game {
namespace component {

/**
 * Numeric spring with one component.
 */
struct spring1
{
	/// Numeric spring with one component.
	numeric_spring<float, float> spring;
	
	/// Spring solved callback.
	std::function<void(float)> callback;
};

/**
 * Numeric spring with two components.
 */
struct spring2
{
	/// Numeric spring with two components.
	numeric_spring<float2, float> spring;
	
	/// Spring solved callback.
	std::function<void(const float2&)> callback;
};

/**
 * Numeric spring with three components.
 */
struct spring3
{
	/// Numeric spring with three components.
	numeric_spring<float3, float> spring;
	
	/// Spring solved callback.
	std::function<void(const float3&)> callback;
};

/**
 * Numeric spring with four components.
 */
struct spring4
{
	/// Numeric spring with four components.
	numeric_spring<float4, float> spring;
	
	/// Spring solved callback.
	std::function<void(const float4&)> callback;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_SPRING_HPP
