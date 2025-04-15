// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.app.sdl_input_manager;
export import engine.app.input_manager;
import <memory>;
export import <string>;
import <unordered_map>;

export namespace engine::app
{
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
		void set_clipboard_text(const std::string& text) override;
		[[nodiscard]] std::string get_clipboard_text() const override;

	private:
		input::keyboard m_keyboard;
		input::mouse m_mouse;
		std::unordered_map<int, std::unique_ptr<input::gamepad>> m_gamepad_map;
	};
}
