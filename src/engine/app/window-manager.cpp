// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/app/window-manager.hpp>
#include <engine/app/sdl/sdl-window-manager.hpp>

namespace app {

std::unique_ptr<window_manager> window_manager::instance()
{
	return std::make_unique<sdl_window_manager>();
}

} // namespace app
