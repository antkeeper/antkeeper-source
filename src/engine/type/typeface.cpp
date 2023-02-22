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

#include <engine/type/typeface.hpp>

namespace type {

typeface::typeface(typeface_style style, int weight):
	style(style),
	weight(weight)
{}

typeface::typeface():
	style(typeface_style::normal),
	weight(400)
{}

void typeface::set_style(typeface_style style)
{
	this->style = style;
}

void typeface::set_weight(int weight)
{
	this->weight = weight;
}

} // namespace type
