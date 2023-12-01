// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
 * The following bone hierarchy will be constructed:
 *
 * * mesosoma
 * * * procoxa_l
 * * * * profemur_l
 * * * * * protibia_l
 * * * * * * protarsomere1_l
 * * * procoxa_r
 * * * * profemur_r
 * * * * * protibia_r
 * * * * * * protarsomere1_r
 * * * mesocoxa_l
 * * * * mesofemur_l
 * * * * * mesotibia_l
 * * * * * * mesotarsomere1_l
 * * * mesocoxa_r
 * * * * mesofemur_r
 * * * * * mesotibia_r
 * * * * * * mesotarsomere1_r
 * * * metacoxa_l
 * * * * metafemur_l
 * * * * * metatibia_l
 * * * * * * metatarsomere1_l
 * * * metacoxa_r
 * * * * metafemur_r
 * * * * * metatibia_r
 * * * * * * metatarsomere1_r
 * * * head
 * * * * mandible_l
 * * * * mandible_r
 * * * * antennomere1_l
 * * * * * antennomere2_l
 * * * * antennomere1_r
 * * * * * antennomere2_r
 * * * petiole
 * * * * †postpetiole
 * * * * * gaster
 * * * * * * sting
 * * * †forewing_l
 * * * †forewing_r
 * * * †hindwing_l
 * * * †hindwing_r
 *
 * †: bone presence dependent on phenome.
 *
 * The following poses will be generated:
 *
 * * pupa
 */
void generate_ant_skeleton(skeleton& skeleton, ant_bone_set& bones, const ant_phenome& phenome);

#endif // ANTKEEPER_GAME_ANT_SKELETON_HPP
