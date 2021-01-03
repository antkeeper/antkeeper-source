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

#ifndef ANTKEEPER_DEBUG_PERFORMANCE_SAMPLER_HPP
#define ANTKEEPER_DEBUG_PERFORMANCE_SAMPLER_HPP

#include <cstdlib>
#include <vector>

namespace debug {

/**
 * Measures a rolling mean frame duration.
 */
class performance_sampler
{
public:
	/// Creates a performance sampler.
	performance_sampler();

	/**
	 * Adds a frame duration to the sample.
	 *
	 * @param duration Duration of the frame.
	 */
	void sample(double duration);

	/**
	 * Resets the sampling process.
	 */
	void reset();

	/**
	 * Sets the number of frames in a sample.
	 *
	 * @param size Number of frames in a sample.
	 */
	void set_sample_size(std::size_t size);

	/**
	 * Returns the mean frame duration.
	 */
	double mean_frame_duration() const;

private:
	std::vector<double> samples;
	std::size_t sample_size;
	std::size_t sample_index;
};

} // namespace debug

#endif // ANTKEEPER_DEBUG_PERFORMANCE_SAMPLER_HPP
