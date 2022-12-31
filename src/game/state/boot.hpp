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

#ifndef ANTKEEPER_GAME_STATE_BOOT_HPP
#define ANTKEEPER_GAME_STATE_BOOT_HPP

#include "game/state/base.hpp"
#include "game/context.hpp"
#include <optional>

namespace game {
namespace state {

class boot: public game::state::base
{
public:
	boot(game::context& ctx, int argc, char** argv);
	virtual ~boot();
	
private:
	void parse_options(int argc, char** argv);
	void setup_resources();
	void load_config();
	void load_strings();
	void setup_window();
	void setup_rendering();
	void setup_sound();
	void setup_scenes();
	void setup_animation();
	void setup_entities();
	void setup_systems();
	void setup_controls();
	void setup_ui();
	void setup_debugging();
	void setup_loop();
	void loop();
	
	// Command line options
	std::optional<bool> option_continue;
	std::optional<std::string> option_data;
	std::optional<bool> option_fullscreen;
	std::optional<bool> option_new_game;
	std::optional<bool> option_quick_start;
	std::optional<bool> option_reset;
	std::optional<int> option_v_sync;
	std::optional<bool> option_windowed;
};

} // namespace state
} // namespace game

#endif // ANTKEEPER_GAME_STATE_BOOT_HPP
