// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_CLEAR_PASS_HPP
#define ANTKEEPER_RENDER_CLEAR_PASS_HPP

#include <engine/render/pass.hpp>

namespace render {

/**
 *
 */
class clear_pass: public pass
{
public:
	clear_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer);
	
	/** Destructs a clear pass. */
	~clear_pass() override = default;
	
	void render(render::context& ctx) override;
};

} // namespace render

#endif // ANTKEEPER_RENDER_CLEAR_PASS_HPP
