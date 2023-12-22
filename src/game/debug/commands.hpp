// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_COMMANDS_HPP
#define ANTKEEPER_DEBUG_COMMANDS_HPP

#include "game/game.hpp"
#include "game/debug/shell.hpp"

void register_commands(::shell& shell, ::game& ctx);

#endif // ANTKEEPER_DEBUG_COMMANDS_HPP
