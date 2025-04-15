// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.app.window_manager;
import engine.app.sdl_window_manager;
import <memory>;

namespace engine::app
{
	std::unique_ptr<window_manager> window_manager::instance()
	{
		return std::make_unique<sdl_window_manager>();
	}
}
