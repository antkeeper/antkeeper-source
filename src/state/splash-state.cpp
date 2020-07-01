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

#include "application-states.hpp"
#include "application.hpp"
#include <iostream>

void enter_splash_state(application* app)
{
	logger* logger = app->get_logger();
	logger->log("Entering splash state...\n");
	
	timeline* timeline = app->get_timeline();
	float t = timeline->get_position();

	sequence splash_sequence =
	{
		{t +  0.0f, [logger](){ logger->log("cue logo fade-in\n"); }},
		{t +  3.0f, [logger](){ logger->log("cue logo fade-out\n"); }},
		{t + 8.0f, [app](){ app->get_state_machine()->change_state(app->get_play_state()); }}
	};

	timeline->add_sequence(splash_sequence);
	
	logger->success("Entering splash state... success\n");
}

void exit_splash_state(application* app)
{
	logger* logger = app->get_logger();
	logger->log("Exiting splash state...\n");
	logger->success("Exiting splash state... success\n");
}

