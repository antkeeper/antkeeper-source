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

void enter_title_state(application* app)
{
	logger* logger = app->get_logger();
	logger->push_task("Entering title state");

	// Get timeline
	timeline* timeline = app->get_timeline();
	
	// Create title sequence
	float t = timeline->get_position();
	sequence title_sequence =
	{
		{t +  0.0f, [logger](){ logger->log("cue sound fade-in\n"); }},
		{t +  3.0f, [logger](){ logger->log("cue scene fade-in from black\n"); }},
		{t +  8.0f, [logger](){ logger->log("cue title fade-in\n"); }},
		{t + 10.0f, [logger](){ logger->log("cue menu fade-in\n"); }}
	};

	// Add title sequence to timeline
	timeline->add_sequence(title_sequence);

	logger->pop_task(EXIT_SUCCESS);
}

void exit_title_state(application* app)
{
	logger* logger = app->get_logger();
	logger->push_task("Exiting title state");

	logger->pop_task(EXIT_SUCCESS);
}

