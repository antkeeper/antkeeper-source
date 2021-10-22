/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "animation/animation.hpp"
#include "render/material.hpp"
#include "render/material-property.hpp"
#include "scene/billboard.hpp"

/**
 * Encapsulates a shader-based animated screen transition.
 */
class screen_transition
{
public:
	screen_transition();
	
	void transition(float duration, bool reverse, animation<float>::interpolator_type interpolator);
	
	scene::billboard* get_billboard();
	render::material* get_material();
	::animation<float>* get_animation();
	
private:
	scene::billboard billboard;
	render::material material;
	render::material_property<float>* progress;
	::animation<float> animation;
	::animation<float>::channel* channel;
};

inline scene::billboard* screen_transition::get_billboard()
{
	return &billboard;
}

inline render::material* screen_transition::get_material()
{
	return &material;
}

inline animation<float>* screen_transition::get_animation()
{
	return &animation;
}

#endif // ANTKEEPER_SCREEN_TRANSITION_HPP
