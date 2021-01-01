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

#ifndef ANTKEEPER_GAME_STATES_HPP
#define ANTKEEPER_GAME_STATES_HPP

struct game_context;

void loading_state_enter(game_context* ctx);
void loading_state_exit(game_context* ctx);
void language_select_state_enter(game_context* ctx);
void language_select_state_exit(game_context* ctx);
void splash_state_enter(game_context* ctx);
void splash_state_exit(game_context* ctx);
void title_state_enter(game_context* ctx);
void title_state_exit(game_context* ctx);
void play_state_enter(game_context* ctx);
void play_state_exit(game_context* ctx);
void pause_state_enter(game_context* ctx);
void pause_state_exit(game_context* ctx);

#endif // ANTKEEPER_GAME_STATES_HPP
