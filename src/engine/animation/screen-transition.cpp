// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/screen-transition.hpp>
#include <engine/render/material-flags.hpp>
#include <functional>

screen_transition::screen_transition()
{
	progress = std::make_shared<render::matvar_float>(1, 0.0f);
	
	// Setup material
	material = std::make_shared<render::material>();
	material->set_blend_mode(render::material_blend_mode::translucent);
	material->set_variable("progress", progress);
	
	// Setup billboard
	billboard.set_material(material);
	billboard.set_layer_mask(0);
	
	// Add single channel to transition animation
	channel = animation.add_channel(0);
	
	// Setup animation start callback to show transition billboard
	// animation.set_start_callback
	// (
		// std::bind(&scene::object_base::set_active, &billboard, true)
	// );
	
	// Setup animation end callback to hide transition billboard
	// animation.set_end_callback
	// (
		// std::bind(&scene::object_base::set_active, &billboard, false)
	// );
	
	// Setup animation frame callback to update transition progress material property
	animation.set_frame_callback
	(
		[this](int channel, float progress)
		{
			this->progress->set(progress);
		}
	);
	
	// Setup animation frame callback to update transition progress material property
	animation.set_frame_callback
	(
		[this](int channel, float progress)
		{
			this->progress->set(progress);
		}
	);
}

void screen_transition::set_visible(bool visible)
{
	//billboard.set_active(visible);
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
				this->billboard.set_layer_mask(0);
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
	progress->set(initial_state);
	
	// Reset and play transition animation
	animation.stop();
	animation.play();
	this->billboard.set_layer_mask(1);
}
