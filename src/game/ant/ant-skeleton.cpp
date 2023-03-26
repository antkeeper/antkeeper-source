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

#include "game/ant/ant-skeleton.hpp"
#include "game/ant/ant-bone-set.hpp"
#include <engine/math/angles.hpp>

namespace {


/**
 * Generates the rest pose of ant skeleton.
 *
 * @param[in] phenome Ant phenome.
 * @param[in] bones Set of ant skeleton bones.
 * @param[in,out] skeleton Ant skeleton.
 */
void generate_ant_rest_pose(::skeleton& skeleton, const ant_bone_set& bones, const ant_phenome& phenome)
{
	// Get skeletons of individual body parts
	const ::skeleton& mesosoma_skeleton = phenome.mesosoma->model->get_skeleton();
	const ::skeleton& legs_skeleton = phenome.legs->model->get_skeleton();
	const ::skeleton& head_skeleton = phenome.head->model->get_skeleton();
	const ::skeleton& mandibles_skeleton = phenome.mandibles->model->get_skeleton();
	const ::skeleton& antennae_skeleton = phenome.antennae->model->get_skeleton();
	const ::skeleton& waist_skeleton = phenome.waist->model->get_skeleton();
	const ::skeleton& gaster_skeleton = phenome.gaster->model->get_skeleton();
	const ::skeleton* sting_skeleton = (phenome.sting->present) ? &phenome.sting->model->get_skeleton() : nullptr;
	const ::skeleton* forewings_skeleton = (phenome.wings->present) ? &phenome.wings->forewings_model->get_skeleton() : nullptr;
	const ::skeleton* hindwings_skeleton = (phenome.wings->present) ? &phenome.wings->hindwings_model->get_skeleton() : nullptr;
	
	auto get_bone_transform = [](const ::skeleton& skeleton, hash::fnv1a32_t bone_name)
	{
		return skeleton.get_rest_pose().get_relative_transform(*skeleton.get_bone_index(bone_name));
	};
	
	const auto& mesosoma_transform = get_bone_transform(mesosoma_skeleton, "mesosoma");
	const auto& head_socket_transform = get_bone_transform(mesosoma_skeleton, "head");
	const auto& head_transform = get_bone_transform(head_skeleton, "head");
	
	const auto inverse_mesosoma_transform = math::inverse(mesosoma_transform);
	
	// Build skeleton rest pose
	skeleton.set_bone_transform(bones.mesosoma, mesosoma_transform);
	
	skeleton.set_bone_transform(bones.procoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "procoxa_l"));
	skeleton.set_bone_transform(bones.profemur_l, get_bone_transform(legs_skeleton, "profemur_l"));
	skeleton.set_bone_transform(bones.protibia_l, get_bone_transform(legs_skeleton, "protibia_l"));
	skeleton.set_bone_transform(bones.protarsus_l, get_bone_transform(legs_skeleton, "protarsus1_l"));
	
	skeleton.set_bone_transform(bones.procoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "procoxa_r"));
	skeleton.set_bone_transform(bones.profemur_r, get_bone_transform(legs_skeleton, "profemur_r"));
	skeleton.set_bone_transform(bones.protibia_r, get_bone_transform(legs_skeleton, "protibia_r"));
	skeleton.set_bone_transform(bones.protarsus_r, get_bone_transform(legs_skeleton, "protarsus1_r"));
	
	skeleton.set_bone_transform(bones.mesocoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "mesocoxa_l"));
	skeleton.set_bone_transform(bones.mesofemur_l, get_bone_transform(legs_skeleton, "mesofemur_l"));
	skeleton.set_bone_transform(bones.mesotibia_l, get_bone_transform(legs_skeleton, "mesotibia_l"));
	skeleton.set_bone_transform(bones.mesotarsus_l, get_bone_transform(legs_skeleton, "mesotarsus1_l"));
	
	skeleton.set_bone_transform(bones.mesocoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "mesocoxa_r"));
	skeleton.set_bone_transform(bones.mesofemur_r, get_bone_transform(legs_skeleton, "mesofemur_r"));
	skeleton.set_bone_transform(bones.mesotibia_r, get_bone_transform(legs_skeleton, "mesotibia_r"));
	skeleton.set_bone_transform(bones.mesotarsus_r, get_bone_transform(legs_skeleton, "mesotarsus1_r"));
	
	skeleton.set_bone_transform(bones.metacoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "metacoxa_l"));
	skeleton.set_bone_transform(bones.metafemur_l, get_bone_transform(legs_skeleton, "metafemur_l"));
	skeleton.set_bone_transform(bones.metatibia_l, get_bone_transform(legs_skeleton, "metatibia_l"));
	skeleton.set_bone_transform(bones.metatarsus_l, get_bone_transform(legs_skeleton, "metatarsus1_l"));
	
	skeleton.set_bone_transform(bones.metacoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "metacoxa_r"));
	skeleton.set_bone_transform(bones.metafemur_r, get_bone_transform(legs_skeleton, "metafemur_r"));
	skeleton.set_bone_transform(bones.metatibia_r, get_bone_transform(legs_skeleton, "metatibia_r"));
	skeleton.set_bone_transform(bones.metatarsus_r, get_bone_transform(legs_skeleton, "metatarsus1_r"));
	
	skeleton.set_bone_transform(bones.head, head_socket_transform * head_transform);
	skeleton.set_bone_transform(bones.mandible_l, get_bone_transform(head_skeleton, "mandible_l") * get_bone_transform(mandibles_skeleton, "mandible_l"));
	skeleton.set_bone_transform(bones.mandible_r, get_bone_transform(head_skeleton, "mandible_r") * get_bone_transform(mandibles_skeleton, "mandible_r"));
	skeleton.set_bone_transform(bones.antennomere1_l, get_bone_transform(head_skeleton, "antenna_l") * get_bone_transform(antennae_skeleton, "antennomere1_l"));
	skeleton.set_bone_transform(bones.antennomere2_l, get_bone_transform(antennae_skeleton, "antennomere2_l"));
	skeleton.set_bone_transform(bones.antennomere1_r, get_bone_transform(head_skeleton, "antenna_r") * get_bone_transform(antennae_skeleton, "antennomere1_r"));
	skeleton.set_bone_transform(bones.antennomere2_r, get_bone_transform(antennae_skeleton, "antennomere2_r"));
	
	if (bones.petiole)
	{
		skeleton.set_bone_transform(*bones.petiole, get_bone_transform(mesosoma_skeleton, "petiole") * get_bone_transform(waist_skeleton, "petiole"));
	}
	
	if (bones.postpetiole)
	{
		skeleton.set_bone_transform(*bones.postpetiole, get_bone_transform(waist_skeleton, "postpetiole"));
	}
	
	skeleton.set_bone_transform(bones.gaster, get_bone_transform(waist_skeleton, "gaster") * get_bone_transform(gaster_skeleton, "gaster"));
	
	if (bones.sting)
	{
		skeleton.set_bone_transform(*bones.sting, get_bone_transform(gaster_skeleton, "sting") * get_bone_transform(*sting_skeleton, "sting"));
	}
	
	if (bones.forewing_l)
	{
		skeleton.set_bone_transform(*bones.forewing_l, get_bone_transform(mesosoma_skeleton, "forewing_l") * get_bone_transform(*forewings_skeleton, "forewing_l"));
		skeleton.set_bone_transform(*bones.forewing_r, get_bone_transform(mesosoma_skeleton, "forewing_r") * get_bone_transform(*forewings_skeleton, "forewing_r"));
		skeleton.set_bone_transform(*bones.hindwing_l, get_bone_transform(mesosoma_skeleton, "hindwing_l") * get_bone_transform(*hindwings_skeleton, "hindwing_l"));
		skeleton.set_bone_transform(*bones.hindwing_r, get_bone_transform(mesosoma_skeleton, "hindwing_r") * get_bone_transform(*hindwings_skeleton, "hindwing_r"));
	}
	
	skeleton.update_rest_pose();
}

/**
 * Generates a pupa pose (named "pupa") for an ant skeleton.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @parma bones Set of ant bone indices.
 */
void generate_ant_pupa_pose(skeleton& skeleton, const ant_bone_set& bones)
{
	const auto& rest_pose = skeleton.get_rest_pose();
	auto& pupa_pose = skeleton.add_pose("pupa");
	
	// Fold forelegs
	{
		constexpr float procoxa_fold_angle_y = math::radians(30.0f);
		constexpr float procoxa_fold_angle_z = math::radians(25.0f);
		constexpr float procoxa_fold_angle_x = math::radians(15.0f);
		auto fold_procoxa_l = math::transform<float>::identity();
		auto fold_procoxa_r = math::transform<float>::identity();
		fold_procoxa_l.rotation = math::angle_axis(procoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(procoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, float3{1, 0, 0});
		fold_procoxa_r.rotation = math::angle_axis(-procoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-procoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, float3{1, 0, 0});
		
		constexpr float profemur_fold_angle_z = math::radians(20.0f);
		constexpr float profemur_fold_angle_x = math::radians(80.0f);
		auto fold_profemur_l = math::transform<float>::identity();
		auto fold_profemur_r = math::transform<float>::identity();
		fold_profemur_l.rotation = math::angle_axis(profemur_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, float3{1, 0, 0});
		fold_profemur_r.rotation = math::angle_axis(-profemur_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, float3{1, 0, 0});
		
		constexpr float protibia_fold_angle = math::radians(165.0f);
		auto fold_protibia_l = math::transform<float>::identity();
		auto fold_protibia_r = math::transform<float>::identity();
		fold_protibia_l.rotation = math::angle_axis(-protibia_fold_angle, float3{1, 0, 0});
		fold_protibia_r.rotation = math::angle_axis(-protibia_fold_angle, float3{1, 0, 0});
		
		constexpr float protarsus_fold_angle = math::radians(20.0f);
		auto fold_protarsus_l = math::transform<float>::identity();
		auto fold_protarsus_r = math::transform<float>::identity();
		fold_protarsus_l.rotation = math::angle_axis(-protarsus_fold_angle, float3{1, 0, 0});
		fold_protarsus_r.rotation = math::angle_axis(-protarsus_fold_angle, float3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * fold_procoxa_l);
		pupa_pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * fold_procoxa_r);
		pupa_pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * fold_profemur_l);
		pupa_pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * fold_profemur_r);
		pupa_pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * fold_protibia_l);
		pupa_pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * fold_protibia_r);
		pupa_pose.set_relative_transform(bones.protarsus_l, rest_pose.get_relative_transform(bones.protarsus_l) * fold_protarsus_l);
		pupa_pose.set_relative_transform(bones.protarsus_r, rest_pose.get_relative_transform(bones.protarsus_r) * fold_protarsus_r);
	}
	
	// Fold midlegs
	{
		constexpr float mesocoxa_fold_angle_z = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_y = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_x = math::radians(10.0f);
		auto fold_mesocoxa_l = math::transform<float>::identity();
		auto fold_mesocoxa_r = math::transform<float>::identity();
		fold_mesocoxa_l.rotation = math::angle_axis(-mesocoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(mesocoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, float3{1, 0, 0});
		fold_mesocoxa_r.rotation = math::angle_axis(mesocoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(-mesocoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, float3{1, 0, 0});
		
		constexpr float mesofemur_fold_angle = math::radians(90.0f);
		auto fold_mesofemur_l = math::transform<float>::identity();
		auto fold_mesofemur_r = math::transform<float>::identity();
		fold_mesofemur_l.rotation = math::angle_axis(mesofemur_fold_angle, float3{1, 0, 0});
		fold_mesofemur_r.rotation = math::angle_axis(mesofemur_fold_angle, float3{1, 0, 0});
		
		constexpr float mesotibia_fold_angle = math::radians(162.0f);
		auto fold_mesotibia_l = math::transform<float>::identity();
		auto fold_mesotibia_r = math::transform<float>::identity();
		fold_mesotibia_l.rotation = math::angle_axis(-mesotibia_fold_angle, float3{1, 0, 0});
		fold_mesotibia_r.rotation = math::angle_axis(-mesotibia_fold_angle, float3{1, 0, 0});
		
		constexpr float mesotarsus_fold_angle = math::radians(20.0f);
		auto fold_mesotarsus_l = math::transform<float>::identity();
		auto fold_mesotarsus_r = math::transform<float>::identity();
		fold_mesotarsus_l.rotation = math::angle_axis(-mesotarsus_fold_angle, float3{1, 0, 0});
		fold_mesotarsus_r.rotation = math::angle_axis(-mesotarsus_fold_angle, float3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * fold_mesocoxa_l);
		pupa_pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * fold_mesocoxa_r);
		pupa_pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * fold_mesofemur_l);
		pupa_pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * fold_mesofemur_r);
		pupa_pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * fold_mesotibia_l);
		pupa_pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * fold_mesotibia_r);
		pupa_pose.set_relative_transform(bones.mesotarsus_l, rest_pose.get_relative_transform(bones.mesotarsus_l) * fold_mesotarsus_l);
		pupa_pose.set_relative_transform(bones.mesotarsus_r, rest_pose.get_relative_transform(bones.mesotarsus_r) * fold_mesotarsus_r);
	}
	
	// Fold hindlegs
	{
		constexpr float metacoxa_fold_angle_z = math::radians(15.0f);
		constexpr float metacoxa_fold_angle_y = math::radians(10.0f);
		constexpr float metacoxa_fold_angle_x = math::radians(25.0f);
		auto fold_metacoxa_l = math::transform<float>::identity();
		auto fold_metacoxa_r = math::transform<float>::identity();
		fold_metacoxa_l.rotation = math::angle_axis(-metacoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(-metacoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, float3{1, 0, 0});
		fold_metacoxa_r.rotation = math::angle_axis(metacoxa_fold_angle_z, float3{0, 0, 1}) * math::angle_axis(metacoxa_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, float3{1, 0, 0});
		
		constexpr float metafemur_fold_angle = math::radians(105.0f);
		auto fold_metafemur_l = math::transform<float>::identity();
		auto fold_metafemur_r = math::transform<float>::identity();
		fold_metafemur_l.rotation = math::angle_axis(metafemur_fold_angle, float3{1, 0, 0});
		fold_metafemur_r.rotation = math::angle_axis(metafemur_fold_angle, float3{1, 0, 0});
		
		constexpr float metatibia_fold_angle = math::radians(165.0f);
		auto fold_metatibia_l = math::transform<float>::identity();
		auto fold_metatibia_r = math::transform<float>::identity();
		fold_metatibia_l.rotation = math::angle_axis(-metatibia_fold_angle, float3{1, 0, 0});
		fold_metatibia_r.rotation = math::angle_axis(-metatibia_fold_angle, float3{1, 0, 0});
		
		constexpr float metatarsus_fold_angle = math::radians(0.0f);
		auto fold_metatarsus_l = math::transform<float>::identity();
		auto fold_metatarsus_r = math::transform<float>::identity();
		fold_metatarsus_l.rotation = math::angle_axis(-metatarsus_fold_angle, float3{1, 0, 0});
		fold_metatarsus_r.rotation = math::angle_axis(-metatarsus_fold_angle, float3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * fold_metacoxa_l);
		pupa_pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * fold_metacoxa_r);
		pupa_pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * fold_metafemur_l);
		pupa_pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * fold_metafemur_r);
		pupa_pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * fold_metatibia_l);
		pupa_pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * fold_metatibia_r);
		pupa_pose.set_relative_transform(bones.metatarsus_l, rest_pose.get_relative_transform(bones.metatarsus_l) * fold_metatarsus_l);
		pupa_pose.set_relative_transform(bones.metatarsus_r, rest_pose.get_relative_transform(bones.metatarsus_r) * fold_metatarsus_r);
	}
	
	// Fold head
	{
		constexpr float head_fold_angle = math::radians(80.0f);
		auto fold_head = math::transform<float>::identity();
		fold_head.rotation = math::angle_axis(-head_fold_angle, float3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.head, rest_pose.get_relative_transform(bones.head) * fold_head);
	}
	
	// Fold antennae
	{
		constexpr float antennomere1_fold_angle_y = math::radians(70.0f);
		constexpr float antennomere1_fold_angle_x = math::radians(45.0f);
		auto fold_antennomere1_l = math::transform<float>::identity();
		auto fold_antennomere1_r = math::transform<float>::identity();
		fold_antennomere1_l.rotation = math::angle_axis(-antennomere1_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, float3{1, 0, 0});
		fold_antennomere1_r.rotation = math::angle_axis(antennomere1_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, float3{1, 0, 0});
		
		constexpr float antennomere2_fold_angle_y = math::radians(75.0f);
		constexpr float antennomere2_fold_angle_x = math::radians(25.0f);
		auto fold_antennomere2_l = math::transform<float>::identity();
		auto fold_antennomere2_r = math::transform<float>::identity();
		fold_antennomere2_l.rotation = math::angle_axis(antennomere2_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, float3{1, 0, 0});
		fold_antennomere2_r.rotation = math::angle_axis(-antennomere2_fold_angle_y, float3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, float3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.antennomere1_l, rest_pose.get_relative_transform(bones.antennomere1_l) * fold_antennomere1_l);
		pupa_pose.set_relative_transform(bones.antennomere1_r, rest_pose.get_relative_transform(bones.antennomere1_r) * fold_antennomere1_r);
		pupa_pose.set_relative_transform(bones.antennomere2_l, rest_pose.get_relative_transform(bones.antennomere2_l) * fold_antennomere2_l);
		pupa_pose.set_relative_transform(bones.antennomere2_r, rest_pose.get_relative_transform(bones.antennomere2_r) * fold_antennomere2_r);
	}
	
	// Fold waist + gaster
	{
		constexpr float petiole_fold_angle = math::radians(40.0f);
		auto fold_petiole = math::transform<float>::identity();
		fold_petiole.rotation = math::angle_axis(petiole_fold_angle, float3{1, 0, 0});
		
		constexpr float postpetiole_fold_angle = math::radians(35.0f);
		auto fold_postpetiole = math::transform<float>::identity();
		fold_postpetiole.rotation = math::angle_axis(-postpetiole_fold_angle, float3{1, 0, 0});
		
		constexpr float gaster_fold_angle = math::radians(0.0f);
		auto fold_gaster = math::transform<float>::identity();
		fold_gaster.rotation = math::angle_axis(-gaster_fold_angle, float3{1, 0, 0});
		
		if (bones.petiole)
		{
			pupa_pose.set_relative_transform(*bones.petiole, rest_pose.get_relative_transform(*bones.petiole) * fold_petiole);
		}
		
		if (bones.postpetiole)
		{
			pupa_pose.set_relative_transform(*bones.postpetiole, rest_pose.get_relative_transform(*bones.postpetiole) * fold_postpetiole);
		}
		
		pupa_pose.set_relative_transform(bones.gaster, rest_pose.get_relative_transform(bones.gaster) * fold_gaster);
	}
	
	// Invert mesosoma
	// {
		// const float mesosoma_invert_angle = math::radians(90.0f);
		// auto invert_mesosoma = math::transform<float>::identity();
		// invert_mesosoma.rotation = math::angle_axis(mesosoma_invert_angle, float3{0, 0, 1});
		
		// auto mesosoma = *skeleton.get_index("mesosoma");
		// pupa_pose.set_relative_transform(bones.mesosoma, invert_mesosoma * rest_pose.get_relative_transform(bones.mesosoma));
	// }
	
	pupa_pose.update();
}

} // namespace

void generate_ant_skeleton(skeleton& skeleton, ant_bone_set& bones, const ant_phenome& phenome)
{
	// Assign bone indices
	bone_index_type bone_index = 0;
	bones.mesosoma = bone_index;
	bones.procoxa_l = ++bone_index;
	bones.profemur_l = ++bone_index;
	bones.protibia_l = ++bone_index;
	bones.protarsus_l = ++bone_index;
	bones.procoxa_r = ++bone_index;
	bones.profemur_r = ++bone_index;
	bones.protibia_r = ++bone_index;
	bones.protarsus_r = ++bone_index;
	bones.mesocoxa_l = ++bone_index;
	bones.mesofemur_l = ++bone_index;
	bones.mesotibia_l = ++bone_index;
	bones.mesotarsus_l = ++bone_index;
	bones.mesocoxa_r = ++bone_index;
	bones.mesofemur_r = ++bone_index;
	bones.mesotibia_r = ++bone_index;
	bones.mesotarsus_r = ++bone_index;
	bones.metacoxa_l = ++bone_index;
	bones.metafemur_l = ++bone_index;
	bones.metatibia_l = ++bone_index;
	bones.metatarsus_l = ++bone_index;
	bones.metacoxa_r = ++bone_index;
	bones.metafemur_r = ++bone_index;
	bones.metatibia_r = ++bone_index;
	bones.metatarsus_r = ++bone_index;
	bones.head = ++bone_index;
	bones.mandible_l = ++bone_index;
	bones.mandible_r = ++bone_index;
	bones.antennomere1_l = ++bone_index;
	bones.antennomere2_l = ++bone_index;
	bones.antennomere1_r = ++bone_index;
	bones.antennomere2_r = ++bone_index;
	
	if (phenome.waist->petiole_present)
	{
		bones.petiole = ++bone_index;
	}
	
	if (phenome.waist->postpetiole_present)
	{
		bones.postpetiole = ++bone_index;
	}
	
	bones.gaster = ++bone_index;
	
	if (phenome.sting->present)
	{
		bones.sting = ++bone_index;
	}
	
	if (phenome.wings->present)
	{
		bones.forewing_l = ++bone_index;
		bones.forewing_r = ++bone_index;
		bones.hindwing_l = ++bone_index;
		bones.hindwing_r = ++bone_index;
	}
	
	// Allocate bones
	skeleton.add_bones(bone_index + 1);
	
	// Assign bone parents
	skeleton.set_bone_parent(bones.mesosoma,       bones.mesosoma);
	skeleton.set_bone_parent(bones.procoxa_l,      bones.mesosoma);
	skeleton.set_bone_parent(bones.profemur_l,     bones.procoxa_l);
	skeleton.set_bone_parent(bones.protibia_l,     bones.profemur_l);
	skeleton.set_bone_parent(bones.protarsus_l,    bones.protibia_l);
	skeleton.set_bone_parent(bones.procoxa_r,      bones.mesosoma);
	skeleton.set_bone_parent(bones.profemur_r,     bones.procoxa_r);
	skeleton.set_bone_parent(bones.protibia_r,     bones.profemur_r);
	skeleton.set_bone_parent(bones.protarsus_r,    bones.protibia_r);
	skeleton.set_bone_parent(bones.mesocoxa_l,     bones.mesosoma);
	skeleton.set_bone_parent(bones.mesofemur_l,    bones.mesocoxa_l);
	skeleton.set_bone_parent(bones.mesotibia_l,    bones.mesofemur_l);
	skeleton.set_bone_parent(bones.mesotarsus_l,   bones.mesotibia_l);
	skeleton.set_bone_parent(bones.mesocoxa_r,     bones.mesosoma);
	skeleton.set_bone_parent(bones.mesofemur_r,    bones.mesocoxa_r);
	skeleton.set_bone_parent(bones.mesotibia_r,    bones.mesofemur_r);
	skeleton.set_bone_parent(bones.mesotarsus_r,   bones.mesotibia_r);
	skeleton.set_bone_parent(bones.metacoxa_l,     bones.mesosoma);
	skeleton.set_bone_parent(bones.metafemur_l,    bones.metacoxa_l);
	skeleton.set_bone_parent(bones.metatibia_l,    bones.metafemur_l);
	skeleton.set_bone_parent(bones.metatarsus_l,   bones.metatibia_l);
	skeleton.set_bone_parent(bones.metacoxa_r,     bones.mesosoma);
	skeleton.set_bone_parent(bones.metafemur_r,    bones.metacoxa_r);
	skeleton.set_bone_parent(bones.metatibia_r,    bones.metafemur_r);
	skeleton.set_bone_parent(bones.metatarsus_r,   bones.metatibia_r);
	skeleton.set_bone_parent(bones.head,           bones.mesosoma);
	skeleton.set_bone_parent(bones.mandible_l,     bones.head);
	skeleton.set_bone_parent(bones.mandible_r,     bones.head);
	skeleton.set_bone_parent(bones.antennomere1_l, bones.head);
	skeleton.set_bone_parent(bones.antennomere2_l, bones.antennomere1_l);
	skeleton.set_bone_parent(bones.antennomere1_r, bones.head);
	skeleton.set_bone_parent(bones.antennomere2_r, bones.antennomere1_r);
	
	if (bones.petiole)
	{
		skeleton.set_bone_parent(*bones.petiole, bones.mesosoma);
	}
	if (bones.postpetiole)
	{
		if (bones.petiole)
		{
			skeleton.set_bone_parent(*bones.postpetiole, *bones.petiole);
		}
		else
		{
			skeleton.set_bone_parent(*bones.postpetiole, bones.mesosoma);
		}
	}
	
	if (bones.postpetiole)
	{
		skeleton.set_bone_parent(bones.gaster, *bones.postpetiole);
	}
	else
	{
		if (bones.petiole)
		{
			skeleton.set_bone_parent(bones.gaster, *bones.petiole);
		}
		else
		{
			skeleton.set_bone_parent(bones.gaster, bones.mesosoma);
		}
	}
	
	if (bones.sting)
	{
		skeleton.set_bone_parent(*bones.sting, bones.gaster);
	}
	
	if (bones.forewing_l)
	{
		skeleton.set_bone_parent(*bones.forewing_l, bones.mesosoma);
		skeleton.set_bone_parent(*bones.forewing_r, bones.mesosoma);
		skeleton.set_bone_parent(*bones.hindwing_l, bones.mesosoma);
		skeleton.set_bone_parent(*bones.hindwing_r, bones.mesosoma);
	}
	
	// Assign bone names
	skeleton.set_bone_name(bones.mesosoma, "mesosoma");
	skeleton.set_bone_name(bones.procoxa_l, "procoxa_l");
	skeleton.set_bone_name(bones.profemur_l, "profemur_l");
	skeleton.set_bone_name(bones.protibia_l, "protibia_l");
	skeleton.set_bone_name(bones.protarsus_l, "protarsus_l");
	skeleton.set_bone_name(bones.procoxa_r, "procoxa_r");
	skeleton.set_bone_name(bones.profemur_r, "profemur_r");
	skeleton.set_bone_name(bones.protibia_r, "protibia_r");
	skeleton.set_bone_name(bones.protarsus_r, "protarsus_r");
	skeleton.set_bone_name(bones.mesocoxa_l, "mesocoxa_l");
	skeleton.set_bone_name(bones.mesofemur_l, "mesofemur_l");
	skeleton.set_bone_name(bones.mesotibia_l, "mesotibia_l");
	skeleton.set_bone_name(bones.mesotarsus_l, "mesotarsus_l");
	skeleton.set_bone_name(bones.mesocoxa_r, "mesocoxa_r");
	skeleton.set_bone_name(bones.mesofemur_r, "mesofemur_r");
	skeleton.set_bone_name(bones.mesotibia_r, "mesotibia_r");
	skeleton.set_bone_name(bones.mesotarsus_r, "mesotarsus_r");
	skeleton.set_bone_name(bones.metacoxa_l, "metacoxa_l");
	skeleton.set_bone_name(bones.metafemur_l, "metafemur_l");
	skeleton.set_bone_name(bones.metatibia_l, "metatibia_l");
	skeleton.set_bone_name(bones.metatarsus_l, "metatarsus_l");
	skeleton.set_bone_name(bones.metacoxa_r, "metacoxa_r");
	skeleton.set_bone_name(bones.metafemur_r, "metafemur_r");
	skeleton.set_bone_name(bones.metatibia_r, "metatibia_r");
	skeleton.set_bone_name(bones.metatarsus_r, "metatarsus_r");
	skeleton.set_bone_name(bones.head, "head");
	skeleton.set_bone_name(bones.mandible_l, "mandible_l");
	skeleton.set_bone_name(bones.mandible_r, "mandible_r");
	skeleton.set_bone_name(bones.antennomere1_l, "antennomere1_l");
	skeleton.set_bone_name(bones.antennomere2_l, "antennomere2_l");
	skeleton.set_bone_name(bones.antennomere1_r, "antennomere1_r");
	skeleton.set_bone_name(bones.antennomere2_r, "antennomere2_r");
	if (bones.petiole)
	{
		skeleton.set_bone_name(*bones.petiole, "petiole");
	}
	if (bones.postpetiole)
	{
		skeleton.set_bone_name(*bones.postpetiole, "postpetiole");
	}
	skeleton.set_bone_name(bones.gaster, "gaster");
	if (bones.sting)
	{
		skeleton.set_bone_name(*bones.sting, "sting");
	}
	if (bones.forewing_l)
	{
		skeleton.set_bone_name(*bones.forewing_l, "forewing_l");
		skeleton.set_bone_name(*bones.forewing_r, "forewing_r");
		skeleton.set_bone_name(*bones.hindwing_l, "hindwing_l");
		skeleton.set_bone_name(*bones.hindwing_r, "hindwing_r");
	}
	
	// Generate poses
	generate_ant_rest_pose(skeleton, bones, phenome);
	generate_ant_pupa_pose(skeleton, bones);
}