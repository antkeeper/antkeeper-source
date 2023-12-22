// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_HPP
#define ANTKEEPER_ANIMATION_HPP

#include <engine/animation/animation-channel.hpp>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>

/**
 * Abstract base class for keyframe animations.
 */
class animation_base
{
public:
	/** Constructs an animation base. */
	animation_base();
	
	/** Destructs an animation base. */
	virtual ~animation_base() = default;
	
	/**
	 * Advances the animation position (t) by @p dt.
	 *
	 * @param dt Delta time by which the animation position will be advanced.
	 */
	virtual void advance(float dt) = 0;
	
	/**
	 * Sets the animation position to @p t.
	 *
	 * @param t Position in time to which the animation position will be set.
	 */
	void seek(float t);
	
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
	 * @param value Speed multiplier.
	 */
	void set_speed(float value);
	
	/// Returns `true` if looping of the animation is enabled, `false` otherwise.
	bool is_looped() const;
	
	/// Returns `true` if the animation is paused, `false` otherwise.
	bool is_paused() const;
	
	/// Returns `true` if the animation is stopped, `false` otherwise.
	bool is_stopped() const;
	
	/// Returns the current position in time of the animation.
	float get_position() const;
	
	/// Returns the current loop count of the animation.
	int get_loop_count() const;
	
	/// Returns the duration of the animation.
	virtual float get_duration() const = 0;
	
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
	float position;
	float speed;

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

inline float animation_base::get_position() const
{
	return position;
}
	
inline int animation_base::get_loop_count() const
{
	return loop_count;
}

/**
 * Keyframe animation.
 *
 * @tparam T Animated data type.
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
	typedef typename std::decay<std::function<T(const T&, const T&, float)>>::type interpolator_type;
	
	/// Creates an animation.
	animation();
	
	/** Destructs an animation. */
	~animation() override = default;
	
	/// @copydoc animation_base::advance()
	virtual void advance(float dt);
	
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
	 * @param interp Frame interpolator function object.
	 */
	void set_interpolator(interpolator_type interp);
	
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
	virtual float get_duration() const;

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
void animation<T>::advance(float dt)
{
	if (paused || stopped)
	{
		return;
	}
	
	// Advance position by dt
	position += dt * speed;
	
	// Determine duration of the animation
	float duration = get_duration();
	
	if (position < duration)
	{
		if (frame_callback != nullptr && interpolator != nullptr)
		{
			for (const auto& channel: channels)
			{
				auto frames = channel.second.find_keyframes(position);
				
				if (frames[0] != nullptr && frames[1] != nullptr)
				{
					// Calculate interpolated frame
					float t0 = std::get<0>(*frames[0]);
					float t1 = std::get<0>(*frames[1]);
					float alpha = (position - t0) / (t1 - t0);
					T frame = interpolator(std::get<1>(*frames[0]), std::get<1>(*frames[1]), alpha);
					
					// Pass frame to frame callback
					frame_callback(channel.first, frame);
				}
				else if (frames[0] != nullptr)
				{
					// Pass frame to frame callback
					frame_callback(channel.first, std::get<1>(*frames[0]));
				}
				else if (frames[1] != nullptr)
				{
					// Pass frame to frame callback
					frame_callback(channel.first, std::get<1>(*frames[1]));
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
				for (auto& channel: channels)
				{
					auto frames = channel.second.find_keyframes(channel.second.get_duration());
					if (frames[0] != nullptr)
					{
						frame_callback(channel.first, std::get<1>(*frames[0]));
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
void animation<T>::set_interpolator(interpolator_type interp)
{
	this->interpolator = interp;
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
float animation<T>::get_duration() const
{
	float duration = 0.0;
	
	for (auto it = channels.begin(); it != channels.end(); ++it)
	{
		duration = std::max<float>(duration, it->second.get_duration());
	}
	
	return duration;
}

#endif // ANTKEEPER_ANIMATION_HPP
