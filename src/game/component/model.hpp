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

#ifndef ANTKEEPER_GAME_COMPONENT_MODEL_HPP
#define ANTKEEPER_GAME_COMPONENT_MODEL_HPP

#include "render/model.hpp"
#include <unordered_map>

namespace game {
namespace component {

struct model
{
	render::model* render_model;
	std::unordered_map<int, render::material*> materials;
	int instance_count;
	unsigned int layers;
};

} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_MODEL_HPP
