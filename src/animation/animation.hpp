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

#ifndef ANTKEEPER_ANIMATION_HPP
#define ANTKEEPER_ANIMATION_HPP

#include <functional>
#include <set>
#include <tuple>
#include <type_traits>

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

/**
 * Templated keyframe animation class.
 */
template <typename T>
class animation: public animation_base
{
public:
	/// Scheduled function consisting of a time and function object.
	typedef std::tuple<double, T> keyframe;
	
	/// Interpolator function type.
	typedef typename std::decay<std::function<T(const T&, const T&, double)>>::type interpolator_type;
	
	/// Creates an animation
	animation();
	
	/// @copydoc animation_base::advance()
	virtual void advance(double dt);
	
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
	
	/**
	 * Returns all the keyframes on `[start, end)`.
	 *
	 * @param start Starting position in time (inclusive).
	 * @param end Ending position in time (non-inclusive).
	 * @return All keyframes on `[start, end)`.
	 */
	std::list<keyframe> get_keyframes(double start, double end) const;
	
	/// Removes all keyframes from the animation.
	void clear();
	
	/**
	 * Sets the frame interpolator function object.
	 *
	 * @param interpolator Frame interpolator function object.
	 */
	void set_interpolator(interpolator_type interpolator);
	
	/**
	 * Sets the callback that's executed on each frame of animation.
	 *
	 * @param callback Frame callback which receives the value of the interpolated frames.
	 */
	void set_frame_callback(std::function<void(const T&)> callback);

private:
	//static bool keyframe_compare(const keyframe& a, const keyframe & b);
	
	struct keyframe_compare
	{
		inline bool operator()(const keyframe& lhs, const keyframe& rhs) const
		{
			return std::get<0>(lhs) < std::get<0>(rhs);
		}
	};
	
	interpolator_type interpolator;
	std::function<void(const T&)> frame_callback;
	std::set<keyframe, keyframe_compare> keyframes;
};

/*
template <typename T>
bool animation<T>::keyframe_compare(const keyframe& a, const keyframe & b)
{
	return std::get<0>(a) < std::get<0>(b);
}
*/

template <typename T>
animation<T>::animation():
	interpolator(nullptr),
	frame_callback(nullptr),
	keyframes(keyframe_compare())
{}

template <typename T>
void animation<T>::advance(double dt)
{
	if (paused || stopped || keyframes.empty())
	{
		return;
	}
	
	// Advance position by dt
	position += dt * speed;
	
	// Find the following keyframe
	auto upper_bound = keyframes.upper_bound({position, T()});
	
	// Find the preceding keyframe
	auto lower_bound = upper_bound;
	--lower_bound;
	
	if (upper_bound != keyframes.end())
	{
		if (frame_callback != nullptr && interpolator != nullptr)
		{
			// Calculate interpolated frame
			double t0 = std::get<0>(*lower_bound);
			double t1 = std::get<0>(*upper_bound);
			double alpha = (position - t0) / (t1 - t0);
			T frame = interpolator(std::get<1>(*lower_bound), std::get<1>(*upper_bound), alpha);
			
			// Pass frame to frame callback
			frame_callback(frame);
		}
	}
	else
	{
		if (looped)
		{
			++loop_count;
			
			// Subtract duration of animation from position
			position -= std::get<0>(*lower_bound);
			
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
			// Call frame callback on final keyframe
			if (frame_callback)
			{
				frame_callback(std::get<1>(*lower_bound));
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
void animation<T>::insert_keyframe(const keyframe& k)
{
	keyframes.emplace(k);
}

template <typename T>
void animation<T>::remove_keyframes(double start, double end)
{
	auto lower_bound = keyframes.lower_bound({start, T()});
	auto upper_bound = keyframes.upper_bound({end, T()});
	keyframes.erase(lower_bound, upper_bound);
}

template <typename T>
std::list<typename animation<T>::keyframe> animation<T>::get_keyframes(double start, double end) const
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
void animation<T>::clear()
{
	keyframes.clear();
}

template <typename T>
void animation<T>::set_interpolator(interpolator_type interpolator)
{
	this->interpolator = interpolator;
}

template <typename T>
void animation<T>::set_frame_callback(std::function<void(const T&)> callback)
{
	this->frame_callback = callback;
}

#endif // ANTKEEPER_ANIMATION_HPP
