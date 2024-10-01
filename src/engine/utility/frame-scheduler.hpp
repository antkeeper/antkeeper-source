// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_FRAME_SCHEDULER_HPP
#define ANTKEEPER_UTILITY_FRAME_SCHEDULER_HPP

#include <chrono>
#include <functional>
#include <type_traits>

/// Schedules fixed- and variable-rate updates.
/// @see Fiedler, G. (2004). Fix your timestep. Gaffer On Games.
class frame_scheduler
{
public:
	/// Clock type is `std::chrono::high_resolution_clock` if steady, `std::chrono::steady_clock` otherwise. 
	using clock_type = std::conditional
	<
		std::chrono::high_resolution_clock::is_steady,
		std::chrono::high_resolution_clock, std::chrono::steady_clock
	>::type;
	
	/// Duration type matches the clock's duration type.
	using duration_type = clock_type::duration;
	
	/// Time point type matches the clock's time point type.
	using time_point_type = clock_type::time_point;
	
	/// Fixed-rate update callback function type.
	/// @details The first parameter is the elapsed time (`t`) and the second parameter is the fixed-rate update interval (`dt`).
	using fixed_update_callback_type = std::function<void(duration_type, duration_type)>;
	
	/// Variable-rate callback function type.
	/// @details The first parameter is the elapsed time (`t`), the second parameter is the fixed-rate update interval (`dt`), and the third parameter is the accumulated time since the previous fixed-rate update (`at`).
	/// @note The subframe interpolation factor (`alpha`) can be calculated as `at / dt`.
	using variable_update_callback_type = std::function<void(duration_type, duration_type, duration_type)>;
	
	/// Constructs a frame scheduler and starts its frame timer.
	frame_scheduler() noexcept;
	
	/// Performs any scheduled fixed-rate updates followed by a single variable-rate update.
	/// @warning Both the fixed-rate and variable-rate update callbacks must be valid when calling `tick()`.
	void tick();
	
	/// Resets the accumulated time (`at`) and frame timer, but not the elapsed fixed-rate update time.
	void refresh() noexcept;
	
	/// Resets the elapsed fixed-rate update time (`t`), accumulated time (`at`), and frame timer.
	void reset() noexcept;
	
	/// Sets the interval (`dt`) at which fixed-rate updates are scheduled.
	/// @param interval Fixed-rate update interval.
	inline void set_fixed_update_interval(duration_type interval) noexcept
	{
		m_fixed_update_interval = interval;
	}
	
	/// Sets the minimum frame duration. If a frame is quicker than the minimum frame duration, the CPU will be idled until the minimum frame duration is met.
	/// @param duration Minimum frame duration.
	inline void set_min_frame_duration(duration_type duration) noexcept
	{
		m_min_frame_duration = duration;
	}
	
	/// Sets the maximum accumulated frame duration. Prevents a "spiral of death", in which updates are scheduled too many times per frame while trying to catch up to the target update rate.
	/// @param duration Maximum accumulated frame duration.
	inline void set_max_frame_duration(duration_type duration) noexcept
	{
		m_max_frame_duration = duration;
	}
	
	/// Sets the fixed-rate update callback.
	/// @param callback Fixed-rate update callback.
	inline void set_fixed_update_callback(fixed_update_callback_type&& callback) noexcept
	{
		m_fixed_update_callback = callback;
	}

	/// @copydoc set_fixed_update_callback(fixed_update_callback_type&&)
	inline void set_fixed_update_callback(const fixed_update_callback_type& callback) noexcept
	{
		m_fixed_update_callback = callback;
	}

	/// Sets the variable-rate update callback.
	/// @param callback Variable-rate update callback.
	inline void set_variable_update_callback(variable_update_callback_type&& callback) noexcept
	{
		m_variable_update_callback = callback;
	}

	/// @copydoc set_variable_update_callback(variable_update_callback_type&&)
	inline void set_variable_update_callback(const variable_update_callback_type& callback) noexcept
	{
		m_variable_update_callback = callback;
	}
	
	/// Returns the elapsed fixed-rate update time (`t`).
	[[nodiscard]] inline duration_type get_fixed_update_time() const noexcept
	{
		return m_fixed_update_time;
	}
	
	/// Returns the accumulated time (`at`).
	[[nodiscard]] inline duration_type get_accumulated_time() const noexcept
	{
		return m_accumulated_time;
	}
	
	/// Returns the duration of the previous frame.
	[[nodiscard]] inline duration_type get_frame_duration() const noexcept
	{
		return m_frame_duration;
	}
	
	/// Returns the minimum frame duration.
	[[nodiscard]] inline duration_type get_min_frame_duration() const noexcept
	{
		return m_min_frame_duration;
	}
	
	/// Returns the maximum frame duration.
	[[nodiscard]] inline duration_type get_max_frame_duration() const noexcept
	{
		return m_max_frame_duration;
	}
	
	/// Returns the fixed-rate update interval (`dt`).
	[[nodiscard]] inline duration_type get_fixed_update_interval() const noexcept
	{
		return m_fixed_update_interval;
	}
	
	/// Returns the fixed-rate update callback.
	[[nodiscard]] inline const fixed_update_callback_type& get_fixed_update_callback() const noexcept
	{
		return m_fixed_update_callback;
	}
	
	/// Returns the variable-rate update callback.
	[[nodiscard]] inline const variable_update_callback_type& get_variable_update_callback() const noexcept
	{
		return m_variable_update_callback;
	}

private:
	duration_type m_fixed_update_time{};
	duration_type m_accumulated_time{};
	
	time_point_type m_frame_start_time;
	time_point_type m_frame_end_time;
	duration_type m_frame_duration{};
	
	duration_type m_min_frame_duration{};
	duration_type m_max_frame_duration{duration_type::max()};
	
	duration_type m_fixed_update_interval{};
	
	fixed_update_callback_type m_fixed_update_callback;
	variable_update_callback_type m_variable_update_callback;
};

#endif // ANTKEEPER_UTILITY_FRAME_SCHEDULER_HPP
