// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/frame-scheduler.hpp>
#include <engine/debug/contract.hpp>
#include <algorithm>
#include <thread>

frame_scheduler::frame_scheduler() noexcept
{
	m_frame_start_time = clock_type::now();
}

void frame_scheduler::tick()
{
	debug::precondition(m_fixed_update_callback != nullptr);
	debug::precondition(m_variable_update_callback != nullptr);

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
	m_variable_update_callback(m_fixed_update_time, m_fixed_update_interval, m_accumulated_time);
}

void frame_scheduler::refresh() noexcept
{
	m_accumulated_time = {};
	m_frame_duration = {};
	m_frame_start_time = clock_type::now();
}

void frame_scheduler::reset() noexcept
{
	m_fixed_update_time = {};
	refresh();
}
