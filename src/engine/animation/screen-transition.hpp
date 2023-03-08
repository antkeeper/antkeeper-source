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
	std::shared_ptr<render::material_float> progress;
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
