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

#include <engine/type/font.hpp>

namespace type {

font::font(const font_metrics& metrics):
	metrics(metrics)
{}

font::font()
{}

font::~font()
{}

void font::kern(char32_t first, char32_t second, const math::fvec2& offset)
{
	kerning_table[first][second] = offset;
}

void font::set_font_metrics(const font_metrics& metrics)
{
	this->metrics = metrics;
}

const math::fvec2& font::get_kerning(char32_t first, char32_t second) const
{
	if (auto it_first = kerning_table.find(first); it_first != kerning_table.end())
		if (auto it_second = it_first->second.find(second); it_second != it_first->second.end())
			return it_second->second;
	
	static const math::fvec2 no_kerning = {0.0f, 0.0f};
	return no_kerning;
}

} // namespace type
