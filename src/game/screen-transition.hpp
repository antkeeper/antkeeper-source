// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SCREEN_TRANSITION_HPP
#define ANTKEEPER_GAME_SCREEN_TRANSITION_HPP

#include <functional>

class game;

void fade_out_to(::game& ctx, const std::function<void()>& callback);
void fade_in_to(::game& ctx, const std::function<void()>& callback);

#endif // ANTKEEPER_GAME_SCREEN_TRANSITION_HPP
