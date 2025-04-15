// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.pass;
import engine.render.context;
import engine.gl.pipeline;
import engine.gl.framebuffer;
import engine.gl.clear_value;
import engine.utility.sized_types;

export namespace engine::render
{
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
	
		inline void set_clear_mask(u8 mask) noexcept
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
		u8 m_clear_mask{};
		gl::clear_value m_clear_value;
	
	private:
		bool m_enabled;
	};
}
