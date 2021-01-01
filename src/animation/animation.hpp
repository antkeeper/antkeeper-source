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

#ifndef ANTKEEPER_ANIMATION_HPP
#define ANTKEEPER_ANIMATION_HPP

#include <algorithm>
#include <functional>
#include <set>
#include <tuple>
#include <type_traits>
#include <unordered_map>

/**
 * Abstract base class for animations.
 */
class animation_base
{
public:
	animation_base();
	
	/**
	 * Advances the animation position (t) by @p dt.
	 *
	 * @param dt Delta time by which the animation position will be advanced.
	 */
	virtual void advance(double dt) = 0;
	
	/**
	 * Sets the animation position to @p t.
	 *
	 * @param t Position in time to which the animation position will be set.
	 */
	void seek(double t);
	
	/// Sets the animation position to `0.0`.
	void rewind();
	
	/// Enables or disables looping of the animation.
	void loop(bool enabled);
	
	/// Pauses the animation.
	void pause();
	
	/// Plays the animation.
	void play();
	
	/// Stops the animation, rewinds it, and resets the loop count.
	void stop();
	
	/**
	 * Sets the speed of the animation.
	 *
	 * @param speed Speed multiplier.
	 */
	void set_speed(double speed);
	
	/// Returns `true` if looping of the animation is enabled, `false` otherwise.
	bool is_looped() const;
	
	/// Returns `true` if the animation is paused, `false` otherwise.
	bool is_paused() const;
	
	/// Returns `true` if the animation is stopped, `false` otherwise.
	bool is_stopped() const;
	
	/// Returns the current position in time of the animation.
	double get_position() const;
	
	/// Returns the current loop count of the animation.
	int get_loop_count() const;
	
	/// Returns the duration of the animation.
	virtual double get_duration() const = 0;
	
	/// Sets the callback that's executed when the animation is started from a stopped state.
	void set_start_callback(std::function<void()> callback);
	
	/// Sets the callback that's executed when a non-looped animation has finished.
	void set_end_callback(std::function<void()> callback);
	
	/**
	 * Sets the callback that's executed when the animation loops.
	 *
	 * @param callback Loop callback function which is passed the current loop count.
	 */
	void set_loop_callback(std::function<void(int)> callback);
	
protected:
	bool looped;
	int loop_count;
	bool paused;
	bool stopped;
	double position;
	double speed;

	std::function<void()> start_callback;
	std::function<void()> end_callback;
	std::function<void(int)> loop_callback;
};

inline bool animation_base::is_looped() const
{
	return looped;
}

inline bool animation_base::is_paused() const
{
	return paused;
}

inline bool animation_base::is_stopped() const
{
	return stopped;
}

inline double animation_base::get_position() const
{
	return position;
}
	
inline int animation_base::get_loop_count() const
{
	return loop_count;
}

template <typename T>
class animation_channel
{
public:
	/// Keyframe consisting of a time and a value.
	typedef std::tuple<double, T> keyframe;
	
	/**
	 * Creates an animation channel.
	 *
	 * @param id ID of the channel.
	 */
	animation_channel(int id);
	
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
	void remove_keyframes(double start, double end);
	
	/// Removes all keyframes from the animation.
	void remove_keyframes();
	
	/**
	 * Finds the keyframes to the left and right of @p position.
	 *
	 * @param position Position in time.
	 * @return Array containing the the keyframes on the left and right of @p position.
	 */
	std::array<const keyframe*, 2> find_keyframes(double position) const;
	
	/**
	 * Finds all the keyframes on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 * @return All keyframes on `[start, end)`.
	 */
	std::list<keyframe> find_keyframes(double start, double end) const;
	
	/// Returns the ID of the animation channel.
	int get_id() const;
	
	/// Returns the duration of the animation channel.
	double get_duration() const;
	
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
}

template <typename T>
void animation_channel<T>::insert_keyframe(const keyframe& k)
{
	keyframes.emplace(k);
}

template <typename T>
void animation_channel<T>::remove_keyframes(double start, double end)
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
std::array<const typename animation_channel<T>::keyframe*, 2> animation_channel<T>::find_keyframes(double position) const
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
std::list<typename animation_channel<T>::keyframe> animation_channel<T>::find_keyframes(double start, double end) const
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
double animation_channel<T>::get_duration() const
{
	if (keyframes.empty())
	{
		return 0.0;
	}
	
	return std::get<0>(*keyframes.rbegin());
}

/**
 * Templated keyframe animation class.
 */
template <typename T>
class animation: public animation_base
{
public:
	/// Channel for this animation type.
	typedef animation_channel<T> channel;
	
	// Keyframe type for this animation.
	typedef typename channel::keyframe keyframe;
	
	/// Interpolator function type.
	typedef typename std::decay<std::function<T(const T&, const T&, double)>>::type interpolator_type;
	
	/// Creates an animation.
	animation();
	
	/// @copydoc animation_base::advance()
	virtual void advance(double dt);
	
	/**
	 * Adds a channel to the animation.
	 *
	 * @param id ID of the channel.
	 * @return Added or pre-existing channel.
	 */
	channel* add_channel(int id);
	
	/**
	 * Removes a channel from the animation.
	 *
	 * @param id ID of the channel to remove.
	 */
	void remove_channel(int id);
	
	/// Removes all channels from the animation.
	void remove_channels();
	
	/**
	 * Sets the frame interpolator function object.
	 *
	 * @param interpolator Frame interpolator function object.
	 */
	void set_interpolator(interpolator_type interpolator);
	
	/**
	 * Sets the callback that's executed on each frame of animation.
	 *
	 * @param callback Frame callback which receives the index of an animation channel and value of an interpolated frame.
	 */
	void set_frame_callback(std::function<void(int, const T&)> callback);
	
	/**
	 * Returns the channel with the specified ID.
	 *
	 * @param id ID of the channel to get.
	 */
	const channel* get_channel(int id) const;
	
	/// @copydoc animation::get_channel(int) const
	channel* get_channel(int id);
	
	/// @copydoc animation_base::get_duration() const
	virtual double get_duration() const;

private:
	std::unordered_map<int, channel> channels;
	interpolator_type interpolator;
	std::function<void(int, const T&)> frame_callback;
};

template <typename T>
animation<T>::animation():
	interpolator(nullptr),
	frame_callback(nullptr)
{}

template <typename T>
void animation<T>::advance(double dt)
{
	if (paused || stopped)
	{
		return;
	}
	
	// Advance position by dt
	position += dt * speed;
	
	// Determine duration of the animation
	double duration = get_duration();
	
	if (position < duration)
	{
		if (frame_callback != nullptr && interpolator != nullptr)
		{
			for (std::size_t i = 0; i < channels.size(); ++i)
			{
				auto frames = channels[i].find_keyframes(position);
				
				if (frames[0] != nullptr && frames[1] != nullptr)
				{
					// Calculate interpolated frame
					double t0 = std::get<0>(*frames[0]);
					double t1 = std::get<0>(*frames[1]);
					double alpha = (position - t0) / (t1 - t0);
					T frame = interpolator(std::get<1>(*frames[0]), std::get<1>(*frames[1]), alpha);
					
					// Pass frame to frame callback
					frame_callback(static_cast<int>(i), frame);
				}
				else if (frames[0] != nullptr)
				{
					// Pass frame to frame callback
					frame_callback(static_cast<int>(i), std::get<1>(*frames[0]));
				}
				else if (frames[1] != nullptr)
				{
					// Pass frame to frame callback
					frame_callback(static_cast<int>(i), std::get<1>(*frames[1]));
				}
			}
		}
	}
	else
	{
		if (looped)
		{
			++loop_count;
			
			// Subtract duration of animation from position
			position -= duration;
			
			// Execute loop callback
			if (loop_callback)
			{
				loop_callback(loop_count);
			}
			
			// Call frame callback on looped frame
			if (frame_callback)
			{
				advance(0.0);
			}
		}
		else
		{
			// Call frame callback for end frame
			if (frame_callback != nullptr)
			{
				for (std::size_t i = 0; i < channels.size(); ++i)
				{
					auto frames = channels[i].find_keyframes(channels[i].get_duration());
					if (frames[0] != nullptr)
					{
						frame_callback(static_cast<int>(i), std::get<1>(*frames[0]));
					}
				}
			}
			
			stopped = true;
			
			// Call end callback
			if (end_callback)
			{
				end_callback();
			}
		}
	}
}

template <typename T>
typename animation<T>::channel* animation<T>::add_channel(int id)
{
	return &(*channels.emplace(id, id).first).second;
}

template <typename T>
void animation<T>::remove_channel(int id)
{
	auto it = channels.find(id);
	if (it != channels.end())
	{
		channels.erase(it);
	}
}

template <typename T>
void animation<T>::remove_channels()
{
	channels.clear();
}

template <typename T>
void animation<T>::set_interpolator(interpolator_type interpolator)
{
	this->interpolator = interpolator;
}

template <typename T>
void animation<T>::set_frame_callback(std::function<void(int, const T&)> callback)
{
	this->frame_callback = callback;
}

template <typename T>
const typename animation<T>::channel* animation<T>::get_channel(int id) const
{
	auto it = channels.find(id);
	if (it != channels.end())
	{
		return &it->second;
	}
	
	return nullptr;
}

template <typename T>
typename animation<T>::channel* animation<T>::get_channel(int id)
{
	auto it = channels.find(id);
	if (it != channels.end())
	{
		return &it->second;
	}
	
	return nullptr;
}

template <typename T>
double animation<T>::get_duration() const
{
	double duration = 0.0;
	
	for (auto it = channels.begin(); it != channels.end(); ++it)
	{
		duration = std::max<double>(duration, it->second.get_duration());
	}
	
	return duration;
}

#endif // ANTKEEPER_ANIMATION_HPP
