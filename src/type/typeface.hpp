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

#ifndef ANTKEEPER_TYPE_TYPEFACE_HPP
#define ANTKEEPER_TYPE_TYPEFACE_HPP

namespace type {

enum class typeface_style
{
	normal,
	italic,
	oblique
};

/**
 * Variant a typeface family, with a specific style and weight. A typeface corresponds to a single digital font file.
 *
 * @see type::font
 */
class typeface
{
public:
	typeface(typeface_style style, int weight);
	
	typeface_style get_style() const;
	int get_weight() const;
	
private:
	typeface_style style;
	int weight;
};

} // namespace type

#endif // ANTKEEPER_TYPE_TYPEFACE_HPP
