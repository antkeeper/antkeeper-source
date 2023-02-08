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

#ifndef ANTKEEPER_MATH_MOVING_AVERAGE_HPP
#define ANTKEEPER_MATH_MOVING_AVERAGE_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

namespace math {

/**
 * Calculates a moving average.
 * 
 * @tparam T Value type.
 * @tparam N Sample capacity.
 */
template <class T, std::size_t N>
class moving_average
{
public:
	/// Type of value to average.
	typedef T value_type;
	
	/// Constructs a moving average
	moving_average():
		m_sample_index{0},
		m_sum{0},
		m_average{0}
	{}
	
	/**
	 * Adds a sample to the moving average. If the moving average has reached its sample capacity, the oldest sample will be discarded.
	 *
	 * @param value Sample value.
	 *
	 * @return Current average value.
	 */
	value_type operator()(value_type value) noexcept
	{
		m_sum += value;
		if (m_sample_index < N)
		{
			m_samples[m_sample_index] = value;
			++m_sample_index;
			m_average = m_sum / static_cast<value_type>(m_sample_index);
		}
		else
		{
			value_type& sample = m_samples[m_sample_index % N];
			m_sum -= sample;
			sample = value;
			++m_sample_index;
			m_average = m_sum / static_cast<value_type>(N);
		}
		
		return m_average;
	}
	
	/**
	 * Resets the moving average.
	 */
	void reset() noexcept
	{
		m_sample_index = 0;
		m_sum = value_type{0};
		m_average = value_type{0};
	}
	
	/// Returns a pointer to the sample data.
	[[nodiscard]] inline constexpr value_type* data() const noexcept
	{
		return m_samples.data();
	}
	
	/// Returns the current moving average value.
	[[nodiscard]] inline value_type average() const noexcept
	{
		return m_average;
	}
	
	///Returns the sum of all current samples.
	[[nodiscard]] inline value_type sum() const noexcept
	{
		return m_sum;
	}
	
	/// Returns the current number of samples.
	[[nodiscard]] inline std::size_t size() const noexcept
	{
		return std::min<std::size_t>(m_sample_index, N);
	}
	
	/// Returns the maximum number of samples.
	[[nodiscard]] inline constexpr std::size_t capacity() const noexcept
	{
		return N;
	}
	
	/// Return `true` if there are currently no samples in the average, `false` otherwise.
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return !m_sample_index;
	}
	
	/// Return `true` if the number of samples in the average has reached its capacity, `false` otherwise.
	[[nodiscard]] inline constexpr bool full() const noexcept
	{
		return m_sample_index >= N;
	}
	
private:
	std::size_t m_sample_index;
	value_type m_samples[N];
	value_type m_sum;
	value_type m_average;
};

} // namespace math

#endif // ANTKEEPER_MATH_MOVING_AVERAGE_HPP
