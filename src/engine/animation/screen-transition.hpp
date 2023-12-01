// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCREEN_TRANSITION_HPP
#define ANTKEEPER_SCREEN_TRANSITION_HPP

#include <engine/animation/animation.hpp>
#include <engine/render/material.hpp>
#include <engine/render/material-variable.hpp>
#include <engine/scene/billboard.hpp>

/**
 * Encapsulates a shader-based animated screen transition.
 */
class screen_transition
{
public:
	screen_transition();
	
	void set_visible(bool visible);
	
	void transition(float duration, bool reverse, animation<float>::interpolator_type interpolator, bool hide = true, const std::function<void()>& callback = nullptr);
	
	scene::billboard* get_billboard();
	std::shared_ptr<render::material> get_material();
	::animation<float>* get_animation();
	
private:
	scene::billboard billboard;
	std::shared_ptr<render::material> material;
	std::shared_ptr<render::matvar_float> progress;
	::animation<float> animation;
	::animation<float>::channel* channel;
	std::function<void()> callback;
};

inline scene::billboard* screen_transition::get_billboard()
{
	return &billboard;
}

inline std::shared_ptr<render::material> screen_transition::get_material()
{
	return material;
}

inline animation<float>* screen_transition::get_animation()
{
	return &animation;
}

#endif // ANTKEEPER_SCREEN_TRANSITION_HPP
