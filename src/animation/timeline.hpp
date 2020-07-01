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

#ifndef ANTKEEPER_TIMELINE_HPP
#define ANTKEEPER_TIMELINE_HPP

#include <functional>
#include <set>
#include <tuple>

/**
 * Scheduled function consisting of a time and function object.
 */
typedef std::tuple<float, std::function<void()>> cue;

/**
 * List of cues.
 */
typedef std::list<cue> sequence;

/**
 * Timeline which executes cues (scheduled functions) when advanced over their respective positions in time.
 */
class timeline
{
public:
	/**
	 * Creates a timeline.
	 */
	timeline();

	/**
	 * Advances the timeline position (t) by @p dt, triggering any cues scheduled between `t` and `dt`. If autoremove is enabled, triggered cues will be removed.
	 *
	 * @param dt Delta time by which the timeline position will be advanced.
	 */
	void advance(float dt);

	/**
	 * Sets the timeline position to @p t.
	 *
	 * @param t Position in time to which the timeline position will be set.
	 */
	void seek(float t);

	/**
	 * Adds a cue to the timeline.
	 *
	 * @param c Cue to add.
	 */
	void add_cue(const cue& c);

	/**
	 * Removes a cue from the timeline. If there are multiple identical cues (same time and function), they will all be removed.
	 *
	 * @param c Cue to remove.
	 */
	void remove_cue(const cue& c);

	/**
	 * Removes all cues on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 */
	void remove_cues(float start, float end);

	/**
	 * Adds a sequence of cues to the timeline.
	 *
	 * @param s Sequence of cues to add.
	 */
	void add_sequence(const sequence& s);

	/**
	 * Removes a sequence of cues from the timeline.
	 *
	 * @param s Sequence of cues to remove.
	 */
	void remove_sequence(const sequence& s);

	/**
	 * Removes all cues from the timeline.
	 */
	void clear();

	/**
	 * If enabled, cues will be automatically removed from the timeline when they are triggered.
	 *
	 * @param enabled Whether to enable autoremove.
	 */
	void set_autoremove(bool enabled);

	/**
	 * Returns the current position in time on the timeline.
	 */
	float get_position() const;

	/**
	 * Returns all the cues on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 * @return All cues on `[start, end)`.
	 */
	sequence get_cues(float start, float end);

private:
	std::multiset<cue, std::function<bool(const cue&, const cue&)>> cues;
	float position;
	bool autoremove;
};

inline float timeline::get_position() const
{
	return position;
}

#endif // ANTKEEPER_TIMELINE_HPP

