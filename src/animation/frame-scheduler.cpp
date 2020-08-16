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

#include "frame-scheduler.hpp"
#include <algorithm>

frame_scheduler::frame_scheduler():
	update_callback(nullptr),
	render_callback(nullptr),
	update_rate(60.0),
	update_timestep(1.0 / update_rate),
	max_frame_duration(update_timestep)
{
	reset();
}

void frame_scheduler::set_update_callback(std::function<void(double, double)> callback)
{
	update_callback = callback;
}

void frame_scheduler::set_render_callback(std::function<void(double)> callback)
{
	render_callback = callback;
}

void frame_scheduler::set_update_rate(double frequency)
{
	update_rate = frequency;
	update_timestep = 1.0 / frequency;
}

void frame_scheduler::set_max_frame_duration(double duration)
{
	max_frame_duration = duration;
}

double frame_scheduler::get_frame_duration() const
{
	return frame_duration;
}

void frame_scheduler::reset()
{
	elapsed_time = 0.0;
	accumulator = 0.0;
	frame_start = std::chrono::high_resolution_clock::now();
	frame_end = frame_start;
	frame_duration = 0.0;
}

void frame_scheduler::tick()
{
	accumulator += std::min<double>(max_frame_duration, frame_duration);

	while (accumulator >= update_timestep)
	{
		update_callback(elapsed_time, update_timestep);
		elapsed_time += update_timestep;
		accumulator -= update_timestep;
	}

	render_callback(accumulator * update_rate);

	frame_end = std::chrono::high_resolution_clock::now();
	frame_duration = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start).count()) / 1000000.0;
	frame_start = frame_end;
}
