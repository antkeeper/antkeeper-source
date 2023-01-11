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

#include "screen-transition.hpp"
#include "render/material-flags.hpp"
#include <functional>

screen_transition::screen_transition()
{
	// Setup material
	material.set_flags(MATERIAL_FLAG_X_RAY);
	material.set_blend_mode(render::blend_mode::translucent);
	progress = material.add_property<float>("progress");
	
	// Setup billboard
	billboard.set_material(&material);
	billboard.set_active(false);
	
	// Add single channel to transition animation
	channel = animation.add_channel(0);
	
	// Setup animation start callback to show transition billboard
	animation.set_start_callback
	(
		std::bind(&scene::object_base::set_active, &billboard, true)
	);
	
	// Setup animation end callback to hide transition billboard
	animation.set_end_callback
	(
		std::bind(&scene::object_base::set_active, &billboard, false)
	);
	
	// Setup animation frame callback to update transition progress material property
	animation.set_frame_callback
	(
		[this](int channel, float progress)
		{
			this->progress->set_value(progress);
		}
	);
	
	// Setup animation frame callback to update transition progress material property
	animation.set_frame_callback
	(
		[this](int channel, float progress)
		{
			this->progress->set_value(progress);
		}
	);
}

void screen_transition::set_visible(bool visible)
{
	billboard.set_active(visible);
}

void screen_transition::transition(float duration, bool reverse, ::animation<float>::interpolator_type interpolator, bool hide, const std::function<void()>& callback)
{
	float initial_state = (reverse) ? 1.0f : 0.0f;
	float final_state = (reverse) ? 0.0f : 1.0f;
	
	// Build transition animation
	channel->remove_keyframes();
	channel->insert_keyframe({0.0f, initial_state});
	channel->insert_keyframe({duration, final_state});
	
	// Set transition animation interpolator
	animation.set_interpolator(interpolator);
	
	this->callback = callback;
	if (hide)
	{
		// Setup animation end callback to hide transition billboard
		animation.set_end_callback
		(
			[this]()
			{
				this->billboard.set_active(false);
				if (this->callback)
					this->callback();
			}
		);
	}
	else
	{
		animation.set_end_callback(callback);
	}
	
	// Update tweens
	progress->set_value(initial_state);
	material.update_tweens();
	
	// Reset and play transition animation
	animation.stop();
	animation.play();
}
