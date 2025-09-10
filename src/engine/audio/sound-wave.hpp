// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/utility/sized-types.hpp>
#include <span>

namespace engine::audio
{
	/// Sound data.
	class sound_wave
	{
	public:
		/// Constructs a sound wave.
		/// @param channels Number of channels. 1 for mono, 2 for stereo.
		/// @param sample_rate Sample rate, in hertz.
		/// @param bits_per_sample Number of bits per sample.
		/// @param samples Sample data.
		sound_wave
		(
			u32 channels,
			u32 sample_rate,
			u32 bits_per_sample,
			std::span<const std::byte> samples
		);
	
		/// Destructs a sound wave.
		~sound_wave();
	
		sound_wave(const sound_wave&) = delete;
		sound_wave(sound_wave&&) = delete;
		sound_wave& operator=(const sound_wave&) = delete;
		sound_wave& operator=(sound_wave&&) = delete;
	
		/// Returns the number of channels in the sound wave.
		[[nodiscard]] inline constexpr auto get_channels() const noexcept
		{
			return m_channels;
		}
	
		/// Returns the sample rate of the sound wave.
		[[nodiscard]] inline constexpr auto get_sample_rate() const noexcept
		{
			return m_sample_rate;
		}
	
		/// Returns the number of bits per sample of the sound wave.
		[[nodiscard]] inline constexpr auto get_bits_per_sample() const noexcept
		{
			return m_bits_per_sample;
		}
	
		/// Returns the size of the sound wave, in bytes.
		[[nodiscard]] inline constexpr auto get_size() const noexcept
		{
			return m_size;
		}
	
		/// Returns the duration of sound wave, in seconds.
		[[nodiscard]] inline constexpr auto get_duration() const noexcept
		{
			return m_duration;
		}

	private:
		friend class sound_que;
	
		u32 m_channels{};
		u32 m_sample_rate{};
		u32 m_bits_per_sample{};
		usize m_size{};
		float m_duration{};
	
		unsigned int m_al_buffer{};
	};
}
