/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "app/sdl/sdl-window.hpp"
#include "config.hpp"
#include "debug/log.hpp"
#include <glad/glad.h>
#include <stdexcept>

namespace app {

sdl_window::sdl_window
(
	const std::string& title,
	const math::vector<int, 2>& windowed_position,
	const math::vector<int, 2>& windowed_size,
	bool maximized,
	bool fullscreen,
	bool v_sync
)
{
	// Determine SDL window creation flags
	Uint32 window_flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	if (maximized)
	{
		window_flags |= SDL_WINDOW_MAXIMIZED;
	}
	if (fullscreen)
	{
		window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	
	// Create SDL window
	debug::log::trace("Creating SDL window...");
	internal_window = SDL_CreateWindow
	(
		title.c_str(),
    	windowed_position.x(),
    	windowed_position.y(),
	    windowed_size.x(),
	    windowed_size.y(),
		window_flags
	);
	if (!internal_window)
	{
		debug::log::fatal("Failed to create SDL window: {}", SDL_GetError());
		throw std::runtime_error("Failed to create SDL window");
	}
	debug::log::trace("Created SDL window");
	
	// Create OpenGL context
	debug::log::trace("Creating OpenGL context...");
	internal_context = SDL_GL_CreateContext(internal_window);
	if (!internal_context)
	{
		debug::log::fatal("Failed to create OpenGL context: {}", SDL_GetError());
		throw std::runtime_error("Failed to create OpenGL context");
	}
	debug::log::trace("Created OpenGL context");
	
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
	debug::log::info
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
		debug::log::warning("Requested OpenGL context version {}.{} but got version {}.{}", config::opengl_version_major, config::opengl_version_minor, opengl_context_version_major, opengl_context_version_minor);
	}
	
	// Compare OpenGL context format with requested format
	if (opengl_context_red_size < config::opengl_min_red_size ||
		opengl_context_green_size < config::opengl_min_green_size ||
		opengl_context_blue_size < config::opengl_min_blue_size ||
		opengl_context_alpha_size < config::opengl_min_alpha_size ||
		opengl_context_depth_size < config::opengl_min_depth_size ||
		opengl_context_stencil_size < config::opengl_min_stencil_size)
	{
		debug::log::warning
		(
			"OpenGL context format (R{}G{}B{}A{}D{}S{}) does not meet minimum requested format (R{}G{}B{}A{}D{}S{})",
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
	debug::log::trace("Loading OpenGL functions...");
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		debug::log::fatal("Failed to load OpenGL functions", SDL_GetError());
		throw std::runtime_error("Failed to load OpenGL functions");
	}
	debug::log::trace("Loaded OpenGL functions");
	
	// Log OpenGL information
	debug::log::info
	(
		"OpenGL vendor: {}; renderer: {}; version: {}; shading language version: {}",
		reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
		reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
		reinterpret_cast<const char*>(glGetString(GL_VERSION)),
		reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION))
	);
	
	// Fill window with color
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	swap_buffers();
	
	// Enable or disable v-sync
	set_v_sync(v_sync);
	
	// Update window state
	this->title = title;
	this->windowed_position = windowed_position;
	this->windowed_size = windowed_size;
	this->maximized = maximized;
	this->fullscreen = fullscreen;
	SDL_GetWindowPosition(internal_window, &this->position.x(), &this->position.y());
	SDL_GetWindowSize(internal_window, &this->size.x(), &this->size.y());
	SDL_GetWindowMinimumSize(internal_window, &this->minimum_size.x(), &this->minimum_size.y());
	SDL_GetWindowMaximumSize(internal_window, &this->maximum_size.x(), &this->maximum_size.y());
	SDL_GL_GetDrawableSize(internal_window, &this->viewport_size.x(), &this->viewport_size.y());
	
	// Allocate rasterizer
	this->rasterizer = new gl::rasterizer();
}

sdl_window::~sdl_window()
{
	// Destruct rasterizer
	delete rasterizer;
	
	// Destruct the OpenGL context
	SDL_GL_DeleteContext(internal_context);
	
	// Destruct the SDL window
	SDL_DestroyWindow(internal_window);
}

void sdl_window::set_title(const std::string& title)
{
	SDL_SetWindowTitle(internal_window, title.c_str());
	this->title = title;
}

void sdl_window::set_position(const math::vector<int, 2>& position)
{
	SDL_SetWindowPosition(internal_window, position.x(), position.y());
}

void sdl_window::set_size(const math::vector<int, 2>& size)
{
	SDL_SetWindowSize(internal_window, size.x(), size.y());
}

void sdl_window::set_minimum_size(const math::vector<int, 2>& size)
{
	SDL_SetWindowMinimumSize(internal_window, size.x(), size.y());
	this->minimum_size = size;
}

void sdl_window::set_maximum_size(const math::vector<int, 2>& size)
{
	SDL_SetWindowMaximumSize(internal_window, size.x(), size.y());
	this->maximum_size = size;
}

void sdl_window::set_maximized(bool maximized)
{
	if (maximized)
	{
		SDL_MaximizeWindow(internal_window);
	}
	else
	{
		SDL_RestoreWindow(internal_window);
	}
}

void sdl_window::set_fullscreen(bool fullscreen)
{
	SDL_SetWindowFullscreen(internal_window, (fullscreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	this->fullscreen = fullscreen;
}

void sdl_window::set_v_sync(bool v_sync)
{
	if (v_sync)
	{
		debug::log::trace("Enabling adaptive v-sync...");
		if (SDL_GL_SetSwapInterval(-1) != 0)
		{
			debug::log::error("Failed to enable adaptive v-sync: {}", SDL_GetError());
			debug::log::trace("Enabling synchronized v-sync...");
			if (SDL_GL_SetSwapInterval(1) != 0)
			{
				debug::log::error("Failed to enable synchronized v-sync: {}", SDL_GetError());
				v_sync = false;
			}
			else
			{
				debug::log::debug("Enabled synchronized v-sync");
			}
		}
		else
		{
			debug::log::debug("Enabled adaptive v-sync");
		}
	}
	else
	{
		debug::log::trace("Disabling v-sync...");
		if (SDL_GL_SetSwapInterval(0) != 0)
		{
			debug::log::error("Failed to disable v-sync: {}", SDL_GetError());
			v_sync = true;
		}
		else
		{
			debug::log::debug("Disabled v-sync");
		}
	}
	
	this->v_sync = v_sync;
}

void sdl_window::make_current()
{
	SDL_GL_MakeCurrent(internal_window, internal_context);
}

void sdl_window::swap_buffers()
{
	SDL_GL_SwapWindow(internal_window);
}

} // namespace app
