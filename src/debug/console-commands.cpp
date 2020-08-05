/*
 * Copyright (C) 2020  Christopher J. Howard
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

namespace cc
{

std::string echo(std::string text)
{
	return text;
}

std::string exit(application* app)
{
	app->close(EXIT_SUCCESS);
	return std::string();
}

std::string scrot(application* app)
{
	app->take_screenshot();
	return std::string("screenshot saved");
}

std::string cue(application* app, float t, std::string command)
{
	::timeline* timeline = app->get_timeline();
	::cli* cli = app->get_cli();
	
	timeline->add_cue({timeline->get_position() + t, std::function<void()>(std::bind(&::cli::interpret, cli, command))});
	
	return std::string("command \"" + command + "\" will execute in " + std::to_string(t) + " seconds");
}

} // namespace cc
