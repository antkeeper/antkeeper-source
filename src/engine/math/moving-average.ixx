// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.moving_average;
import engine.utility.sized_types;
import engine.math.functions;
import <vector>;

export namespace engine::math
{
	/// Calculates a moving average.
	/// @tparam T Sample value type.
	template <class T>
	class moving_average
	{
	public:
		/// Type of value to average.
		using sample_type = T;

		/// Constructs a moving average.
		/// @param capacity Sample capacity.
		explicit moving_average(usize capacity):
			m_samples(capacity)
		{
		}

		/// Constructs a moving average.
		moving_average() noexcept = default;

		/// Adds a sample to the moving average. If the moving average has reached its sample capacity, the oldest sample will be discarded.
		/// @param value Sample value.
		/// @return Current average value.
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

		/// Resets the moving average.
		void reset() noexcept
		{
			m_sample_index = 0;
			m_sum = sample_type{0};
			m_average = sample_type{0};
		}

		/// Changes the sample capacity of the moving average.
		/// @param capacity Sample capacity.
		void reserve(usize capacity)
		{
			m_samples.resize(capacity, sample_type{0});
		}

		/// Changes the current number of samples of the moving average.
		/// @param size Number of samples
		void resize(usize size)
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
		[[nodiscard]] inline usize size() const noexcept
		{
			return min(m_sample_index, m_samples.size());
		}

		/// Returns the maximum number of samples.
		[[nodiscard]] inline constexpr usize capacity() const noexcept
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
		usize m_sample_index{0};
		sample_type m_sum{0};
		sample_type m_average{0};
	};
}
