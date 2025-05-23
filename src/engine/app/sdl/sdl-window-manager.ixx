// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <SDL3/SDL.h>

export module engine.app.sdl_window_manager;
export import engine.app.window_manager;
export import engine.app.display;
export import engine.math.vector;
export import <span>;
export import <memory>;
import <unordered_map>;
import <vector>;

export namespace engine::app
{
	class sdl_window;

	/// SDL-based window manager.
	class sdl_window_manager: public window_manager
	{
	public:
		/// Constructs an SDL window manager.
		sdl_window_manager();

		/// Destructs an SDL window manager.
		~sdl_window_manager() override;

		void update() override;

		[[nodiscard]] std::shared_ptr<window> create_window
		(
			const std::string& title,
			const math::ivec2& windowed_position,
			const math::ivec2& windowed_size,
			bool maximized,
			bool fullscreen,
			bool v_sync,
			const math::fvec3& clear_color
		) override;

		[[nodiscard]] std::span<const std::shared_ptr<display>> get_displays() const override;
		[[nodiscard]] std::shared_ptr<display> get_primary_display() const override;

	private:
		friend class sdl_window;

		sdl_window* get_window(SDL_Window* internal_window);
		void update_display(display& display, unsigned int sdl_display_index);

		std::unordered_map<unsigned int, unsigned int> m_display_map;
		std::vector<std::shared_ptr<display>> m_displays;
		std::unordered_map<SDL_Window*, app::sdl_window*> m_window_map;
	};
}
