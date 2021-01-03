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

#include "performance-sampler.hpp"
#include <algorithm>
#include <numeric>

namespace debug {

performance_sampler::performance_sampler():
	sample_size(1),
	sample_index(0)
{}

void performance_sampler::sample(double duration)
{
	if (samples.size() < sample_size)
	{
		samples.push_back(duration);
	}
	else
	{
		samples[sample_index] = duration;
		sample_index = (sample_index + 1) % samples.size();
	}
}

void performance_sampler::reset()
{
	samples.clear();
	sample_index = 0;
}

void performance_sampler::set_sample_size(std::size_t size)
{
	sample_size = size;
	if (samples.size() > size)
	{
		samples.resize(size);
		sample_index = 0;
	}
}

double performance_sampler::mean_frame_duration() const
{
	return std::accumulate(samples.begin(), samples.end(), 0.0) / static_cast<double>(samples.size());
}

} // namespace debug

