// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_BONE_SET_HPP
#define ANTKEEPER_GAME_ANT_BONE_SET_HPP

#include <engine/animation/bone.hpp>

/// Set of bone indices for all possible bones in an ant skeleotn.
struct ant_bone_set
{
	bone* mesosoma{};
	bone* procoxa_l{};
	bone* profemur_l{};
	bone* protibia_l{};
	bone* protarsomere1_l{};
	bone* procoxa_r{};
	bone* profemur_r{};
	bone* protibia_r{};
	bone* protarsomere1_r{};
	bone* mesocoxa_l{};
	bone* mesofemur_l{};
	bone* mesotibia_l{};
	bone* mesotarsomere1_l{};
	bone* mesocoxa_r{};
	bone* mesofemur_r{};
	bone* mesotibia_r{};
	bone* mesotarsomere1_r{};
	bone* metacoxa_l{};
	bone* metafemur_l{};
	bone* metatibia_l{};
	bone* metatarsomere1_l{};
	bone* metacoxa_r{};
	bone* metafemur_r{};
	bone* metatibia_r{};
	bone* metatarsomere1_r{};
	bone* head{};
	bone* mandible_l{};
	bone* mandible_r{};
	bone* antennomere1_l{};
	bone* antennomere2_l{};
	bone* antennomere1_r{};
	bone* antennomere2_r{};
	bone* petiole{};     // optional
	bone* postpetiole{}; // optional
	bone* gaster{};
	bone* sting{};       // optional
	bone* forewing_l{};  // optional
	bone* forewing_r{};  // optional
	bone* hindwing_l{};  // optional
	bone* hindwing_r{};  // optional
};

#endif // ANTKEEPER_GAME_ANT_BONE_SET_HPP
