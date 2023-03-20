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
 * @tparam T Sample value type.
 */
template <class T>
class moving_average
{
public:
	/// Type of value to average.
	using sample_type = T;
	
	/**
	 * Constructs a moving average
	 *
	 * @param capacity Sample capacity.
	 */
	/// @{
	moving_average(std::size_t capacity):
		m_samples(capacity)
	{}
	moving_average() noexcept = default;
	/// @}
	
	/**
	 * Adds a sample to the moving average. If the moving average has reached its sample capacity, the oldest sample will be discarded.
	 *
	 * @param value Sample value.
	 *
	 * @return Current average value.
	 */
	sample_type operator()(sample_type value) noexcept
	{
		m_sum += value;
		if (m_sample_index < m_samples.size())
		{
			m_samples[m_sample_index] = value;
			++m_sample_index;
			m_average = m_sum / static_cast<sample_type>(m_sample_index);
		}
		else
		{
			sample_type& sample = m_samples[m_sample_index % m_samples.size()];
			m_sum -= sample;
			sample = value;
			++m_sample_index;
			m_average = m_sum / static_cast<sample_type>(m_samples.size());
		}
		
		return m_average;
	}
	
	/**
	 * Resets the moving average.
	 */
	void reset() noexcept
	{
		m_sample_index = 0;
		m_sum = sample_type{0};
		m_average = sample_type{0};
	}
	
	/**
	 * Changes the sample capacity of the moving average.
	 *
	 * @param capacity Sample capacity.
	 */
	void reserve(std::size_t capacity)
	{
		m_samples.resize(capacity, sample_type{0});
	}
	
	/**
	 * Changes the current number of samples of the moving average.
	 *
	 * @param size Number of samples
	 */
	void resize(std::size_t size)
	{
		if (size > m_samples.size())
		{
			m_samples.resize(size);
		}
		m_sample_index = size;
	}
	
	/// Returns a pointer to the sample data.
	[[nodiscard]] inline constexpr sample_type* data() const noexcept
	{
		return m_samples.data();
	}
	
	/// Returns the current moving average value.
	[[nodiscard]] inline sample_type average() const noexcept
	{
		return m_average;
	}
	
	///Returns the sum of all current samples.
	[[nodiscard]] inline sample_type sum() const noexcept
	{
		return m_sum;
	}
	
	/// Returns the current number of samples.
	[[nodiscard]] inline std::size_t size() const noexcept
	{
		return std::min<std::size_t>(m_sample_index, m_samples.size());
	}
	
	/// Returns the maximum number of samples.
	[[nodiscard]] inline constexpr std::size_t capacity() const noexcept
	{
		return m_samples.size();
	}
	
	/// Return `true` if there are currently no samples in the average, `false` otherwise.
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return !m_sample_index;
	}
	
	/// Return `true` if the number of samples in the average has reached its capacity, `false` otherwise.
	[[nodiscard]] inline constexpr bool full() const noexcept
	{
		return m_sample_index >= m_samples.size();
	}
	
private:
	std::vector<sample_type> m_samples;
	std::size_t m_sample_index{0};
	sample_type m_sum{0};
	sample_type m_average{0};
};

} // namespace math

#endif // ANTKEEPER_MATH_MOVING_AVERAGE_HPP
