// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_MORPHOGENESIS_HPP
#define ANTKEEPER_GAME_ANT_MORPHOGENESIS_HPP

#include "game/ant/ant-phenome.hpp"
import engine.render.model;
import <memory>;

using namespace engine;

/// Generates a 3D model of an ant given its phenome.
/// @param phenome Ant phenome.
/// @return Generated 3D model of the given phenome.
[[nodiscard]] std::unique_ptr<render::model> ant_morphogenesis(const ant_phenome& phenome);

#endif // ANTKEEPER_GAME_ANT_MORPHOGENESIS_HPP
