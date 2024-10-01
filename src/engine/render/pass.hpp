// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_PASS_HPP
#define ANTKEEPER_RENDER_PASS_HPP

#include <engine/gl/pipeline.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/clear-value.hpp>
#include <engine/render/context.hpp>

namespace render {

/// Render pass.
class pass
{
public:
	pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer);
	virtual ~pass() = default;

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
