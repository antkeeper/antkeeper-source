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

#include "animation.hpp"

animation_base::animation_base():
	position(0.0f),
	start_callback(nullptr),
	end_callback(nullptr),
	loop_callback(nullptr)
{}

void animation_base::seek(double t)
{
	position = t;
}

void animation_base::reset()
{
	seek(0.0);
}

void animation_base::set_start_callback(std::function<void()> callback)
{
	start_callback = callback;
}
	
void animation_base::set_end_callback(std::function<void()> callback)
{
	end_callback = callback;
}
	
void animation_base::set_loop_callback(std::function<void()> callback)
{
	loop_callback = callback;
}
