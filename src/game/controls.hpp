/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_CONTROLS_HPP
#define ANTKEEPER_GAME_CONTROLS_HPP

#include "game/context.hpp"
#include "resources/json.hpp"
#include "input/gamepad.hpp"
#include <filesystem>

namespace game {

/**
 * Applies a control profile to the game context.
 *
 * @param ctx Game context.
 * @param profile Control profile.
 */
void apply_control_profile(game::context& ctx, const json& profile);

/**
 * Saves the current control profile.
 *
 * @param ctx Game context.
 */
void save_control_profile(game::context& ctx);

/**
 * Generates a default control profile.
 *
 * @return Default control profile.
 */
json default_control_profile();

/**
 * Returns a string containing the path to the gamepad calibration file.
 */
std::filesystem::path gamepad_calibration_path(const game::context& ctx, const input::gamepad& gamepad);

/**
 * Generates default gamepad calibration settings.
 *
 * @return Default gamepad calibration settings.
 */
json default_gamepad_calibration();

/**
 * Loads gamepad calibration settings.
 *
 * @param ctx Game context.
 * @param gamepad Gamepad for which to load calibration settings.
 * @return Gamepad calibration settings, or `nullptr` if not loaded.
 */
json* load_gamepad_calibration(game::context& ctx, const input::gamepad& gamepad);

/**
 * Saves gamepad calibration settings.
 *
 * @param ctx Game context.
 * @param gamepad Gamepad for which to save calibration settings.
 * @return `true` if calibration settings were successfully saved, `false` otherwise.
 */
bool save_gamepad_calibration(const game::context& ctx, const input::gamepad& gamepad, const json& calibration);

/**
 * Applies gamepad calibration settings.
 *
 * @param gamepad Gamepad to calibrate.
 * @param calibration JSON element containing gamepad calibration settings.
 */
void apply_gamepad_calibration(input::gamepad& gamepad, const json& calibration);

} // namespace game

#endif // ANTKEEPER_GAME_CONTROLS_HPP
