/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "renderer/material-property.hpp"
#include "rasterizer/shader-input.hpp"

material_property_base::material_property_base():
	input(nullptr)
{}

bool material_property_base::connect(const shader_input* input)
{
	if (!input || input->get_data_type() != get_data_type())
	{
		return false;
	}

	this->input = input;

	return true;
}

void material_property_base::disconnect()
{
	this->input = nullptr;
}

