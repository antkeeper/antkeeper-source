// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.animation.animation_sequence;
import engine.animation.animation_track;
import engine.animation.animation_context;
export import <functional>;
export import <map>;
export import <string>;

export namespace engine::animation
{
	/// Set of related animation tracks.
	class animation_sequence
	{
	public:
		/// Triggers all cues on the half-open interval [@p start_time, @p end_time).
		/// @param start_time Start of the interval (inclusive).
		/// @param end_time End of the interval (exclusive).
		/// @param context Animation context.
		void trigger_cues(float start_time, float end_time, animation_context& context) const;

		/// Returns a reference to the name of the sequence.
		[[nodiscard]] inline constexpr auto& name() noexcept
		{
			return m_name;
		}

		/// @copydoc name()
		[[nodiscard]] inline constexpr const auto& name() const noexcept
		{
			return m_name;
		}

		/// Returns a reference to the animation tracks of the sequence.
		[[nodiscard]] inline constexpr auto& tracks() noexcept
		{
			return m_tracks;
		}

		/// @copydoc tracks()
		[[nodiscard]] inline constexpr const auto& tracks() const noexcept
		{
			return m_tracks;
		}

		/// Returns a reference to the animation cues of the sequence.
		/// @details Cues are function objects mapped to times in the sequence. Cues take one parameter: a reference to an animation context.
		/// @note Cues do not affect the duration of the sequence.
		[[nodiscard]] inline constexpr auto& cues() noexcept
		{
			return m_cues;
		}

		/// @copydoc cues()
		[[nodiscard]] inline constexpr const auto& cues() const noexcept
		{
			return m_cues;
		}

		/// Returns the non-negative duration of the sequence.
		[[nodiscard]] float duration() const;

	private:
		std::string m_name;
		std::map<std::string, animation_track> m_tracks;
		std::multimap<float, std::function<void(animation_context&)>> m_cues;
	};
}
