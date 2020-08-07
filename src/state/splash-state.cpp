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
#include "scene/billboard.hpp"
#include "renderer/material.hpp"
#include "renderer/material-property.hpp"
#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/easings.hpp"
#include "animation/screen-transition.hpp"
#include "renderer/passes/sky-pass.hpp"

#include <functional>
#include <iostream>

void enter_splash_state(application* app)
{
	logger* logger = app->get_logger();	
	logger->push_task("Entering splash state");
	
	// Disable sky pass
	app->get_sky_pass()->set_enabled(false);
	
	// Add splash billboard to UI scene
	app->get_ui_scene()->add_object(app->get_splash_billboard());
	
	// Setup timing
	const float splash_fade_in_duration = 0.5f;
	const float splash_hang_duration = 2.0f;
	const float splash_fade_out_duration = 0.5f;
	
	// Start fade in
	app->get_fade_transition()->transition(splash_fade_in_duration, true, ease_in_quad<float, double>);
	
	// Crate fade out function
	auto fade_out = [app, splash_fade_out_duration]()
	{
		app->get_fade_transition()->transition(splash_fade_out_duration, false, ease_out_quad<float, double>);
	};
	
	// Create change state function
	auto change_state = [app]()
	{
		app->get_state_machine()->change_state(app->get_play_state());
	};
	
	// Schedule fade out and change state events
	timeline* timeline = app->get_timeline();
	float t = timeline->get_position();
	timeline::sequence splash_sequence =
	{
		{t + splash_fade_in_duration + splash_hang_duration, fade_out},
		{t + splash_fade_in_duration + splash_hang_duration + splash_fade_out_duration, change_state}
	};
	timeline->add_sequence(splash_sequence);
	
	logger->pop_task(EXIT_SUCCESS);
}

void exit_splash_state(application* app)
{
	logger* logger = app->get_logger();
	logger->push_task("Exiting splash state");
	
	// Remove splash billboard from UI scene
	app->get_ui_scene()->remove_object(app->get_splash_billboard());
	
	logger->pop_task(EXIT_SUCCESS);
}
