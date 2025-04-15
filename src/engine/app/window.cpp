// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.app.window;

namespace engine::app
{
	window::window(window_manager& window_manager):
		m_window_manager(&window_manager)
	{}
}
