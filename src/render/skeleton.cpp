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

#include "render/skeleton.hpp"
#include "math/transform-operators.hpp"

namespace render {

math::transform<float> skeleton::concatenate(std::uint16_t index) const
{
	const bone* bone = &bones[index];
	math::transform<float> transform = bone->transform;
	
	while (bone->parent)
	{
		transform = bone->parent->transform * transform;
		bone = bone->parent;
	}
	
	return transform;
}

} // namespace render
