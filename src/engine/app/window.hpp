// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_WINDOW_HPP
#define ANTKEEPER_APP_WINDOW_HPP

#include <engine/math/vector.hpp>
#include <engine/event/publisher.hpp>
#include <engine/app/window-events.hpp>
#include <string>

namespace gl
{
	class pipeline;
}

namespace app {

class window_manager;

/// Window interface.
class window
{
public:
	/// Destructs a window.
	virtual ~window() = default;
	
	/// Changes the title of the window.
	/// @param title Window title.
	virtual void set_title(const std::string& title) = 0;
	
	/// Changes the position of the window.
	/// @param position Position of the window, in display units.
	virtual void set_position(const math::ivec2& position) = 0;
	
	/// Changes the size of the window.
	/// @param size Size of the window, in display units.
	virtual void set_size(const math::ivec2& size) = 0;
	
	/// Sets the minimum size of the window.
	/// @param size Minimum size of the window, in display units.
	virtual void set_minimum_size(const math::ivec2& size) = 0;
	
	/// Sets the maximum size of the window.
	/// @param size Maximum size of the window, in display units.
	virtual void set_maximum_size(const math::ivec2& size) = 0;
	
	/// Maximizes or unmaximizes the window.
	/// @param maximized `true` if the window should be maximized, `false` otherwise.
	virtual void set_maximized(bool maximized) = 0;
	
	/// Enables or disables fullscreen mode.
	/// @param fullscreen `true` if the window should be in fullscreen mode, `false` otherwise.
	virtual void set_fullscreen(bool fullscreen) = 0;
	
	/// Enables or disables v-sync.
	/// @param v_sync `true` if the v-sync should be enabled, `false` otherwise.
	virtual void set_v_sync(bool v_sync) = 0;
	
	/// Makes the window's graphics context current.
	virtual void make_current() = 0;
	
	/// Swaps the front and back buffers of the window's graphics context.
	virtual void swap_buffers() = 0;
	
	/// Returns the title of the window.
	[[nodiscard]] inline const std::string& get_title() const noexcept
	{
		return m_title;
	}
	
	/// Returns the windowed (non-maximized, non-fullscreen) position of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_windowed_position() const noexcept
	{
		return m_windowed_position;
	}
	
	/// Returns the current position of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_position() const noexcept
	{
		return m_position;
	}
	
	/// Returns the windowed (non-maximized, non-fullscreen) size of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_windowed_size() const noexcept
	{
		return m_windowed_size;
	}
	
	/// Returns the current size of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_size() const noexcept
	{
		return m_size;
	}
	
	/// Returns the minimum size of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_minimum_size() const noexcept
	{
		return m_minimum_size;
	}
	
	/// Returns the maximum size of the window, in display units.
	[[nodiscard]] inline const math::ivec2& get_maximum_size() const noexcept
	{
		return m_minimum_size;
	}
	
	/// Returns the current size of the window's drawable viewport, in pixels.
	[[nodiscard]] inline const math::ivec2& get_viewport_size() const noexcept
	{
		return m_viewport_size;
	}
	
	/// Returns `true` if the window is maximized, `false` otherwise.
	[[nodiscard]] inline bool is_maximized() const noexcept
	{
		return m_maximized;
	}
	
	/// Returns `true` if the window is in fullscreen mode, `false` otherwise.
	[[nodiscard]] inline bool is_fullscreen() const noexcept
	{
		return m_fullscreen;
	}
	
	/// Returns `true` if the v-sync is enabled, `false` otherwise.
	[[nodiscard]] inline bool get_v_sync() const noexcept
	{
		return m_v_sync;
	}
	
	/// Returns the graphics pipeline associated with this window.
	[[nodiscard]] virtual gl::pipeline& get_graphics_pipeline() noexcept = 0;

	/// @copydoc get_graphics_pipeline() noexcept
	[[nodiscard]] virtual const gl::pipeline& get_graphics_pipeline() const noexcept = 0;
	
	/// Returns the channel through which window closed events are published.
	[[nodiscard]] inline auto& get_closed_channel() noexcept
	{
		return m_closed_publisher.channel();
	}
	
	/// Returns the channel through which window focus changed events are published.
	[[nodiscard]] inline auto& get_focus_changed_channel() noexcept
	{
		return m_focus_changed_publisher.channel();
	}
	
	/// Returns the channel through which window maximized events are published.
	[[nodiscard]] inline auto& get_maximized_channel() noexcept
	{
		return m_maximized_publisher.channel();
	}
	
	/// Returns the channel through which window minimized events are published.
	[[nodiscard]] inline auto& get_minimized_channel() noexcept
	{
		return m_minimized_publisher.channel();
	}
	
	/// Returns the channel through which window moved events are published.
	[[nodiscard]] inline auto& get_moved_channel() noexcept
	{
		return m_moved_publisher.channel();
	}
	
	/// Returns the channel through which window resized events are published.
	[[nodiscard]] inline auto& get_resized_channel() noexcept
	{
		return m_resized_publisher.channel();
	}
	
	/// Returns the channel through which window restored events are published.
	[[nodiscard]] inline auto& get_restored_channel() noexcept
	{
		return m_restored_publisher.channel();
	}
	
	/// Returns the channel through which window drop begin events are published.
	[[nodiscard]] inline auto& get_drop_begin_channel() noexcept
	{
		return m_drop_begin_publisher.channel();
	}
	
	/// Returns the channel through which window drop end events are published.
	[[nodiscard]] inline auto& get_drop_end_channel() noexcept
	{
		return m_drop_end_publisher.channel();
	}
	
	/// Returns the channel through which window drop file events are published.
	[[nodiscard]] inline auto& get_drop_file_channel() noexcept
	{
		return m_drop_file_publisher.channel();
	}
	
	/// Returns the channel through which window drop text events are published.
	[[nodiscard]] inline auto& get_drop_text_channel() noexcept
	{
		return m_drop_text_publisher.channel();
	}
	
protected:
	friend class window_manager;
	
	std::string m_title;
	math::ivec2 m_windowed_position{0, 0};
	math::ivec2 m_position{0, 0};
	math::ivec2 m_windowed_size{0, 0};
	math::ivec2 m_size{0, 0};
	math::ivec2 m_minimum_size{0, 0};
	math::ivec2 m_maximum_size{0, 0};
	math::ivec2 m_viewport_size{0, 0};
	bool m_maximized{false};
	bool m_fullscreen{false};
	bool m_v_sync{false};
	
	event::publisher<window_closed_event> m_closed_publisher;
	event::publisher<window_focus_changed_event> m_focus_changed_publisher;
	event::publisher<window_maximized_event> m_maximized_publisher;
	event::publisher<window_minimized_event> m_minimized_publisher;
	event::publisher<window_moved_event> m_moved_publisher;
	event::publisher<window_resized_event> m_resized_publisher;
	event::publisher<window_restored_event> m_restored_publisher;
	event::publisher<window_drop_begin_event> m_drop_begin_publisher;
	event::publisher<window_drop_end_event> m_drop_end_publisher;
	event::publisher<window_drop_file_event> m_drop_file_publisher;
	event::publisher<window_drop_text_event> m_drop_text_publisher;
};

} // namespace app

#endif // ANTKEEPER_APP_WINDOW_HPP
