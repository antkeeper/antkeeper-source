// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP
#define ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP

#include <engine/app/window-manager.hpp>
#include <engine/app/display.hpp>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL.h>

namespace app {

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
		bool v_sync
	) override;
	
	[[nodiscard]] std::size_t get_display_count() const override;

	[[nodiscard]] const display& get_display(std::size_t index) const override;
	
private:
	sdl_window* get_window(SDL_Window* internal_window);
	void update_display(int sdl_display_index);
	
	std::vector<display> m_displays;
	std::unordered_map<SDL_Window*, app::sdl_window*> m_window_map;
};

} // namespace app

#endif // ANTKEEPER_APP_SDL_WINDOW_MANAGER_HPP
