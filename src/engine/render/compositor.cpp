// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/compositor.hpp>
#include <engine/render/pass.hpp>

namespace engine::render
{
	void compositor::add_pass(pass* pass)
	{
		m_passes.push_back(pass);
	}

	void compositor::remove_pass(pass* pass)
	{
		m_passes.remove(pass);
	}

	void compositor::remove_passes()
	{
		m_passes.clear();
	}

	void compositor::composite(render::context& ctx)
	{
		for (pass* pass: m_passes)
		{
			if (pass->is_enabled())
			{
				pass->render(ctx);
			}
		}
	}
}
