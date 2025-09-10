// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/audio/playback-state.hpp>
#include <engine/math/vector.hpp>
#include <engine/audio/sound-wave.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <limits>

namespace engine::audio
{
	/// Sound source.
	class sound_que
	{
	public:
		/// Constructs a sound que.
		/// @param wave Sound wave to emit.
		explicit sound_que(std::shared_ptr<sound_wave> wave = nullptr);
	
		/// Destructs a sound que.
		~sound_que();
	
		sound_que(const sound_que&) = delete;
		sound_que(sound_que&&) = delete;
		sound_que& operator=(const sound_que&) = delete;
		sound_que& operator=(sound_que&&) = delete;
	
		/// @name Playback
		/// @{
	
		/// Plays the sound que.
		void play();
	
		/// Stops the sound que.
		void stop();
	
		/// Rewinds the sound que.
		void rewind();
	
		/// Pauses the sound que.
		void pause();
	
		/// Sets the playback position of the sound que.
		/// @param seconds Offset from the start of the sound que, in seconds.
		void seek_seconds(float seconds);
	
		/// Sets the playback position of the sound que.
		/// @param samples Offset from the start of the sound que, in samples.
		void seek_samples(usize samples);
	
		/// Sets the playback position of the sound que.
		/// @param bytes Offset from the start of the sound que, in bytes.
		void seek_bytes(usize bytes);
	
		/// Sets whether the sound que should repeat indefinitely.
		/// @param looping `true` if the sound que should repeat indefinitely, `false` otherwise.
		void set_looping(bool looping);
	
		/// Returns the playback state of the sound que.
		[[nodiscard]] playback_state get_playback_state() const;
	
		/// Returns `true` if the sound que is stopped, `false` otherwise.
		[[nodiscard]] inline bool is_stopped() const
		{
			return get_playback_state() == playback_state::stopped;
		}
	
		/// Returns `true` if the sound que is playing, `false` otherwise.
		[[nodiscard]] inline bool is_playing() const
		{
			return get_playback_state() == playback_state::playing;
		}
	
		/// Returns `true` if the sound que is paused, `false` otherwise.
		[[nodiscard]] inline bool is_paused() const
		{
			return get_playback_state() == playback_state::paused;
		}
	
		/// Returns the playback position, in seconds.
		[[nodiscard]] float get_playback_position_seconds() const;
	
		/// Returns the playback position, in samples.
		[[nodiscard]] usize get_playback_position_samples() const;
	
		/// Returns the playback position, in bytes.
		[[nodiscard]] usize get_playback_position_bytes() const;
	
		/// Returns `true` if the sound que is looping, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_looping() const noexcept
		{
			return m_looping;
		}
	
		/// @}
	
		/// @name Spatiality
		/// @{
	
		/// Sets the position of the sound que.
		/// @param position Position vector.
		void set_position(const math::fvec3& position);
	
		/// Sets the velocity of the sound que.
		/// @param velocity Velocity vector.
		void set_velocity(const math::fvec3& velocity);
	
		/// Sets whether the spatial properties of the sound que are relative to the listener.
		/// @param relative `true` if the position, velocity, cone, and direction of the sound que should be interpreted as relative to the listener, `false` otherwise.
		void set_listener_relative(bool relative);
	
		/// Sets the reference distance for distance attenuation calculations.
		/// @param distance Reference distance.
		void set_reference_distance(float distance);
	
		/// Sets the rolloff factor used for scaling distance attenuation between the reference distance and max distance.
		/// @param factor Rolloff factor.
		void set_rolloff_factor(float factor);
	
		/// Sets the maximum attenuation distance.
		/// @param distance Maximum attenuation distance.
		void set_max_distance(float distance);
	
		/// Returns the position of the sound que.
		[[nodiscard]] inline constexpr const auto& get_position() const noexcept
		{
			return m_position;
		}
	
		/// Returns the velocity of the sound que.
		[[nodiscard]] inline constexpr const auto& get_velocity() const noexcept
		{
			return m_velocity;
		}
	
		/// Returns `true` if the spatial properties of the sound que are relative to the listener, `false` otherwise.
		[[nodiscard]] inline constexpr auto is_listener_relative() const noexcept
		{
			return m_listener_relative;
		}
	
		/// Returns the reference distance for distance attenuation calculations.
		[[nodiscard]] inline constexpr auto get_reference_distance() const noexcept
		{
			return m_reference_distance;
		}
	
		/// Returns the rolloff factor used for scaling distance attenuation.
		[[nodiscard]] inline constexpr auto get_rolloff_factor() const noexcept
		{
			return m_rolloff_factor;
		}
	
		/// Returns the maximum attenuation distance.
		[[nodiscard]] inline constexpr auto get_max_distance() const noexcept
		{
			return m_max_distance;
		}
	
		/// @}
	
		/// @name Directionality
		/// @{
	
		/// Sets the direction of the sound.
		/// @param direction Sound direction vector. A zero vector indicates the sound is omnidirectional.
		void set_direction(const math::fvec3& direction);
	
		/// Sets the inner angle of the sound cone.
		/// @param angle Inner angle of the sound cone, in radians.
		void set_cone_inner_angle(float angle);
	
		/// Sets the outer angle of the sound cone.
		/// @param angle Outer angle of the sound cone, in radians.
		void set_cone_outer_angle(float angle);
	
		/// Sets the gain of the sound outside of the sound cone.
		/// @param gain Gain multiplier outside the sound cone.
		void set_cone_outer_gain(float gain);
	
		/// Returns the direction of the sound. A zero vector indicates the sound is omnidirectional.
		[[nodiscard]] inline constexpr const auto& get_direction() const noexcept
		{
			return m_direction;
		}
	
		/// Returns the inner angle of the sound cone, in radians.
		[[nodiscard]] inline constexpr auto get_cone_inner_angle() const noexcept
		{
			return m_cone_inner_angle;
		}
	
		/// Returns the outer angle of the sound cone, in radians.
		[[nodiscard]] inline constexpr auto get_cone_outer_angle() const noexcept
		{
			return m_cone_outer_angle;
		}
	
		/// Returns the gain multiplier outside the sound cone.
		[[nodiscard]] inline constexpr auto get_cone_outer_gain() const noexcept
		{
			return m_cone_outer_gain;
		}
	
		/// @}
	
		/// @name Gain
		/// @{
	
		/// Sets the gain of the sound.
		/// @param gain Amplitude multiplier.
		void set_gain(float gain);
	
		/// Sets the minimum gain of the sound.
		/// @param gain Minimum gain.
		void set_min_gain(float gain);
	
		/// Sets the maximum gain of the sound.
		/// @param gain Maximum gain.
		void set_max_gain(float gain);
	
		/// Returns the gain of the sound que.
		[[nodiscard]] inline constexpr auto get_gain() const noexcept
		{
			return m_gain;
		}
	
		/// Returns the minimum gain of the sound que.
		[[nodiscard]] inline constexpr auto get_min_gain() const noexcept
		{
			return m_min_gain;
		}
	
		/// Returns the maximum gain of the sound que.
		[[nodiscard]] inline constexpr auto get_max_gain() const noexcept
		{
			return m_max_gain;
		}
	
		/// @}
	
		/// @name Pitch
		/// @{
	
		/// Sets the pitch of the sound que.
		/// @param pitch Pitch multiplier, on (0, inf].
		/// @exception std::out_of_range Sound que pitch out of range (0, inf].
		void set_pitch(float pitch);
	
		/// Returns the pitch of the sound que.
		[[nodiscard]] inline constexpr auto get_pitch() const noexcept
		{
			return m_pitch;
		}
	
		/// @}
	
		/// @name Sound wave
		/// @{
	
		/// Sets the sound wave emitted by the sound que.
		/// @param wave Sound wave to emit. If `nullptr`, no sound will be emitted.
		void set_sound_wave(std::shared_ptr<sound_wave> wave);
	
		/// Returns the sound wave with which the sound que is associated.
		[[nodiscard]] inline constexpr const auto& get_sound_wave() const noexcept
		{
			return m_sound_wave;
		}
	
		/// @}

	private:
		std::shared_ptr<sound_wave> m_sound_wave;
	
		bool m_looping{false};
		math::fvec3 m_position{};
		math::fvec3 m_velocity{};
		bool m_listener_relative{false};
		float m_max_distance{std::numeric_limits<float>::max()};
		float m_rolloff_factor{1.0f};
		float m_reference_distance{1.0f};
		math::fvec3 m_direction{};
		float m_cone_inner_angle{math::two_pi<float>};
		float m_cone_outer_angle{math::two_pi<float>};
		float m_cone_outer_gain{0.0f};
		float m_gain{1.0f};
		float m_min_gain{0.0f};
		float m_max_gain{1.0f};	
		float m_pitch{1.0f};

		unsigned int m_al_source{};
	};
}
