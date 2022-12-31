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

#include "console-commands.hpp"
#include "application.hpp"
#include "animation/timeline.hpp"
#include "debug/cli.hpp"

namespace debug {
namespace cc {

std::string echo(std::string text)
{
	return text;
}

std::string exit(game::context* ctx)
{
	ctx->app->close();
	return std::string();
}

std::string scrot(game::context* ctx)
{
	//ctx->app->take_screenshot();
	return std::string("screenshot saved");
}

std::string cue(game::context* ctx, float t, std::string command)
{
	::timeline* timeline = ctx->timeline;
	debug::cli* cli = ctx->cli;
	
	timeline->add_cue({timeline->get_position() + t, std::function<void()>(std::bind(&debug::cli::interpret, cli, command))});
	
	return std::string("command \"" + command + "\" will execute in " + std::to_string(t) + " seconds");
}

} // namespace cc
} // namespace debug
