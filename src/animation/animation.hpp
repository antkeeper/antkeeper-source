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

#ifndef ANTKEEPER_ANIMATION_HPP
#define ANTKEEPER_ANIMATION_HPP

#include <functional>

class animation_channel_base
{
	
};

template <typename T>
class animation_channel: public animation_channel_base
{
public:
	void insert_keyframe(float position, const T& value);
};

class animation_base
{
public:
	void animate(float dt);
};

template <typename T>
class animation: public animation_base
{
public:
	void set_start_callback(std::function<void()> callback);
	void set_end_callback(std::function<void()> callback);
	void set_loop_callback(std::function<void()> callback);
	void set_frame_callback(std::function<void(std::size_t, const T&)> callback);

private:
	std::function<void()> start_callback;
	std::function<void()> end_callback;
	std::function<void()> loop_callback;
	std::function<void(std::size_t, const T&)> motion_callback;
};

class skeletal_animation: public animation<int>
{

};

#endif // ANTKEEPER_ANIMATION_HPP
