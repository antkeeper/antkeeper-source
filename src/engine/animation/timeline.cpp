// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/timeline.hpp>

auto cue_compare = [](const typename timeline::cue& a, const typename timeline::cue& b)
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

typename timeline::sequence timeline::get_cues(float start, float end) const
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

