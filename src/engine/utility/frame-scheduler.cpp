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

#include <engine/utility/frame-scheduler.hpp>
#include <algorithm>
#include <thread>

frame_scheduler::frame_scheduler() noexcept
{
	m_frame_start_time = clock_type::now();
}

void frame_scheduler::tick()
{
	// Measure duration of previous frame
	m_frame_end_time = clock_type::now();
	m_frame_duration = m_frame_end_time - m_frame_start_time;
	
	// Idle until the minimum frame duration has passed
	if (m_frame_duration < m_min_frame_duration)
	{
		// Determine time to idle until
		const time_point_type idle_time = m_frame_end_time + m_min_frame_duration - m_frame_duration;
		
		// Idle
		do
		{
			std::this_thread::yield();
			m_frame_end_time = clock_type::now();
		}
		while (m_frame_end_time < idle_time);
		
		// Measure duration of previous frame with idle
		m_frame_duration = m_frame_end_time - m_frame_start_time;
	}
	
	// Accumulate previous frame duration (with limit to prevent "spiral of death")
	m_accumulated_time += std::min<duration_type>(m_max_frame_duration, m_frame_duration);
	
	// Start measuring duration of next frame
	m_frame_start_time = m_frame_end_time;
	
	// Perform fixed-rate updates
	while (m_accumulated_time >= m_fixed_update_interval)
	{
		m_fixed_update_callback(m_fixed_update_time, m_fixed_update_interval);
		
		m_fixed_update_time += m_fixed_update_interval;
		m_accumulated_time -= m_fixed_update_interval;
	}
	
	// Perform variable-rate update
	m_variable_update_callback(m_fixed_update_time + m_accumulated_time, m_fixed_update_interval, m_accumulated_time);
}

void frame_scheduler::refresh() noexcept
{
	m_accumulated_time = duration_type::zero();
	m_frame_duration = duration_type::zero();
	m_frame_start_time = clock_type::now();
}

void frame_scheduler::reset() noexcept
{
	m_fixed_update_time = duration_type::zero();
	refresh();
}
