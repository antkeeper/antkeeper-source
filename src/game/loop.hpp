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

#ifndef ANTKEEPER_GAME_LOOP_HPP
#define ANTKEEPER_GAME_LOOP_HPP

#include <chrono>
#include <functional>

namespace game {

/**
 * Game loop with fixed timestep update calls and variable timestep render calls.
 *
 * @see https://gafferongames.com/post/fix_your_timestep/
 */
class loop
{
public:
	loop();

	/**
	 * Sets the update callback.
	 *
	 * @param callback Function which takes two parameters: `t`, the total elapsed time, and `dt`, delta time (timestep) which is calculated as `1.0 / update_rate`. This function will be called at the frequency specified by `loop::set_update_rate()`.
	 */
	void set_update_callback(std::function<void(double, double)> callback);

	/**
	 * Sets the render callback.
	 *
	 * @param callback Function which takes one parameter: `alpha`, which is a factor that can be used to interpolate between the previous and current update states.
	 */
	void set_render_callback(std::function<void(double)> callback);

	/**
	 * Sets the update frequency.
	 *
	 * @param frequency Frequency, in hertz, at which the update callback should be called.
	 */
	void set_update_frequency(double frequency);

	/**
	 * Sets the maximum duration of a frame. This limits the number of times the update callback is called per frame, thereby preventing a "spiral of death", in which the update callback is called too many times per frame while trying to catch up to the target update rate.
	 *
	 * @param duration Maximum frame duration, in seconds..
	 */
	void set_max_frame_duration(double duration);

	/**
	 * Returns the duration of the last frame, in seconds.
	 */
	double get_frame_duration() const;
	
	/// Returns the frequency, in hertz, at which the update callback should be called.
	double get_update_frequency() const;
	
	/// Returns the period, in seconds, between update callback calls.
	double get_update_period() const;

	/**
	 * Resets the total elapsed time, frame duration, and internal timers.
	 */
	void reset();

	/**
	 * Updates the internal timers and performs the scheduled update and render callbacks.
	 */
	void tick();

private:
	std::function<void(double, double)> update_callback;
	std::function<void(double)> render_callback;
	double update_frequency;
	double update_period;
	double max_frame_duration;
	double elapsed_time;
	double accumulator;
	std::chrono::high_resolution_clock::time_point frame_start;
	std::chrono::high_resolution_clock::time_point frame_end;
	double frame_duration;
};

inline double loop::get_update_frequency() const
{
	return update_frequency;
}

inline double loop::get_update_period() const
{
	return update_period;
}

} // namespace game

#endif // ANTKEEPER_GAME_LOOP_HPP
