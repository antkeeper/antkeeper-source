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

#include <engine/render/stages/queue-stage.hpp>
#include <engine/scene/object.hpp>
#include <algorithm>
#include <execution>

namespace render {

void queue_stage::execute(render::context& ctx)
{
	// For each visible object in the render context
	std::for_each
	(
		std::execution::seq,
		std::begin(ctx.objects),
		std::end(ctx.objects),
		[&ctx](scene::object_base* object)
		{
			object->render(ctx);
		}
	);
}

} // namespace render
