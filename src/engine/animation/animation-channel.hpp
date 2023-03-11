/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_ANIMATION_CHANNEL_HPP
#define ANTKEEPER_ANIMATION_CHANNEL_HPP

#include <list>
#include <set>
#include <tuple>

/**
 * Single channel in a keyframe animation.
 *
 * @see animation
 */
template <typename T>
class animation_channel
{
public:
	/// Keyframe consisting of a time and a value.
	typedef std::tuple<float, T> keyframe;
	
	/**
	 * Creates an animation channel.
	 *
	 * @param id ID of the channel.
	 */
	explicit animation_channel(int id);
	
	/// Creates an animation channel.
	animation_channel();
	
	/// Creates an animation channel.
	animation_channel(const animation_channel& other);
	
	/// Assigns the contents of another channel to this channel.
	animation_channel& operator=(const animation_channel& other);
	
	/**
	 * Adds a keyframe to the animation.
	 *
	 * @param k Keyframe to add.
	 */
	void insert_keyframe(const keyframe& k);
	
	/**
	 * Removes all keyframes on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 */
	void remove_keyframes(float start, float end);
	
	/// Removes all keyframes from the animation.
	void remove_keyframes();
	
	/**
	 * Finds the keyframes to the left and right of @p position.
	 *
	 * @param position Position in time.
	 * @return Array containing the the keyframes on the left and right of @p position.
	 */
	std::array<const keyframe*, 2> find_keyframes(float position) const;
	
	/**
	 * Finds all the keyframes on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 * @return All keyframes on `[start, end)`.
	 */
	std::list<keyframe> find_keyframes(float start, float end) const;
	
	/// Returns the ID of the animation channel.
	int get_id() const;
	
	/// Returns the duration of the animation channel.
	float get_duration() const;
	
private:
	struct keyframe_compare
	{
		inline bool operator()(const keyframe& lhs, const keyframe& rhs) const
		{
			return std::get<0>(lhs) < std::get<0>(rhs);
		}
	};
	
	int id;
	std::set<keyframe, keyframe_compare> keyframes;
};

template <typename T>
animation_channel<T>::animation_channel(int id):
	id(id),
	keyframes(keyframe_compare())
{}

template <typename T>
animation_channel<T>::animation_channel():
	animation_channel(-1)
{}

template <typename T>
animation_channel<T>::animation_channel(const animation_channel& other):
	id(other.id),
	keyframes(other.keyframes)
{}

template <typename T>
animation_channel<T>& animation_channel<T>::operator=(const animation_channel& other)
{
	id = other.id;
	keyframes = other.keyframes;
	return *this;
}

template <typename T>
void animation_channel<T>::insert_keyframe(const keyframe& k)
{
	keyframes.emplace(k);
}

template <typename T>
void animation_channel<T>::remove_keyframes(float start, float end)
{
	auto lower_bound = keyframes.lower_bound({start, T()});
	auto upper_bound = keyframes.upper_bound({end, T()});
	keyframes.erase(lower_bound, upper_bound);
}

template <typename T>
void animation_channel<T>::remove_keyframes()
{
	keyframes.clear();
}

template <typename T>
std::array<const typename animation_channel<T>::keyframe*, 2> animation_channel<T>::find_keyframes(float position) const
{
	// Find the following keyframe
	auto upper_bound = keyframes.upper_bound({position, T()});
	
	// Find the preceding keyframe
	auto lower_bound = upper_bound;
	--lower_bound;
	
	std::array<const keyframe*, 2> frames;
	frames[0] = (lower_bound != keyframes.end()) ? &(*lower_bound) : nullptr;
	frames[1] = (upper_bound != keyframes.end()) ? &(*upper_bound) : nullptr;
	
	return frames;
}

template <typename T>
std::list<typename animation_channel<T>::keyframe> animation_channel<T>::find_keyframes(float start, float end) const
{
	std::list<keyframe> keyframe_list;

	auto lower_bound = keyframes.lower_bound({start, T()});
	auto upper_bound = keyframes.upper_bound({end, T()});
	for (auto iterator = lower_bound; iterator != upper_bound; ++iterator)
	{
		keyframe_list.push_back(*iterator);
	}

	return keyframe_list;
}

template <typename T>
inline int animation_channel<T>::get_id() const
{
	return id;
}

template <typename T>
float animation_channel<T>::get_duration() const
{
	if (keyframes.empty())
	{
		return 0.0;
	}
	
	return std::get<0>(*keyframes.rbegin());
}

#endif // ANTKEEPER_ANIMATION_CHANNEL_HPP
