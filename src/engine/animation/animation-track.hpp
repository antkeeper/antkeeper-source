// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/animation/animation-curve.hpp>
#include <engine/animation/animation-context.hpp>
#include <engine/utility/sized-types.hpp>
#include <functional>
#include <span>
#include <vector>

namespace engine::animation
{
	/// Set of related animation channels.
	class animation_track
	{
	public:
		/// Output function type.
		/// @details Output functions take two parameters: the track samples, and a reference to an animation context.
		using output_function_type = std::function<void(std::span<const float>, animation_context&)>;

		/// Evaluates the channels of the track at a given time, storing the resulting values in a buffer.
		/// @param[in] time Time at which to sample the track.
		/// @param[in] first_channel Index of the first channel to sample.
		/// @param[out] samples Buffer to store the evaluated values of the channels. The number of channels sampled is limited by the size of the buffer.
		void sample(float time, usize first_channel, std::span<float> samples) const;

		/// Evaluates the channels of the track at a given time, storing the resulting values in a buffer.
		/// @param[in] time Time at which to sample the track.
		/// @param[out] samples Buffer to store the evaluated values of the channels. The number of channels sampled is limited by the size of the buffer.
		void sample(float time, std::span<float> samples) const;

		/// Returns a reference to the channels of the track.
		[[nodiscard]] inline constexpr auto& channels() noexcept
		{
			return m_channels;
		}

		/// @copydoc channels()
		[[nodiscard]] inline constexpr const auto& channels() const noexcept
		{
			return m_channels;
		}

		/// Returns a reference to the output function of the track.
		[[nodiscard]] inline constexpr auto& output() noexcept
		{
			return m_output_function;
		}

		/// @copydoc output()
		[[nodiscard]] inline constexpr const auto& output() const noexcept
		{
			return m_output_function;
		}

		/// Returns the non-negative duration of the track, in seconds.
		[[nodiscard]] float duration() const;

	private:
		std::vector<animation_curve> m_channels;
		output_function_type m_output_function;
	};
}
