// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/context.hpp>
#include <engine/render/pass.hpp>
#include <list>

namespace engine::render
{
	/// Composites a scene.
	class compositor
	{
	public:
		void add_pass(pass* pass);
		void remove_pass(pass* pass);
		void remove_passes();
		void composite(render::context& ctx);

		inline const auto& get_passes() const
		{
			return m_passes;
		}

	private:
		std::list<pass*> m_passes;
	};
}
