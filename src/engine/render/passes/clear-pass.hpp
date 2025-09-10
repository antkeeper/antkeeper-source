// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/pass.hpp>

namespace engine::render
{
	/// Pass that clears the framebuffer.
	class clear_pass: public pass
	{
	public:
		/// Constructs a clear pass.
		clear_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer);
	
		/// Destructs a clear pass.
		~clear_pass() override = default;
	
		void render(render::context& ctx) override;
	};
}
