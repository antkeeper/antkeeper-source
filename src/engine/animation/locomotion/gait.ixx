// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.animation.locomotion.gait;
export import engine.animation.locomotion.step;
export import <vector>;

export namespace engine::animation
{
	/// Describes the synchronized timing of limbs in a locomotion pattern.
	struct gait
	{
	public:
		/// Frequency of the gait cycle, in Hz.
		float frequency{};

		/// Array of steps for each limb.
		std::vector<step> steps;

		/// Returns the phase of the gait at the elapsed time.
		/// @param t Elapsed time, in seconds.
		/// @return Gait phase, on `[0, 1]`.
		[[nodiscard]] float phase(float t) const noexcept;
	};
}
