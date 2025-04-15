// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_BONE_SET_HPP
#define ANTKEEPER_GAME_ANT_BONE_SET_HPP

import engine.animation.bone;

using namespace engine;

/// Set of bone indices for all possible bones in an ant skeleotn.
struct ant_bone_set
{
	animation::bone* mesosoma{};
	animation::bone* procoxa_l{};
	animation::bone* profemur_l{};
	animation::bone* protibia_l{};
	animation::bone* protarsomere1_l{};
	animation::bone* procoxa_r{};
	animation::bone* profemur_r{};
	animation::bone* protibia_r{};
	animation::bone* protarsomere1_r{};
	animation::bone* mesocoxa_l{};
	animation::bone* mesofemur_l{};
	animation::bone* mesotibia_l{};
	animation::bone* mesotarsomere1_l{};
	animation::bone* mesocoxa_r{};
	animation::bone* mesofemur_r{};
	animation::bone* mesotibia_r{};
	animation::bone* mesotarsomere1_r{};
	animation::bone* metacoxa_l{};
	animation::bone* metafemur_l{};
	animation::bone* metatibia_l{};
	animation::bone* metatarsomere1_l{};
	animation::bone* metacoxa_r{};
	animation::bone* metafemur_r{};
	animation::bone* metatibia_r{};
	animation::bone* metatarsomere1_r{};
	animation::bone* head{};
	animation::bone* mandible_l{};
	animation::bone* mandible_r{};
	animation::bone* antennomere1_l{};
	animation::bone* antennomere2_l{};
	animation::bone* antennomere1_r{};
	animation::bone* antennomere2_r{};
	animation::bone* petiole{};     // optional
	animation::bone* postpetiole{}; // optional
	animation::bone* gaster{};
	animation::bone* sting{};       // optional
	animation::bone* forewing_l{};  // optional
	animation::bone* forewing_r{};  // optional
	animation::bone* hindwing_l{};  // optional
	animation::bone* hindwing_r{};  // optional
};

#endif // ANTKEEPER_GAME_ANT_BONE_SET_HPP
