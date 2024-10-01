// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP
#define ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP

#include <engine/app/input-manager.hpp>
#include <memory>

namespace app {

class sdl_window;

/// Input manager implementation using SDL2.
class sdl_input_manager: public input_manager
{
public:
	/// Constructs an SDL input manager.
	sdl_input_manager();
	
	/// Destructs an SDL input manager.
	~sdl_input_manager() override;
	
	void update() override;
	void set_cursor_visible(bool visible) override;
	void set_relative_mouse_mode(bool enabled) override;
	void set_clipboard_text(const std::string& text) override;
	[[nodiscard]] std::string get_clipboard_text() const override;
	void start_text_input(const geom::rectangle<int>& rect) override;
	void stop_text_input() override;

private:
	input::keyboard m_keyboard;
	input::mouse m_mouse;
	std::unordered_map<int, std::unique_ptr<input::gamepad>> m_gamepad_map;
};

} // namespace app

#endif // ANTKEEPER_APP_SDL_INPUT_MANAGER_HPP
