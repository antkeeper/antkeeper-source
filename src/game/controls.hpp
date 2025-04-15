// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CONTROLS_HPP
#define ANTKEEPER_GAME_CONTROLS_HPP

#include "game/control-profile.hpp"

class game;

/// Resets a control profile to default settings.
/// @param profile Control profile to reset.
void reset_control_profile(::control_profile& profile);

/// Applies a control profile to the game context.
/// @param ctx Game context.
/// @param profile Control profile to apply.
/// @param key `true` to apply key mappings, `false` otherwise.
/// @param mouse `true` to apply mouse mappings, `false` otherwise.
/// @param gamepad `true` to apply gamepad mappings, `false` otherwise.
void apply_control_profile(::game& ctx, const ::control_profile& profile, bool key, bool mouse, bool gamepad);

/// Updates a control profile after actions have been remapped.
/// @param ctx Game context.
/// @param profile Control profile to update.
void update_control_profile(::game& ctx, ::control_profile& profile);

void setup_window_controls(::game& ctx);
void setup_menu_controls(::game& ctx);
void setup_game_controls(::game& ctx);
void setup_camera_controls(::game& ctx);
void setup_ant_controls(::game& ctx);
void setup_debug_controls(::game& ctx);
void setup_terminal_controls(::game& ctx);

void enable_window_controls(::game& ctx);
void enable_menu_controls(::game& ctx);
void enable_game_controls(::game& ctx);
void enable_camera_controls(::game& ctx);
void enable_ant_controls(::game& ctx);
void enable_debug_controls(::game& ctx);
void enable_terminal_controls(::game& ctx);

void disable_window_controls(::game& ctx);
void disable_menu_controls(::game& ctx);
void disable_game_controls(::game& ctx);
void disable_camera_controls(::game& ctx);
void disable_ant_controls(::game& ctx);
void disable_debug_controls(::game& ctx);
void disable_terminal_controls(::game& ctx);

#endif // ANTKEEPER_GAME_CONTROLS_HPP
