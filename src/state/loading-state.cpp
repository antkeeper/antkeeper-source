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

void enter_loading_state(application* app)
{
	logger* logger = app->get_logger();
	int task_id = logger->open_task("Entering loading state");

	logger->close_task(task_id, EXIT_SUCCESS);

	app->get_state_machine()->change_state(app->get_title_state());
}

void exit_loading_state(application* app)
{
	logger* logger = app->get_logger();
	int task_id = logger->open_task("Exiting loading state");

	logger->close_task(task_id, EXIT_SUCCESS);
}

