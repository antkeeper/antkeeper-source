// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/app/sdl/sdl-window.hpp>
#include <engine/app/sdl/sdl-window-manager.hpp>
#include <engine/config.hpp>
#include <engine/debug/log.hpp>
#include <engine/gl/pipeline.hpp>
#include <engine/debug/contract.hpp>
#include <glad/gl.h>
#include <stdexcept>

namespace app {

sdl_window::sdl_window
(
	window_manager& window_manager,
	const std::string& title,
	const math::ivec2& windowed_position,
	const math::ivec2& windowed_size,
	bool maximized,
	bool fullscreen,
	bool v_sync,
	const math::fvec3& clear_color
):
	window(window_manager)
{
	// Create SDL window properties
	SDL_PropertiesID sdl_window_properties = SDL_CreateProperties();
	if (sdl_window_properties == 0)
	{
		auto error_message = std::format("Failed to create SDL window properties: {}", SDL_GetError());
		SDL_ClearError();

		debug::log_error("{}", error_message);
		throw std::runtime_error(std::move(error_message));
	}

	// Set SDL window properties
	try
	{
		debug::invariant(SDL_SetStringProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str()));
		debug::invariant(SDL_SetNumberProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, windowed_position.x()));
		debug::invariant(SDL_SetNumberProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, windowed_position.y()));
		debug::invariant(SDL_SetNumberProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowed_size.x()));
		debug::invariant(SDL_SetNumberProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowed_size.y()));
		debug::invariant(SDL_SetBooleanProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, maximized));
		debug::invariant(SDL_SetBooleanProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, false));
		debug::invariant(SDL_SetBooleanProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true));
		debug::invariant(SDL_SetBooleanProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true));
		debug::invariant(SDL_SetBooleanProperty(sdl_window_properties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true));
	}
	catch (...)
	{
		SDL_DestroyProperties(sdl_window_properties);
		throw;
	}

	// Create SDL window
	debug::log_debug("Creating SDL window...");
	m_internal_window = SDL_CreateWindowWithProperties(sdl_window_properties);

	// Destroy SDL window properties
	SDL_DestroyProperties(sdl_window_properties);
	
	if (!m_internal_window)
	{
		auto error_message = std::format("Failed to create SDL window: {}", SDL_GetError());
		SDL_ClearError();

		debug::log_fatal("{}", error_message);
		debug::log_debug("Creating SDL window... FAILED");
		throw std::runtime_error(std::move(error_message));
	}

	debug::log_debug("Creating SDL window... OK");
	
	// Create OpenGL context
	debug::log_debug("Creating OpenGL context...");
	m_internal_context = SDL_GL_CreateContext(m_internal_window);
	if (!m_internal_context)
	{
		auto error_message = std::format("Failed to create OpenGL context: {}", SDL_GetError());
		SDL_ClearError();

		debug::log_fatal("{}", error_message);
		debug::log_debug("Creating OpenGL context... FAILED");
		throw std::runtime_error(std::move(error_message));
	}
	debug::log_debug("Creating OpenGL context... OK");
	
	// Query OpenGL context info
	int opengl_context_version_major = -1;
	int opengl_context_version_minor = -1;
	int opengl_context_red_size = -1;
	int opengl_context_green_size = -1;
	int opengl_context_blue_size = -1;
	int opengl_context_alpha_size = -1;
	int opengl_context_depth_size = -1;
	int opengl_context_stencil_size = -1;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &opengl_context_version_major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &opengl_context_version_minor);
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &opengl_context_red_size);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &opengl_context_green_size);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &opengl_context_blue_size);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &opengl_context_alpha_size);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &opengl_context_depth_size);
	SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &opengl_context_stencil_size);
	
	// Log OpenGL context info
	debug::log_info
	(
		"OpenGL context version: {}.{}; format: R{}G{}B{}A{}D{}S{}",
		opengl_context_version_major,
		opengl_context_version_minor,
		opengl_context_red_size,
		opengl_context_green_size,
		opengl_context_blue_size,
		opengl_context_alpha_size,
		opengl_context_depth_size,
		opengl_context_stencil_size
	);
	
	// Compare OpenGL context version with requested version
	if (opengl_context_version_major != config::opengl_version_major ||
		opengl_context_version_minor != config::opengl_version_minor)
	{
		debug::log_warning("Requested OpenGL context version {}.{} but got version {}.{}", config::opengl_version_major, config::opengl_version_minor, opengl_context_version_major, opengl_context_version_minor);
	}
	
	// Compare OpenGL context format with requested format
	if (opengl_context_red_size < config::opengl_min_red_size ||
		opengl_context_green_size < config::opengl_min_green_size ||
		opengl_context_blue_size < config::opengl_min_blue_size ||
		opengl_context_alpha_size < config::opengl_min_alpha_size ||
		opengl_context_depth_size < config::opengl_min_depth_size ||
		opengl_context_stencil_size < config::opengl_min_stencil_size)
	{
		debug::log_warning
		(
			"OpenGL context format (R{}G{}B{}A{}D{}S{}) does not meet minimum requested format (R{}G{}B{}A{}D{}S{}).",
			opengl_context_red_size,
			opengl_context_green_size,
			opengl_context_blue_size,
			opengl_context_alpha_size,
			opengl_context_depth_size,
			opengl_context_stencil_size,
			config::opengl_min_red_size,
			config::opengl_min_green_size,
			config::opengl_min_blue_size,
			config::opengl_min_alpha_size,
			config::opengl_min_depth_size,
			config::opengl_min_stencil_size
		);
	}
	
	// Load OpenGL functions via GLAD
	debug::log_debug("Loading OpenGL functions...");
	if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)))
	{
		auto error_message = std::format("Failed to load OpenGL functions: {}", SDL_GetError());
		debug::log_fatal("{}", error_message);
		debug::log_debug("Loading OpenGL functions... FAILED");
		throw std::runtime_error(std::move(error_message));
	}
	debug::log_debug("Loading OpenGL functions... OK");
	
	// Log OpenGL information
	debug::log_info
	(
		"OpenGL vendor: {}; renderer: {}; version: {}; shading language version: {}",
		reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
		reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
		reinterpret_cast<const char*>(glGetString(GL_VERSION)),
		reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))
	);

	//
	if (fullscreen)
	{
		set_fullscreen(true);
	}

	// Enable or disable v-sync
	set_v_sync(v_sync);

	// Allocate graphics pipeline
	m_graphics_pipeline = std::make_unique<gl::pipeline>();
	
	// Fill window with clear color
	m_graphics_pipeline->clear_attachments(gl::color_clear_bit, {{clear_color[0], clear_color[1], clear_color[2], 1.0f}});
	swap_buffers();

	// Update window state
	this->m_title = title;
	this->m_windowed_position = windowed_position;
	this->m_windowed_size = windowed_size;
	this->m_maximized = maximized;
	SDL_GetWindowPosition(m_internal_window, &this->m_position.x(), &this->m_position.y());
	SDL_GetWindowSize(m_internal_window, &this->m_size.x(), &this->m_size.y());
	SDL_GetWindowMinimumSize(m_internal_window, &this->m_minimum_size.x(), &this->m_minimum_size.y());
	SDL_GetWindowMaximumSize(m_internal_window, &this->m_maximum_size.x(), &this->m_maximum_size.y());
	SDL_GetWindowSizeInPixels(m_internal_window, &this->m_viewport_size.x(), &this->m_viewport_size.y());
}

sdl_window::~sdl_window()
{
	// Deallocate graphics pipeline
	m_graphics_pipeline.reset();
	
	// Destruct the OpenGL context
	SDL_GL_DestroyContext(m_internal_context);
	
	// Destruct the SDL window
	SDL_DestroyWindow(m_internal_window);
}

void sdl_window::set_title(const std::string& title)
{
	SDL_SetWindowTitle(m_internal_window, title.c_str());
	this->m_title = title;
}

void sdl_window::set_position(const math::ivec2& position)
{
	SDL_SetWindowPosition(m_internal_window, position.x(), position.y());
}

void sdl_window::set_size(const math::ivec2& size)
{
	SDL_SetWindowSize(m_internal_window, size.x(), size.y());
}

void sdl_window::set_minimum_size(const math::ivec2& size)
{
	SDL_SetWindowMinimumSize(m_internal_window, size.x(), size.y());
	this->m_minimum_size = size;
}

void sdl_window::set_maximum_size(const math::ivec2& size)
{
	SDL_SetWindowMaximumSize(m_internal_window, size.x(), size.y());
	this->m_maximum_size = size;
}

void sdl_window::set_maximized(bool maximized)
{
	if (maximized)
	{
		SDL_MaximizeWindow(m_internal_window);
	}
	else
	{
		SDL_RestoreWindow(m_internal_window);
	}
}

void sdl_window::set_fullscreen(bool fullscreen)
{
	if (fullscreen != m_fullscreen)
	{
		if (fullscreen)
		{
			const SDL_DisplayMode* sdl_display_mode = nullptr;

			if (const auto sdl_window_display_id = SDL_GetDisplayForWindow(m_internal_window); sdl_window_display_id)
			{
				sdl_display_mode = SDL_GetDesktopDisplayMode(sdl_window_display_id);
			}

			SDL_SetWindowFullscreenMode(m_internal_window, sdl_display_mode);
		}

		SDL_SetWindowFullscreen(m_internal_window, fullscreen);
		m_fullscreen = fullscreen;
	}
}

void sdl_window::set_v_sync(bool v_sync)
{
	if (v_sync)
	{
		debug::log_debug("Enabling adaptive v-sync...");
		if (!SDL_GL_SetSwapInterval(-1))
		{
			debug::log_error("Failed to enable adaptive v-sync: {}", SDL_GetError());
			debug::log_debug("Enabling adaptive v-sync... FAILED");
			debug::log_debug("Enabling synchronized v-sync...");
			if (!SDL_GL_SetSwapInterval(1))
			{
				debug::log_error("Failed to enable synchronized v-sync: {}", SDL_GetError());
				debug::log_debug("Enabling synchronized v-sync... FAILED");
				v_sync = false;
			}
			else
			{
				debug::log_debug("Enabling synchronized v-sync... OK");
			}
		}
		else
		{
			debug::log_debug("Enabling adaptive v-sync... OK");
		}
	}
	else
	{
		debug::log_debug("Disabling v-sync...");
		if (!SDL_GL_SetSwapInterval(0))
		{
			debug::log_error("Failed to disable v-sync: {}", SDL_GetError());
			debug::log_debug("Disabling v-sync... FAILED");
			v_sync = true;
		}
		else
		{
			debug::log_debug("Disabling v-sync... OK");
		}
	}
	
	this->m_v_sync = v_sync;
}

void sdl_window::set_relative_mouse_mode(bool enabled)
{
	if (!SDL_SetWindowRelativeMouseMode(m_internal_window, enabled ? true : false))
	{
		debug::log_error("Failed to set relative mouse mode: \"{}\"", SDL_GetError());
		SDL_ClearError();
	}
}

void sdl_window::start_text_input(const geom::rectangle<int>& rect)
{
	const SDL_Rect sdl_rect
	{
		rect.min.x(),
		rect.min.y(),
		rect.max.x() - rect.min.x(),
		rect.max.y() - rect.min.y()
	};
	debug::invariant(SDL_SetTextInputArea(m_internal_window, &sdl_rect, 0));
	debug::invariant(SDL_StartTextInput(m_internal_window));
	debug::log_debug("Started text input");
}

void sdl_window::stop_text_input()
{
	debug::invariant(SDL_StopTextInput(m_internal_window));
	debug::log_debug("Stopped text input");
}

void sdl_window::make_current()
{
	debug::invariant(SDL_GL_MakeCurrent(m_internal_window, m_internal_context));
}

void sdl_window::swap_buffers()
{
	debug::invariant(SDL_GL_SwapWindow(m_internal_window));
}

std::shared_ptr<display> sdl_window::get_display() const
{
	debug::precondition(m_window_manager);

	SDL_DisplayID sdl_display_id = SDL_GetDisplayForWindow(m_internal_window);
	if (!sdl_display_id)
	{
		debug::log_error("Failed to get SDL display for window: {}", SDL_GetError());
		SDL_ClearError();
		return nullptr;
	}

	auto& sdl_window_manager = static_cast<app::sdl_window_manager&>(*m_window_manager);

	auto it = sdl_window_manager.m_display_map.find(sdl_display_id);
	if (it == sdl_window_manager.m_display_map.end())
	{
		debug::log_error("SDL display unrecognized by SDL window manager.");
		return nullptr;
	}

	return sdl_window_manager.m_displays[it->second];
}

} // namespace app
