// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_INPUT_MANAGER_HPP
#define ANTKEEPER_APP_INPUT_MANAGER_HPP

#include <engine/input/device.hpp>
#include <engine/input/gamepad.hpp>
#include <engine/input/keyboard.hpp>
#include <engine/input/mouse.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/geom/primitives/rectangle.hpp>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

namespace app {

/// Manages virtual input devices.
class input_manager
{
public:
	/// Allocates and returns an input manager.
	static std::unique_ptr<input_manager> instance();

	/// Destructs an input manager.
	virtual ~input_manager() = default;
	
	/// Processes input events.
	virtual void update() = 0;
	
	/// Shows or hides the cursor.
	/// @param visible `true` to show the cursor, `false` to hide it.
	virtual void set_cursor_visible(bool visible) = 0;
	
	/// Enables or disables relative mouse mode.
	/// @param enabled `true` to enable relative mouse mode, `false` to disable it.
	virtual void set_relative_mouse_mode(bool enabled) = 0;
	
	/// Sets the clipboard text.
	/// @param text UTF-8 text.
	virtual void set_clipboard_text(const std::string& text) = 0;
	
	/// Returns UTF-8 text from the clipboard.
	[[nodiscard]] virtual std::string get_clipboard_text() const = 0;
	
	/// Enables text input events.
	/// @param rect Text input rectangle, with the origin at the upper left.
	virtual void start_text_input(const geom::rectangle<int>& rect = {}) = 0;
	
	/// Disables text input events.
	virtual void stop_text_input() = 0;

	/// Returns the event dispatcher associated with registered input devices.
	[[nodiscard]] inline ::event::dispatcher& get_event_dispatcher() noexcept
	{
		return m_event_dispatcher;
	}

	/// @copydoc get_event_dispatcher() noexcept
	[[nodiscard]] inline const ::event::dispatcher& get_event_dispatcher() const noexcept
	{
		return m_event_dispatcher;
	}
	
	/// Returns the set of registered gamepads.
	[[nodiscard]] inline const std::unordered_set<input::gamepad*>& get_gamepads() noexcept
	{
		return m_gamepads;
	}
	
	/// Returns the set of registered keyboards.
	[[nodiscard]] inline const std::unordered_set<input::keyboard*>& get_keyboards() noexcept
	{
		return m_keyboards;
	}
	
	/// Returns the set of registered mice.
	[[nodiscard]] inline const std::unordered_set<input::mouse*>& get_mice() noexcept
	{
		return m_mice;
	}
	
protected:
	/// Registers an input device.
	/// @param device Input device to register.
	void register_device(input::device& device);

	/// Registers a gamepad.
	/// @param device Gamepad to register.
	void register_gamepad(input::gamepad& device);

	/// Registers a keyboard.
	/// @param device Keyboard to register.
	void register_keyboard(input::keyboard& device);

	/// Registers a mouse.
	/// @param device Mouse to register.
	void register_mouse(input::mouse& device);
	
	/// Unregisters an input device.
	/// @param device Input device to unregister.
	void unregister_device(input::device& device);

	/// Unregisters a gamepad.
	/// @param device Gamepad to unregister.
	void unregister_gamepad(input::gamepad& device);

	/// Unregisters a keyboard.
	/// @param device Keyboard to unregister.
	void unregister_keyboard(input::keyboard& device);

	/// Unregisters a mouse.
	/// @param device Mouse to unregister.
	void unregister_mouse(input::mouse& device);
	
	::event::dispatcher m_event_dispatcher;
	
private:
	std::multimap<input::device*, std::shared_ptr<::event::subscription>> m_subscriptions;
	std::unordered_set<input::gamepad*> m_gamepads;
	std::unordered_set<input::keyboard*> m_keyboards;
	std::unordered_set<input::mouse*> m_mice;
};

} // namespace app

#endif // ANTKEEPER_APP_INPUT_MANAGER_HPP
