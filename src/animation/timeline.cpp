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

#include "timeline.hpp"

auto cue_compare = [](const cue& a, const cue& b)
{
	return std::get<0>(a) < std::get<0>(b);
};

timeline::timeline():
	cues(cue_compare),
	position(0.0f),
	autoremove(false)
{}

void timeline::advance(float dt)
{
	auto lower_bound = cues.lower_bound({position, nullptr});
	auto upper_bound = cues.upper_bound({position + dt, nullptr});

	for (auto iterator = lower_bound; iterator != upper_bound; ++iterator)
	{
		std::get<1>(*iterator)();
	}

	if (autoremove && lower_bound != upper_bound)
	{
		cues.erase(lower_bound, upper_bound);
	}

	position += dt;
}

void timeline::seek(float t)
{
	position = t;
}

void timeline::add_cue(const cue& c)
{
	cues.emplace(c);
}

void timeline::remove_cue(const cue& c)
{
	cues.erase(c);
}

void timeline::remove_cues(float start, float end)
{
	auto lower_bound = cues.lower_bound({start, nullptr});
	auto upper_bound = cues.upper_bound({end, nullptr});
	cues.erase(lower_bound, upper_bound);
}

void timeline::add_sequence(const sequence& s)
{
	for (const cue& c: s)
	{
		add_cue(c);
	}
}

void timeline::remove_sequence(const sequence& s)
{
	for (const cue& c: s)
	{
		remove_cue(c);
	}
}

void timeline::clear()
{
	cues.clear();
}

void timeline::set_autoremove(bool enabled)
{
	autoremove = enabled;
}

sequence timeline::get_cues(float start, float end)
{
	sequence s;

	auto lower_bound = cues.lower_bound({start, nullptr});
	auto upper_bound = cues.upper_bound({end, nullptr});
	for (auto iterator = lower_bound; iterator != upper_bound; ++iterator)
	{
		s.push_back(*iterator);
	}

	return s;
}

