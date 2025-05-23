// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.app.window_manager;
export import engine.app.window;
export import engine.app.display;
export import engine.math.vector;
export import <memory>;
export import <string>;
export import <span>;

export namespace engine::app
{
	/// Manages the creation and updating of windows.
	class window_manager
	{
	public:
		/// Allocates and returns a window manager.
		static std::unique_ptr<window_manager> instance();
	
		/// Destructs a window manager.
		virtual ~window_manager() = default;
	
		/// Updates all managed windows. This should be called once per frame.
		virtual void update() = 0;
	
		/// Constructs a window.
		/// @param title Title of the window.
		/// @param windowed_position Windowed (non-maximized, non-fullscreen) position of the window, in display units.
		/// @param windowed_size Windowed (non-maximized, non-fullscreen) size of the window, in display units.
		/// @param maximized `true` if the window should start maximized, `false` otherwise.
		/// @param fullscreen `true` if the window should start fullscreen, `false` otherwise.
		/// @param v_sync `true` if v-sync should be enabled, `false` otherwise.
		/// @return Shared pointer to the created window.
		[[nodiscard]] virtual std::shared_ptr<window> create_window
		(
			const std::string& title,
			const math::ivec2& windowed_position,
			const math::ivec2& windowed_size,
			bool maximized,
			bool fullscreen,
			bool v_sync,
			const math::fvec3& clear_color
		) = 0;
	
		/// Returns all known displays, both connected and disconnected.
		[[nodiscard]] virtual std::span<const std::shared_ptr<display>> get_displays() const = 0;

		/// Returns the primary display.
		[[nodiscard]] virtual std::shared_ptr<display> get_primary_display() const = 0;
	};
}
