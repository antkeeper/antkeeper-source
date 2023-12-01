// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

void compositor::composite(render::context& ctx)
{
	for (pass* pass: passes)
	{
		if (pass->is_enabled())
		{
			pass->render(ctx);
		}
	}
}

} // namespace render
