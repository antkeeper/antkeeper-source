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

#ifndef ANTKEEPER_RENDER_PASS_HPP
#define ANTKEEPER_RENDER_PASS_HPP

#include <engine/gl/pipeline.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/clear-value.hpp>
#include <engine/render/context.hpp>

namespace render {

/**
 * Render pass.
 */
class pass
{
public:
	pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer);
	virtual ~pass();

	virtual void render(render::context& ctx) = 0;

	void set_enabled(bool enabled);
	
	[[nodiscard]] inline constexpr bool is_enabled() const noexcept
	{
		return m_enabled;
	}
	
	void set_framebuffer(const gl::framebuffer* framebuffer);
	
	inline void set_clear_mask(std::uint8_t mask) noexcept
	{
		m_clear_mask = mask;
	}
	
	inline void set_clear_value(const gl::clear_value& value) noexcept
	{
		m_clear_value = value;
	}
	
	void clear();

protected:
	gl::pipeline* m_pipeline;
	const gl::framebuffer* m_framebuffer;
	std::uint8_t m_clear_mask{};
	gl::clear_value m_clear_value;
	
private:
	bool m_enabled;
};

} // namespace render

#endif // ANTKEEPER_RENDER_PASS_HPP
