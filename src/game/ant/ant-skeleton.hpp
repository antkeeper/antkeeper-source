/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_ANT_SKELETON_HPP
#define ANTKEEPER_GAME_ANT_SKELETON_HPP

#include <engine/animation/skeleton.hpp>
#include "game/ant/ant-phenome.hpp"
#include "game/ant/ant-bone-set.hpp"

/**
 * Generates a skeleton for an ant model.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @param[out] bones Ant bone set.
 * @param[in] phenome Ant phenome.
 *
 * The following bones will always be present:
 *
 * * antennomere1_l
 * * antennomere1_r
 * * antennomere2_l
 * * antennomere2_r
 * * gaster
 * * head
 * * mandible_l
 * * mandible_r
 * * mesocoxa_l
 * * mesocoxa_r
 * * mesofemur_l
 * * mesofemur_r
 * * mesosoma
 * * mesotarsus_l
 * * mesotarsus_r
 * * mesotibia_l
 * * mesotibia_r
 * * metacoxa_l
 * * metacoxa_r
 * * metafemur_l
 * * metafemur_r
 * * metatarsus_l
 * * metatarsus_r
 * * metatibia_l
 * * metatibia_r
 * * procoxa_l
 * * procoxa_r
 * * profemur_l
 * * profemur_r
 * * protarsus_l
 * * protarsus_r
 * * protibia_l
 * * protibia_r
 *
 * The presence of the following bones depend on the ant phenome:
 *
 * * forewing_l
 * * forewing_r
 * * hindwing_l
 * * hindwing_r
 * * petiole
 * * postpetiole
 * * sting
 *
 * The following poses will be generated:
 *
 * * rest pose
 * * "pupa"
 */
void generate_ant_skeleton(skeleton& skeleton, ant_bone_set& bones, const ant_phenome& phenome);

#endif // ANTKEEPER_GAME_ANT_SKELETON_HPP
