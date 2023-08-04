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
#include <engine/math/euler-angles.hpp>

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
	const ::skeleton* wings_skeleton = (phenome.wings->present) ? &phenome.wings->model->get_skeleton() : nullptr;
	
	auto get_bone_transform = [](const ::skeleton& skeleton, hash::fnv1a32_t bone_name)
	{
		return skeleton.get_rest_pose().get_relative_transform(*skeleton.get_bone_index(bone_name));
	};
	
	// Build skeleton rest pose
	skeleton.set_bone_transform(bones.mesosoma, get_bone_transform(mesosoma_skeleton, "mesosoma"));
	
	skeleton.set_bone_transform(bones.procoxa_l, get_bone_transform(mesosoma_skeleton, "procoxa_socket_l") * get_bone_transform(legs_skeleton, "procoxa_l"));
	skeleton.set_bone_transform(bones.profemur_l, get_bone_transform(legs_skeleton, "profemur_l"));
	skeleton.set_bone_transform(bones.protibia_l, get_bone_transform(legs_skeleton, "protibia_l"));
	skeleton.set_bone_transform(bones.protarsomere1_l, get_bone_transform(legs_skeleton, "protarsomere1_l"));
	
	skeleton.set_bone_transform(bones.procoxa_r, get_bone_transform(mesosoma_skeleton, "procoxa_socket_r") * get_bone_transform(legs_skeleton, "procoxa_r"));
	skeleton.set_bone_transform(bones.profemur_r, get_bone_transform(legs_skeleton, "profemur_r"));
	skeleton.set_bone_transform(bones.protibia_r, get_bone_transform(legs_skeleton, "protibia_r"));
	skeleton.set_bone_transform(bones.protarsomere1_r, get_bone_transform(legs_skeleton, "protarsomere1_r"));
	
	skeleton.set_bone_transform(bones.mesocoxa_l, get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_l") * get_bone_transform(legs_skeleton, "mesocoxa_l"));
	skeleton.set_bone_transform(bones.mesofemur_l, get_bone_transform(legs_skeleton, "mesofemur_l"));
	skeleton.set_bone_transform(bones.mesotibia_l, get_bone_transform(legs_skeleton, "mesotibia_l"));
	skeleton.set_bone_transform(bones.mesotarsomere1_l, get_bone_transform(legs_skeleton, "mesotarsomere1_l"));
	
	skeleton.set_bone_transform(bones.mesocoxa_r, get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_r") * get_bone_transform(legs_skeleton, "mesocoxa_r"));
	skeleton.set_bone_transform(bones.mesofemur_r, get_bone_transform(legs_skeleton, "mesofemur_r"));
	skeleton.set_bone_transform(bones.mesotibia_r, get_bone_transform(legs_skeleton, "mesotibia_r"));
	skeleton.set_bone_transform(bones.mesotarsomere1_r, get_bone_transform(legs_skeleton, "mesotarsomere1_r"));
	
	skeleton.set_bone_transform(bones.metacoxa_l, get_bone_transform(mesosoma_skeleton, "metacoxa_socket_l") * get_bone_transform(legs_skeleton, "metacoxa_l"));
	skeleton.set_bone_transform(bones.metafemur_l, get_bone_transform(legs_skeleton, "metafemur_l"));
	skeleton.set_bone_transform(bones.metatibia_l, get_bone_transform(legs_skeleton, "metatibia_l"));
	skeleton.set_bone_transform(bones.metatarsomere1_l, get_bone_transform(legs_skeleton, "metatarsomere1_l"));
	
	skeleton.set_bone_transform(bones.metacoxa_r, get_bone_transform(mesosoma_skeleton, "metacoxa_socket_r") * get_bone_transform(legs_skeleton, "metacoxa_r"));
	skeleton.set_bone_transform(bones.metafemur_r, get_bone_transform(legs_skeleton, "metafemur_r"));
	skeleton.set_bone_transform(bones.metatibia_r, get_bone_transform(legs_skeleton, "metatibia_r"));
	skeleton.set_bone_transform(bones.metatarsomere1_r, get_bone_transform(legs_skeleton, "metatarsomere1_r"));
	
	skeleton.set_bone_transform(bones.head, get_bone_transform(mesosoma_skeleton, "head_socket") * get_bone_transform(head_skeleton, "head"));
	skeleton.set_bone_transform(bones.mandible_l, get_bone_transform(head_skeleton, "mandible_socket_l") * get_bone_transform(mandibles_skeleton, "mandible_l"));
	skeleton.set_bone_transform(bones.mandible_r, get_bone_transform(head_skeleton, "mandible_socket_r") * get_bone_transform(mandibles_skeleton, "mandible_r"));
	skeleton.set_bone_transform(bones.antennomere1_l, get_bone_transform(head_skeleton, "antenna_socket_l") * get_bone_transform(antennae_skeleton, "antennomere1_l"));
	skeleton.set_bone_transform(bones.antennomere2_l, get_bone_transform(antennae_skeleton, "antennomere2_l"));
	skeleton.set_bone_transform(bones.antennomere1_r, get_bone_transform(head_skeleton, "antenna_socket_r") * get_bone_transform(antennae_skeleton, "antennomere1_r"));
	skeleton.set_bone_transform(bones.antennomere2_r, get_bone_transform(antennae_skeleton, "antennomere2_r"));
	
	if (phenome.waist->present)
	{
		skeleton.set_bone_transform(*bones.petiole, get_bone_transform(mesosoma_skeleton, "petiole_socket") * get_bone_transform(waist_skeleton, "petiole"));
		
		if (phenome.waist->postpetiole_present)
		{
			skeleton.set_bone_transform(*bones.postpetiole, get_bone_transform(waist_skeleton, "postpetiole"));
		}
		
		skeleton.set_bone_transform(bones.gaster, get_bone_transform(waist_skeleton, "gaster_socket") * get_bone_transform(gaster_skeleton, "gaster"));
	}
	else
	{
		skeleton.set_bone_transform(bones.gaster, get_bone_transform(mesosoma_skeleton, "petiole_socket") * get_bone_transform(gaster_skeleton, "gaster"));
	}
	
	if (phenome.sting->present)
	{
		skeleton.set_bone_transform(*bones.sting, get_bone_transform(gaster_skeleton, "sting_socket") * get_bone_transform(*sting_skeleton, "sting"));
	}
	
	if (phenome.wings->present)
	{
		skeleton.set_bone_transform(*bones.forewing_l, get_bone_transform(mesosoma_skeleton, "forewing_socket_l") * get_bone_transform(*wings_skeleton, "forewing_l"));
		skeleton.set_bone_transform(*bones.forewing_r, get_bone_transform(mesosoma_skeleton, "forewing_socket_r") * get_bone_transform(*wings_skeleton, "forewing_r"));
		skeleton.set_bone_transform(*bones.hindwing_l, get_bone_transform(mesosoma_skeleton, "hindwing_socket_l") * get_bone_transform(*wings_skeleton, "hindwing_l"));
		skeleton.set_bone_transform(*bones.hindwing_r, get_bone_transform(mesosoma_skeleton, "hindwing_socket_r") * get_bone_transform(*wings_skeleton, "hindwing_r"));
	}
	
	skeleton.update_rest_pose();
}

/**
 * Generates a standing pose (named "midstance") for an ant skeleton.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @parma bones Set of ant bone indices.
 */
void generate_ant_midstance_pose(skeleton& skeleton, const ant_bone_set& bones)
{
	auto& pose = skeleton.add_pose("midstance");
	const auto& rest_pose = skeleton.get_rest_pose();
	
	// Pose forelegs
	{
		const auto procoxa_l_angles = math::fvec3{0.0f, 40.0f, 0.0f} * math::deg2rad<float>;
		const auto procoxa_r_angles = procoxa_l_angles * math::fvec3{1, -1, 1};
		const auto profemur_l_angles = math::fvec3{31.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto profemur_r_angles = profemur_l_angles * math::fvec3{1, 1, 1};
		const auto protibia_l_angles = math::fvec3{-90.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protibia_r_angles = protibia_l_angles * math::fvec3{1, 1, 1};
		const auto protarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protarsomere1_r_angles = protarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto procoxa_l_xf = math::transform<float>::identity();
		auto procoxa_r_xf = math::transform<float>::identity();
		auto profemur_l_xf = math::transform<float>::identity();
		auto profemur_r_xf = math::transform<float>::identity();
		auto protibia_l_xf = math::transform<float>::identity();
		auto protibia_r_xf = math::transform<float>::identity();
		auto protarsomere1_l_xf = math::transform<float>::identity();
		auto protarsomere1_r_xf = math::transform<float>::identity();
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * procoxa_l_xf);
		pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * procoxa_r_xf);
		pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * profemur_l_xf);
		pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * profemur_r_xf);
		pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * protibia_l_xf);
		pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * protibia_r_xf);
		pose.set_relative_transform(bones.protarsomere1_l, rest_pose.get_relative_transform(bones.protarsomere1_l) * protarsomere1_l_xf);
		pose.set_relative_transform(bones.protarsomere1_r, rest_pose.get_relative_transform(bones.protarsomere1_r) * protarsomere1_r_xf);
	}
	
	// Pose midlegs
	{
		const auto mesocoxa_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesocoxa_r_angles = mesocoxa_l_angles * math::fvec3{1, 1, 1};
		const auto mesofemur_l_angles = math::fvec3{38.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesofemur_r_angles = mesofemur_l_angles * math::fvec3{1, 1, 1};
		const auto mesotibia_l_angles = math::fvec3{-100.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotibia_r_angles = mesotibia_l_angles * math::fvec3{1, 1, 1};
		const auto mesotarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotarsomere1_r_angles = mesotarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto mesocoxa_l_xf = math::transform<float>::identity();
		auto mesocoxa_r_xf = math::transform<float>::identity();
		auto mesofemur_l_xf = math::transform<float>::identity();
		auto mesofemur_r_xf = math::transform<float>::identity();
		auto mesotibia_l_xf = math::transform<float>::identity();
		auto mesotibia_r_xf = math::transform<float>::identity();
		auto mesotarsomere1_l_xf = math::transform<float>::identity();
		auto mesotarsomere1_r_xf = math::transform<float>::identity();
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * mesocoxa_l_xf);
		pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * mesocoxa_r_xf);
		pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * mesofemur_l_xf);
		pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * mesofemur_r_xf);
		pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * mesotibia_l_xf);
		pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * mesotibia_r_xf);
		pose.set_relative_transform(bones.mesotarsomere1_l, rest_pose.get_relative_transform(bones.mesotarsomere1_l) * mesotarsomere1_l_xf);
		pose.set_relative_transform(bones.mesotarsomere1_r, rest_pose.get_relative_transform(bones.mesotarsomere1_r) * mesotarsomere1_r_xf);
	}
	
	// Pose hindlegs
	{
		const auto metacoxa_l_angles = math::fvec3{0.0f, -34.0f, 0.0f} * math::deg2rad<float>;
		const auto metacoxa_r_angles = metacoxa_l_angles * math::fvec3{1, -1, 1};
		const auto metafemur_l_angles = math::fvec3{48.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metafemur_r_angles = metafemur_l_angles * math::fvec3{1, 1, 1};
		const auto metatibia_l_angles = math::fvec3{-100.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatibia_r_angles = metatibia_l_angles * math::fvec3{1, 1, 1};
		const auto metatarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatarsomere1_r_angles = metatarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto metacoxa_l_xf = math::transform<float>::identity();
		auto metacoxa_r_xf = math::transform<float>::identity();
		auto metafemur_l_xf = math::transform<float>::identity();
		auto metafemur_r_xf = math::transform<float>::identity();
		auto metatibia_l_xf = math::transform<float>::identity();
		auto metatibia_r_xf = math::transform<float>::identity();
		auto metatarsomere1_l_xf = math::transform<float>::identity();
		auto metatarsomere1_r_xf = math::transform<float>::identity();
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * metacoxa_l_xf);
		pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * metacoxa_r_xf);
		pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * metafemur_l_xf);
		pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * metafemur_r_xf);
		pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * metatibia_l_xf);
		pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * metatibia_r_xf);
		pose.set_relative_transform(bones.metatarsomere1_l, rest_pose.get_relative_transform(bones.metatarsomere1_l) * metatarsomere1_l_xf);
		pose.set_relative_transform(bones.metatarsomere1_r, rest_pose.get_relative_transform(bones.metatarsomere1_r) * metatarsomere1_r_xf);
	}
	
	pose.update();
}

/**
 * Generates a lift-off pose (named "liftoff") for an ant skeleton.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @parma bones Set of ant bone indices.
 */
void generate_ant_liftoff_pose(skeleton& skeleton, const ant_bone_set& bones)
{
	auto& pose = skeleton.add_pose("liftoff");
	const auto& rest_pose = skeleton.get_rest_pose();
	
	// Pose forelegs
	{
		const auto procoxa_l_angles = math::fvec3{0.0f, 50.0f, 0.0f} * math::deg2rad<float>;
		const auto procoxa_r_angles = procoxa_l_angles * math::fvec3{1, -1, 1};
		const auto profemur_l_angles = math::fvec3{34.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto profemur_r_angles = profemur_l_angles * math::fvec3{1, 1, 1};
		const auto protibia_l_angles = math::fvec3{-118.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protibia_r_angles = protibia_l_angles * math::fvec3{1, 1, 1};
		const auto protarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protarsomere1_r_angles = protarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto procoxa_l_xf = math::transform<float>::identity();
		auto procoxa_r_xf = math::transform<float>::identity();
		auto profemur_l_xf = math::transform<float>::identity();
		auto profemur_r_xf = math::transform<float>::identity();
		auto protibia_l_xf = math::transform<float>::identity();
		auto protibia_r_xf = math::transform<float>::identity();
		auto protarsomere1_l_xf = math::transform<float>::identity();
		auto protarsomere1_r_xf = math::transform<float>::identity();
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * procoxa_l_xf);
		pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * procoxa_r_xf);
		pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * profemur_l_xf);
		pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * profemur_r_xf);
		pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * protibia_l_xf);
		pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * protibia_r_xf);
		pose.set_relative_transform(bones.protarsomere1_l, rest_pose.get_relative_transform(bones.protarsomere1_l) * protarsomere1_l_xf);
		pose.set_relative_transform(bones.protarsomere1_r, rest_pose.get_relative_transform(bones.protarsomere1_r) * protarsomere1_r_xf);
	}
	
	// Pose midlegs
	{
		const auto mesocoxa_l_angles = math::fvec3{0.0f, 30.0f, 0.0f} * math::deg2rad<float>;
		const auto mesocoxa_r_angles = mesocoxa_l_angles * math::fvec3{1, -1, 1};
		const auto mesofemur_l_angles = math::fvec3{36.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesofemur_r_angles = mesofemur_l_angles * math::fvec3{1, 1, 1};
		const auto mesotibia_l_angles = math::fvec3{-110.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotibia_r_angles = mesotibia_l_angles * math::fvec3{1, 1, 1};
		const auto mesotarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotarsomere1_r_angles = mesotarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto mesocoxa_l_xf = math::transform<float>::identity();
		auto mesocoxa_r_xf = math::transform<float>::identity();
		auto mesofemur_l_xf = math::transform<float>::identity();
		auto mesofemur_r_xf = math::transform<float>::identity();
		auto mesotibia_l_xf = math::transform<float>::identity();
		auto mesotibia_r_xf = math::transform<float>::identity();
		auto mesotarsomere1_l_xf = math::transform<float>::identity();
		auto mesotarsomere1_r_xf = math::transform<float>::identity();
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * mesocoxa_l_xf);
		pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * mesocoxa_r_xf);
		pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * mesofemur_l_xf);
		pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * mesofemur_r_xf);
		pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * mesotibia_l_xf);
		pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * mesotibia_r_xf);
		pose.set_relative_transform(bones.mesotarsomere1_l, rest_pose.get_relative_transform(bones.mesotarsomere1_l) * mesotarsomere1_l_xf);
		pose.set_relative_transform(bones.mesotarsomere1_r, rest_pose.get_relative_transform(bones.mesotarsomere1_r) * mesotarsomere1_r_xf);
	}
	
	// Pose hindlegs
	{
		const auto metacoxa_l_angles = math::fvec3{0.0f, -27.5f, 0.0f} * math::deg2rad<float>;
		const auto metacoxa_r_angles = metacoxa_l_angles * math::fvec3{1, -1, 1};
		const auto metafemur_l_angles = math::fvec3{6.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metafemur_r_angles = metafemur_l_angles * math::fvec3{1, 1, 1};
		const auto metatibia_l_angles = math::fvec3{-39.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatibia_r_angles = metatibia_l_angles * math::fvec3{1, 1, 1};
		const auto metatarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatarsomere1_r_angles = metatarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto metacoxa_l_xf = math::transform<float>::identity();
		auto metacoxa_r_xf = math::transform<float>::identity();
		auto metafemur_l_xf = math::transform<float>::identity();
		auto metafemur_r_xf = math::transform<float>::identity();
		auto metatibia_l_xf = math::transform<float>::identity();
		auto metatibia_r_xf = math::transform<float>::identity();
		auto metatarsomere1_l_xf = math::transform<float>::identity();
		auto metatarsomere1_r_xf = math::transform<float>::identity();
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * metacoxa_l_xf);
		pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * metacoxa_r_xf);
		pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * metafemur_l_xf);
		pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * metafemur_r_xf);
		pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * metatibia_l_xf);
		pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * metatibia_r_xf);
		pose.set_relative_transform(bones.metatarsomere1_l, rest_pose.get_relative_transform(bones.metatarsomere1_l) * metatarsomere1_l_xf);
		pose.set_relative_transform(bones.metatarsomere1_r, rest_pose.get_relative_transform(bones.metatarsomere1_r) * metatarsomere1_r_xf);
	}
	
	pose.update();
}

/**
 * Generates a touchdown pose (named "touchdown") for an ant skeleton.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @parma bones Set of ant bone indices.
 */
void generate_ant_touchdown_pose(skeleton& skeleton, const ant_bone_set& bones)
{
	auto& pose = skeleton.add_pose("touchdown");
	const auto& rest_pose = skeleton.get_rest_pose();
	
	// Pose forelegs
	{
		const auto procoxa_l_angles = math::fvec3{0.0f, 25.0f, 0.0f} * math::deg2rad<float>;
		const auto procoxa_r_angles = procoxa_l_angles * math::fvec3{1, -1, 1};
		const auto profemur_l_angles = math::fvec3{10.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto profemur_r_angles = profemur_l_angles * math::fvec3{1, 1, 1};
		const auto protibia_l_angles = math::fvec3{-60.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protibia_r_angles = protibia_l_angles * math::fvec3{1, 1, 1};
		const auto protarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protarsomere1_r_angles = protarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto procoxa_l_xf = math::transform<float>::identity();
		auto procoxa_r_xf = math::transform<float>::identity();
		auto profemur_l_xf = math::transform<float>::identity();
		auto profemur_r_xf = math::transform<float>::identity();
		auto protibia_l_xf = math::transform<float>::identity();
		auto protibia_r_xf = math::transform<float>::identity();
		auto protarsomere1_l_xf = math::transform<float>::identity();
		auto protarsomere1_r_xf = math::transform<float>::identity();
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * procoxa_l_xf);
		pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * procoxa_r_xf);
		pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * profemur_l_xf);
		pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * profemur_r_xf);
		pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * protibia_l_xf);
		pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * protibia_r_xf);
		pose.set_relative_transform(bones.protarsomere1_l, rest_pose.get_relative_transform(bones.protarsomere1_l) * protarsomere1_l_xf);
		pose.set_relative_transform(bones.protarsomere1_r, rest_pose.get_relative_transform(bones.protarsomere1_r) * protarsomere1_r_xf);
	}
	
	// Pose midlegs
	{
		const auto mesocoxa_l_angles = math::fvec3{0.0f, -22.0f, 0.0f} * math::deg2rad<float>;
		const auto mesocoxa_r_angles = mesocoxa_l_angles * math::fvec3{1, -1, 1};
		const auto mesofemur_l_angles = math::fvec3{21.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesofemur_r_angles = mesofemur_l_angles * math::fvec3{1, 1, 1};
		const auto mesotibia_l_angles = math::fvec3{-80.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotibia_r_angles = mesotibia_l_angles * math::fvec3{1, 1, 1};
		const auto mesotarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotarsomere1_r_angles = mesotarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto mesocoxa_l_xf = math::transform<float>::identity();
		auto mesocoxa_r_xf = math::transform<float>::identity();
		auto mesofemur_l_xf = math::transform<float>::identity();
		auto mesofemur_r_xf = math::transform<float>::identity();
		auto mesotibia_l_xf = math::transform<float>::identity();
		auto mesotibia_r_xf = math::transform<float>::identity();
		auto mesotarsomere1_l_xf = math::transform<float>::identity();
		auto mesotarsomere1_r_xf = math::transform<float>::identity();
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * mesocoxa_l_xf);
		pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * mesocoxa_r_xf);
		pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * mesofemur_l_xf);
		pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * mesofemur_r_xf);
		pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * mesotibia_l_xf);
		pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * mesotibia_r_xf);
		pose.set_relative_transform(bones.mesotarsomere1_l, rest_pose.get_relative_transform(bones.mesotarsomere1_l) * mesotarsomere1_l_xf);
		pose.set_relative_transform(bones.mesotarsomere1_r, rest_pose.get_relative_transform(bones.mesotarsomere1_r) * mesotarsomere1_r_xf);
	}
	
	// Pose hindlegs
	{
		const auto metacoxa_l_angles = math::fvec3{0.0f, -42.0f, 0.0f} * math::deg2rad<float>;
		const auto metacoxa_r_angles = metacoxa_l_angles * math::fvec3{1, -1, 1};
		const auto metafemur_l_angles = math::fvec3{60.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metafemur_r_angles = metafemur_l_angles * math::fvec3{1, 1, 1};
		const auto metatibia_l_angles = math::fvec3{-125.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatibia_r_angles = metatibia_l_angles * math::fvec3{1, 1, 1};
		const auto metatarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatarsomere1_r_angles = metatarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto metacoxa_l_xf = math::transform<float>::identity();
		auto metacoxa_r_xf = math::transform<float>::identity();
		auto metafemur_l_xf = math::transform<float>::identity();
		auto metafemur_r_xf = math::transform<float>::identity();
		auto metatibia_l_xf = math::transform<float>::identity();
		auto metatibia_r_xf = math::transform<float>::identity();
		auto metatarsomere1_l_xf = math::transform<float>::identity();
		auto metatarsomere1_r_xf = math::transform<float>::identity();
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * metacoxa_l_xf);
		pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * metacoxa_r_xf);
		pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * metafemur_l_xf);
		pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * metafemur_r_xf);
		pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * metatibia_l_xf);
		pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * metatibia_r_xf);
		pose.set_relative_transform(bones.metatarsomere1_l, rest_pose.get_relative_transform(bones.metatarsomere1_l) * metatarsomere1_l_xf);
		pose.set_relative_transform(bones.metatarsomere1_r, rest_pose.get_relative_transform(bones.metatarsomere1_r) * metatarsomere1_r_xf);
	}
	
	pose.update();
}

/**
 * Generates a pose in which each leg is lifted to its step height (named "midswing") for an ant skeleton.
 *
 * @param[in,out] skeleton Ant skeleton.
 * @parma bones Set of ant bone indices.
 */
void generate_ant_midswing_pose(skeleton& skeleton, const ant_bone_set& bones)
{
	auto& pose = skeleton.add_pose("midswing");
	const auto& rest_pose = skeleton.get_rest_pose();
	
	// Pose forelegs
	{
		const auto procoxa_l_angles = math::fvec3{0.0f, 37.5f, 0.0f} * math::deg2rad<float>;
		const auto procoxa_r_angles = procoxa_l_angles * math::fvec3{1, -1, 1};
		const auto profemur_l_angles = math::fvec3{60.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto profemur_r_angles = profemur_l_angles * math::fvec3{1, 1, 1};
		const auto protibia_l_angles = math::fvec3{-100.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protibia_r_angles = protibia_l_angles * math::fvec3{1, 1, 1};
		const auto protarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto protarsomere1_r_angles = protarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto procoxa_l_xf = math::transform<float>::identity();
		auto procoxa_r_xf = math::transform<float>::identity();
		auto profemur_l_xf = math::transform<float>::identity();
		auto profemur_r_xf = math::transform<float>::identity();
		auto protibia_l_xf = math::transform<float>::identity();
		auto protibia_r_xf = math::transform<float>::identity();
		auto protarsomere1_l_xf = math::transform<float>::identity();
		auto protarsomere1_r_xf = math::transform<float>::identity();
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * procoxa_l_xf);
		pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * procoxa_r_xf);
		pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * profemur_l_xf);
		pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * profemur_r_xf);
		pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * protibia_l_xf);
		pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * protibia_r_xf);
		pose.set_relative_transform(bones.protarsomere1_l, rest_pose.get_relative_transform(bones.protarsomere1_l) * protarsomere1_l_xf);
		pose.set_relative_transform(bones.protarsomere1_r, rest_pose.get_relative_transform(bones.protarsomere1_r) * protarsomere1_r_xf);
	}
	
	// Pose midlegs
	{
		const auto mesocoxa_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesocoxa_r_angles = mesocoxa_l_angles * math::fvec3{1, 1, 1};
		const auto mesofemur_l_angles = math::fvec3{60.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesofemur_r_angles = mesofemur_l_angles * math::fvec3{1, 1, 1};
		const auto mesotibia_l_angles = math::fvec3{-100.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotibia_r_angles = mesotibia_l_angles * math::fvec3{1, 1, 1};
		const auto mesotarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto mesotarsomere1_r_angles = mesotarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto mesocoxa_l_xf = math::transform<float>::identity();
		auto mesocoxa_r_xf = math::transform<float>::identity();
		auto mesofemur_l_xf = math::transform<float>::identity();
		auto mesofemur_r_xf = math::transform<float>::identity();
		auto mesotibia_l_xf = math::transform<float>::identity();
		auto mesotibia_r_xf = math::transform<float>::identity();
		auto mesotarsomere1_l_xf = math::transform<float>::identity();
		auto mesotarsomere1_r_xf = math::transform<float>::identity();
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * mesocoxa_l_xf);
		pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * mesocoxa_r_xf);
		pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * mesofemur_l_xf);
		pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * mesofemur_r_xf);
		pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * mesotibia_l_xf);
		pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * mesotibia_r_xf);
		pose.set_relative_transform(bones.mesotarsomere1_l, rest_pose.get_relative_transform(bones.mesotarsomere1_l) * mesotarsomere1_l_xf);
		pose.set_relative_transform(bones.mesotarsomere1_r, rest_pose.get_relative_transform(bones.mesotarsomere1_r) * mesotarsomere1_r_xf);
	}
	
	// Pose hindlegs
	{
		const auto metacoxa_l_angles = math::fvec3{0.0f, -37.5f, 0.0f} * math::deg2rad<float>;
		const auto metacoxa_r_angles = metacoxa_l_angles * math::fvec3{1, -1, 1};
		const auto metafemur_l_angles = math::fvec3{60.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metafemur_r_angles = metafemur_l_angles * math::fvec3{1, 1, 1};
		const auto metatibia_l_angles = math::fvec3{-100.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatibia_r_angles = metatibia_l_angles * math::fvec3{1, 1, 1};
		const auto metatarsomere1_l_angles = math::fvec3{0.0f, 0.0f, 0.0f} * math::deg2rad<float>;
		const auto metatarsomere1_r_angles = metatarsomere1_l_angles * math::fvec3{1, 1, 1};
		
		auto metacoxa_l_xf = math::transform<float>::identity();
		auto metacoxa_r_xf = math::transform<float>::identity();
		auto metafemur_l_xf = math::transform<float>::identity();
		auto metafemur_r_xf = math::transform<float>::identity();
		auto metatibia_l_xf = math::transform<float>::identity();
		auto metatibia_r_xf = math::transform<float>::identity();
		auto metatarsomere1_l_xf = math::transform<float>::identity();
		auto metatarsomere1_r_xf = math::transform<float>::identity();
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * metacoxa_l_xf);
		pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * metacoxa_r_xf);
		pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * metafemur_l_xf);
		pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * metafemur_r_xf);
		pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * metatibia_l_xf);
		pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * metatibia_r_xf);
		pose.set_relative_transform(bones.metatarsomere1_l, rest_pose.get_relative_transform(bones.metatarsomere1_l) * metatarsomere1_l_xf);
		pose.set_relative_transform(bones.metatarsomere1_r, rest_pose.get_relative_transform(bones.metatarsomere1_r) * metatarsomere1_r_xf);
	}
	
	pose.update();
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
		fold_procoxa_l.rotation = math::angle_axis(procoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(procoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_procoxa_r.rotation = math::angle_axis(-procoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-procoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float profemur_fold_angle_z = math::radians(20.0f);
		constexpr float profemur_fold_angle_x = math::radians(80.0f);
		auto fold_profemur_l = math::transform<float>::identity();
		auto fold_profemur_r = math::transform<float>::identity();
		fold_profemur_l.rotation = math::angle_axis(profemur_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, math::fvec3{1, 0, 0});
		fold_profemur_r.rotation = math::angle_axis(-profemur_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float protibia_fold_angle = math::radians(165.0f);
		auto fold_protibia_l = math::transform<float>::identity();
		auto fold_protibia_r = math::transform<float>::identity();
		fold_protibia_l.rotation = math::angle_axis(-protibia_fold_angle, math::fvec3{1, 0, 0});
		fold_protibia_r.rotation = math::angle_axis(-protibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float protarsomere1_fold_angle = math::radians(20.0f);
		auto fold_protarsomere1_l = math::transform<float>::identity();
		auto fold_protarsomere1_r = math::transform<float>::identity();
		fold_protarsomere1_l.rotation = math::angle_axis(-protarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_protarsomere1_r.rotation = math::angle_axis(-protarsomere1_fold_angle, math::fvec3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.procoxa_l, rest_pose.get_relative_transform(bones.procoxa_l) * fold_procoxa_l);
		pupa_pose.set_relative_transform(bones.procoxa_r, rest_pose.get_relative_transform(bones.procoxa_r) * fold_procoxa_r);
		pupa_pose.set_relative_transform(bones.profemur_l, rest_pose.get_relative_transform(bones.profemur_l) * fold_profemur_l);
		pupa_pose.set_relative_transform(bones.profemur_r, rest_pose.get_relative_transform(bones.profemur_r) * fold_profemur_r);
		pupa_pose.set_relative_transform(bones.protibia_l, rest_pose.get_relative_transform(bones.protibia_l) * fold_protibia_l);
		pupa_pose.set_relative_transform(bones.protibia_r, rest_pose.get_relative_transform(bones.protibia_r) * fold_protibia_r);
		pupa_pose.set_relative_transform(bones.protarsomere1_l, rest_pose.get_relative_transform(bones.protarsomere1_l) * fold_protarsomere1_l);
		pupa_pose.set_relative_transform(bones.protarsomere1_r, rest_pose.get_relative_transform(bones.protarsomere1_r) * fold_protarsomere1_r);
	}
	
	// Fold midlegs
	{
		constexpr float mesocoxa_fold_angle_z = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_y = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_x = math::radians(10.0f);
		auto fold_mesocoxa_l = math::transform<float>::identity();
		auto fold_mesocoxa_r = math::transform<float>::identity();
		fold_mesocoxa_l.rotation = math::angle_axis(-mesocoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(mesocoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_mesocoxa_r.rotation = math::angle_axis(mesocoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(-mesocoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float mesofemur_fold_angle = math::radians(90.0f);
		auto fold_mesofemur_l = math::transform<float>::identity();
		auto fold_mesofemur_r = math::transform<float>::identity();
		fold_mesofemur_l.rotation = math::angle_axis(mesofemur_fold_angle, math::fvec3{1, 0, 0});
		fold_mesofemur_r.rotation = math::angle_axis(mesofemur_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float mesotibia_fold_angle = math::radians(162.0f);
		auto fold_mesotibia_l = math::transform<float>::identity();
		auto fold_mesotibia_r = math::transform<float>::identity();
		fold_mesotibia_l.rotation = math::angle_axis(-mesotibia_fold_angle, math::fvec3{1, 0, 0});
		fold_mesotibia_r.rotation = math::angle_axis(-mesotibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float mesotarsomere1_fold_angle = math::radians(20.0f);
		auto fold_mesotarsomere1_l = math::transform<float>::identity();
		auto fold_mesotarsomere1_r = math::transform<float>::identity();
		fold_mesotarsomere1_l.rotation = math::angle_axis(-mesotarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_mesotarsomere1_r.rotation = math::angle_axis(-mesotarsomere1_fold_angle, math::fvec3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.mesocoxa_l, rest_pose.get_relative_transform(bones.mesocoxa_l) * fold_mesocoxa_l);
		pupa_pose.set_relative_transform(bones.mesocoxa_r, rest_pose.get_relative_transform(bones.mesocoxa_r) * fold_mesocoxa_r);
		pupa_pose.set_relative_transform(bones.mesofemur_l, rest_pose.get_relative_transform(bones.mesofemur_l) * fold_mesofemur_l);
		pupa_pose.set_relative_transform(bones.mesofemur_r, rest_pose.get_relative_transform(bones.mesofemur_r) * fold_mesofemur_r);
		pupa_pose.set_relative_transform(bones.mesotibia_l, rest_pose.get_relative_transform(bones.mesotibia_l) * fold_mesotibia_l);
		pupa_pose.set_relative_transform(bones.mesotibia_r, rest_pose.get_relative_transform(bones.mesotibia_r) * fold_mesotibia_r);
		pupa_pose.set_relative_transform(bones.mesotarsomere1_l, rest_pose.get_relative_transform(bones.mesotarsomere1_l) * fold_mesotarsomere1_l);
		pupa_pose.set_relative_transform(bones.mesotarsomere1_r, rest_pose.get_relative_transform(bones.mesotarsomere1_r) * fold_mesotarsomere1_r);
	}
	
	// Fold hindlegs
	{
		constexpr float metacoxa_fold_angle_z = math::radians(15.0f);
		constexpr float metacoxa_fold_angle_y = math::radians(10.0f);
		constexpr float metacoxa_fold_angle_x = math::radians(25.0f);
		auto fold_metacoxa_l = math::transform<float>::identity();
		auto fold_metacoxa_r = math::transform<float>::identity();
		fold_metacoxa_l.rotation = math::angle_axis(-metacoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(-metacoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_metacoxa_r.rotation = math::angle_axis(metacoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(metacoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float metafemur_fold_angle = math::radians(105.0f);
		auto fold_metafemur_l = math::transform<float>::identity();
		auto fold_metafemur_r = math::transform<float>::identity();
		fold_metafemur_l.rotation = math::angle_axis(metafemur_fold_angle, math::fvec3{1, 0, 0});
		fold_metafemur_r.rotation = math::angle_axis(metafemur_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float metatibia_fold_angle = math::radians(165.0f);
		auto fold_metatibia_l = math::transform<float>::identity();
		auto fold_metatibia_r = math::transform<float>::identity();
		fold_metatibia_l.rotation = math::angle_axis(-metatibia_fold_angle, math::fvec3{1, 0, 0});
		fold_metatibia_r.rotation = math::angle_axis(-metatibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float metatarsomere1_fold_angle = math::radians(0.0f);
		auto fold_metatarsomere1_l = math::transform<float>::identity();
		auto fold_metatarsomere1_r = math::transform<float>::identity();
		fold_metatarsomere1_l.rotation = math::angle_axis(-metatarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_metatarsomere1_r.rotation = math::angle_axis(-metatarsomere1_fold_angle, math::fvec3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.metacoxa_l, rest_pose.get_relative_transform(bones.metacoxa_l) * fold_metacoxa_l);
		pupa_pose.set_relative_transform(bones.metacoxa_r, rest_pose.get_relative_transform(bones.metacoxa_r) * fold_metacoxa_r);
		pupa_pose.set_relative_transform(bones.metafemur_l, rest_pose.get_relative_transform(bones.metafemur_l) * fold_metafemur_l);
		pupa_pose.set_relative_transform(bones.metafemur_r, rest_pose.get_relative_transform(bones.metafemur_r) * fold_metafemur_r);
		pupa_pose.set_relative_transform(bones.metatibia_l, rest_pose.get_relative_transform(bones.metatibia_l) * fold_metatibia_l);
		pupa_pose.set_relative_transform(bones.metatibia_r, rest_pose.get_relative_transform(bones.metatibia_r) * fold_metatibia_r);
		pupa_pose.set_relative_transform(bones.metatarsomere1_l, rest_pose.get_relative_transform(bones.metatarsomere1_l) * fold_metatarsomere1_l);
		pupa_pose.set_relative_transform(bones.metatarsomere1_r, rest_pose.get_relative_transform(bones.metatarsomere1_r) * fold_metatarsomere1_r);
	}
	
	// Fold head
	{
		constexpr float head_fold_angle = math::radians(80.0f);
		auto fold_head = math::transform<float>::identity();
		fold_head.rotation = math::angle_axis(-head_fold_angle, math::fvec3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.head, rest_pose.get_relative_transform(bones.head) * fold_head);
	}
	
	// Fold antennae
	{
		constexpr float antennomere1_fold_angle_y = math::radians(70.0f);
		constexpr float antennomere1_fold_angle_x = math::radians(45.0f);
		auto fold_antennomere1_l = math::transform<float>::identity();
		auto fold_antennomere1_r = math::transform<float>::identity();
		fold_antennomere1_l.rotation = math::angle_axis(-antennomere1_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, math::fvec3{1, 0, 0});
		fold_antennomere1_r.rotation = math::angle_axis(antennomere1_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float antennomere2_fold_angle_y = math::radians(75.0f);
		constexpr float antennomere2_fold_angle_x = math::radians(25.0f);
		auto fold_antennomere2_l = math::transform<float>::identity();
		auto fold_antennomere2_r = math::transform<float>::identity();
		fold_antennomere2_l.rotation = math::angle_axis(antennomere2_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, math::fvec3{1, 0, 0});
		fold_antennomere2_r.rotation = math::angle_axis(-antennomere2_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, math::fvec3{1, 0, 0});
		
		pupa_pose.set_relative_transform(bones.antennomere1_l, rest_pose.get_relative_transform(bones.antennomere1_l) * fold_antennomere1_l);
		pupa_pose.set_relative_transform(bones.antennomere1_r, rest_pose.get_relative_transform(bones.antennomere1_r) * fold_antennomere1_r);
		pupa_pose.set_relative_transform(bones.antennomere2_l, rest_pose.get_relative_transform(bones.antennomere2_l) * fold_antennomere2_l);
		pupa_pose.set_relative_transform(bones.antennomere2_r, rest_pose.get_relative_transform(bones.antennomere2_r) * fold_antennomere2_r);
	}
	
	// Fold waist + gaster
	{
		constexpr float petiole_fold_angle = math::radians(40.0f);
		auto fold_petiole = math::transform<float>::identity();
		fold_petiole.rotation = math::angle_axis(petiole_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float postpetiole_fold_angle = math::radians(35.0f);
		auto fold_postpetiole = math::transform<float>::identity();
		fold_postpetiole.rotation = math::angle_axis(-postpetiole_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float gaster_fold_angle = math::radians(0.0f);
		auto fold_gaster = math::transform<float>::identity();
		fold_gaster.rotation = math::angle_axis(-gaster_fold_angle, math::fvec3{1, 0, 0});
		
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
		// invert_mesosoma.rotation = math::angle_axis(mesosoma_invert_angle, math::fvec3{0, 0, 1});
		
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
	bones.protarsomere1_l = ++bone_index;
	bones.procoxa_r = ++bone_index;
	bones.profemur_r = ++bone_index;
	bones.protibia_r = ++bone_index;
	bones.protarsomere1_r = ++bone_index;
	bones.mesocoxa_l = ++bone_index;
	bones.mesofemur_l = ++bone_index;
	bones.mesotibia_l = ++bone_index;
	bones.mesotarsomere1_l = ++bone_index;
	bones.mesocoxa_r = ++bone_index;
	bones.mesofemur_r = ++bone_index;
	bones.mesotibia_r = ++bone_index;
	bones.mesotarsomere1_r = ++bone_index;
	bones.metacoxa_l = ++bone_index;
	bones.metafemur_l = ++bone_index;
	bones.metatibia_l = ++bone_index;
	bones.metatarsomere1_l = ++bone_index;
	bones.metacoxa_r = ++bone_index;
	bones.metafemur_r = ++bone_index;
	bones.metatibia_r = ++bone_index;
	bones.metatarsomere1_r = ++bone_index;
	bones.head = ++bone_index;
	bones.mandible_l = ++bone_index;
	bones.mandible_r = ++bone_index;
	bones.antennomere1_l = ++bone_index;
	bones.antennomere2_l = ++bone_index;
	bones.antennomere1_r = ++bone_index;
	bones.antennomere2_r = ++bone_index;
	
	if (phenome.waist->present)
	{
		bones.petiole = ++bone_index;
		
		if (phenome.waist->postpetiole_present)
		{
			bones.postpetiole = ++bone_index;
		}
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
	skeleton.set_bone_parent(bones.mesosoma,         bones.mesosoma);
	skeleton.set_bone_parent(bones.procoxa_l,        bones.mesosoma);
	skeleton.set_bone_parent(bones.profemur_l,       bones.procoxa_l);
	skeleton.set_bone_parent(bones.protibia_l,       bones.profemur_l);
	skeleton.set_bone_parent(bones.protarsomere1_l,  bones.protibia_l);
	skeleton.set_bone_parent(bones.procoxa_r,        bones.mesosoma);
	skeleton.set_bone_parent(bones.profemur_r,       bones.procoxa_r);
	skeleton.set_bone_parent(bones.protibia_r,       bones.profemur_r);
	skeleton.set_bone_parent(bones.protarsomere1_r,  bones.protibia_r);
	skeleton.set_bone_parent(bones.mesocoxa_l,       bones.mesosoma);
	skeleton.set_bone_parent(bones.mesofemur_l,      bones.mesocoxa_l);
	skeleton.set_bone_parent(bones.mesotibia_l,      bones.mesofemur_l);
	skeleton.set_bone_parent(bones.mesotarsomere1_l, bones.mesotibia_l);
	skeleton.set_bone_parent(bones.mesocoxa_r,       bones.mesosoma);
	skeleton.set_bone_parent(bones.mesofemur_r,      bones.mesocoxa_r);
	skeleton.set_bone_parent(bones.mesotibia_r,      bones.mesofemur_r);
	skeleton.set_bone_parent(bones.mesotarsomere1_r, bones.mesotibia_r);
	skeleton.set_bone_parent(bones.metacoxa_l,       bones.mesosoma);
	skeleton.set_bone_parent(bones.metafemur_l,      bones.metacoxa_l);
	skeleton.set_bone_parent(bones.metatibia_l,      bones.metafemur_l);
	skeleton.set_bone_parent(bones.metatarsomere1_l, bones.metatibia_l);
	skeleton.set_bone_parent(bones.metacoxa_r,       bones.mesosoma);
	skeleton.set_bone_parent(bones.metafemur_r,      bones.metacoxa_r);
	skeleton.set_bone_parent(bones.metatibia_r,      bones.metafemur_r);
	skeleton.set_bone_parent(bones.metatarsomere1_r, bones.metatibia_r);
	skeleton.set_bone_parent(bones.head,             bones.mesosoma);
	skeleton.set_bone_parent(bones.mandible_l,       bones.head);
	skeleton.set_bone_parent(bones.mandible_r,       bones.head);
	skeleton.set_bone_parent(bones.antennomere1_l,   bones.head);
	skeleton.set_bone_parent(bones.antennomere2_l,   bones.antennomere1_l);
	skeleton.set_bone_parent(bones.antennomere1_r,   bones.head);
	skeleton.set_bone_parent(bones.antennomere2_r,   bones.antennomere1_r);
	
	if (phenome.waist->present)
	{
		skeleton.set_bone_parent(*bones.petiole, bones.mesosoma);
		
		if (phenome.waist->postpetiole_present)
		{
			skeleton.set_bone_parent(*bones.postpetiole, *bones.petiole);
			skeleton.set_bone_parent(bones.gaster, *bones.postpetiole);
		}
		else
		{
			skeleton.set_bone_parent(bones.gaster, *bones.petiole);
		}
	}
	else
	{
		skeleton.set_bone_parent(bones.gaster, bones.mesosoma);
	}
	
	if (phenome.sting->present)
	{
		skeleton.set_bone_parent(*bones.sting, bones.gaster);
	}
	
	if (phenome.wings->present)
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
	skeleton.set_bone_name(bones.protarsomere1_l, "protarsomere1_l");
	skeleton.set_bone_name(bones.procoxa_r, "procoxa_r");
	skeleton.set_bone_name(bones.profemur_r, "profemur_r");
	skeleton.set_bone_name(bones.protibia_r, "protibia_r");
	skeleton.set_bone_name(bones.protarsomere1_r, "protarsomere1_r");
	skeleton.set_bone_name(bones.mesocoxa_l, "mesocoxa_l");
	skeleton.set_bone_name(bones.mesofemur_l, "mesofemur_l");
	skeleton.set_bone_name(bones.mesotibia_l, "mesotibia_l");
	skeleton.set_bone_name(bones.mesotarsomere1_l, "mesotarsomere1_l");
	skeleton.set_bone_name(bones.mesocoxa_r, "mesocoxa_r");
	skeleton.set_bone_name(bones.mesofemur_r, "mesofemur_r");
	skeleton.set_bone_name(bones.mesotibia_r, "mesotibia_r");
	skeleton.set_bone_name(bones.mesotarsomere1_r, "mesotarsomere1_r");
	skeleton.set_bone_name(bones.metacoxa_l, "metacoxa_l");
	skeleton.set_bone_name(bones.metafemur_l, "metafemur_l");
	skeleton.set_bone_name(bones.metatibia_l, "metatibia_l");
	skeleton.set_bone_name(bones.metatarsomere1_l, "metatarsomere1_l");
	skeleton.set_bone_name(bones.metacoxa_r, "metacoxa_r");
	skeleton.set_bone_name(bones.metafemur_r, "metafemur_r");
	skeleton.set_bone_name(bones.metatibia_r, "metatibia_r");
	skeleton.set_bone_name(bones.metatarsomere1_r, "metatarsomere1_r");
	skeleton.set_bone_name(bones.head, "head");
	skeleton.set_bone_name(bones.mandible_l, "mandible_l");
	skeleton.set_bone_name(bones.mandible_r, "mandible_r");
	skeleton.set_bone_name(bones.antennomere1_l, "antennomere1_l");
	skeleton.set_bone_name(bones.antennomere2_l, "antennomere2_l");
	skeleton.set_bone_name(bones.antennomere1_r, "antennomere1_r");
	skeleton.set_bone_name(bones.antennomere2_r, "antennomere2_r");
	
	if (phenome.waist->present)
	{
		skeleton.set_bone_name(*bones.petiole, "petiole");
		
		if (phenome.waist->postpetiole_present)
		{
			skeleton.set_bone_name(*bones.postpetiole, "postpetiole");
		}
	}
	
	skeleton.set_bone_name(bones.gaster, "gaster");
	
	if (phenome.sting->present)
	{
		skeleton.set_bone_name(*bones.sting, "sting");
	}
	
	if (phenome.wings->present)
	{
		skeleton.set_bone_name(*bones.forewing_l, "forewing_l");
		skeleton.set_bone_name(*bones.forewing_r, "forewing_r");
		skeleton.set_bone_name(*bones.hindwing_l, "hindwing_l");
		skeleton.set_bone_name(*bones.hindwing_r, "hindwing_r");
	}
	
	// Generate poses
	generate_ant_rest_pose(skeleton, bones, phenome);
	generate_ant_midstance_pose(skeleton, bones);
	generate_ant_midswing_pose(skeleton, bones);
	generate_ant_liftoff_pose(skeleton, bones);
	generate_ant_touchdown_pose(skeleton, bones);
	generate_ant_pupa_pose(skeleton, bones);
}
