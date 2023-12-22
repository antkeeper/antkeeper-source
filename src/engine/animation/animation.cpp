// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation.hpp>

animation_base::animation_base():
	looped(false),
	loop_count(0),
	paused(false),
	stopped(true),
	position(0.0),
	speed(1.0),
	start_callback(nullptr),
	end_callback(nullptr),
	loop_callback(nullptr)
{}

void animation_base::seek(float t)
{
	position = t;
}

void animation_base::rewind()
{
	seek(0.0);
}

void animation_base::loop(bool enabled)
{
	looped = enabled;
}

void animation_base::pause()
{
	paused = true;
}

void animation_base::play()
{
	if (stopped)
	{
		stopped = false;
		
		if (start_callback)
		{
			start_callback();
		}
	}
	
	paused = false;
}

void animation_base::stop()
{
	rewind();
	stopped = true;
	paused = false;
	loop_count = 0;
}

void animation_base::set_speed(float value)
{
	this->speed = value;
}

void animation_base::set_start_callback(std::function<void()> callback)
{
	start_callback = callback;
}
	
void animation_base::set_end_callback(std::function<void()> callback)
{
	end_callback = callback;
}
	
void animation_base::set_loop_callback(std::function<void(int)> callback)
{
	loop_callback = callback;
}
