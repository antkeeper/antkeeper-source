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

#include <engine/render/compositor.hpp>
#include <engine/render/pass.hpp>

namespace render {

void compositor::add_pass(pass* pass)
{
	passes.push_back(pass);
}

void compositor::remove_pass(pass* pass)
{
	passes.remove(pass);
}

void compositor::remove_passes()
{
	passes.clear();
}

void compositor::composite(const render::context& ctx, render::queue& queue) const
{
	for (const pass* pass: passes)
	{
		if (pass->is_enabled())
		{
			pass->render(ctx, queue);
		}
	}
}

} // namespace render
