// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ai.steering.behavior.flee;
export import engine.math.vector;
import engine.ai.steering.agent;

export namespace engine::ai::steering::behavior
{
	/// Attempts to steer an agent so that it moves away from a target.
	/// @param agent Autonomous agent to steer.
	/// @param target Target position.
	/// @return Flee force.
	[[nodiscard]] math::fvec3 flee(const agent& agent, const math::fvec3& target);
}
