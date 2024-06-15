// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/ant-skeleton.hpp"
#include "game/ant/ant-bone-set.hpp"
#include <engine/math/functions.hpp>
#include <engine/math/euler-angles.hpp>

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
	
	auto get_bone_transform = [](const ::skeleton& skeleton, const std::string& bone_name)
	{
		return skeleton.rest_pose().get_relative_transform(skeleton.bones().at(bone_name).index());
	};

	auto& rest_pose = skeleton.rest_pose();
	
	// Build skeleton rest pose
	rest_pose.set_relative_transform(bones.mesosoma->index(), get_bone_transform(mesosoma_skeleton, "mesosoma"));
	
	rest_pose.set_relative_transform(bones.procoxa_l->index(), get_bone_transform(mesosoma_skeleton, "procoxa_socket_l") * get_bone_transform(legs_skeleton, "procoxa_l"));
	rest_pose.set_relative_transform(bones.profemur_l->index(), get_bone_transform(legs_skeleton, "profemur_l"));
	rest_pose.set_relative_transform(bones.protibia_l->index(), get_bone_transform(legs_skeleton, "protibia_l"));
	rest_pose.set_relative_transform(bones.protarsomere1_l->index(), get_bone_transform(legs_skeleton, "protarsomere1_l"));
	
	rest_pose.set_relative_transform(bones.procoxa_r->index(), get_bone_transform(mesosoma_skeleton, "procoxa_socket_r") * get_bone_transform(legs_skeleton, "procoxa_r"));
	rest_pose.set_relative_transform(bones.profemur_r->index(), get_bone_transform(legs_skeleton, "profemur_r"));
	rest_pose.set_relative_transform(bones.protibia_r->index(), get_bone_transform(legs_skeleton, "protibia_r"));
	rest_pose.set_relative_transform(bones.protarsomere1_r->index(), get_bone_transform(legs_skeleton, "protarsomere1_r"));
	
	rest_pose.set_relative_transform(bones.mesocoxa_l->index(), get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_l") * get_bone_transform(legs_skeleton, "mesocoxa_l"));
	rest_pose.set_relative_transform(bones.mesofemur_l->index(), get_bone_transform(legs_skeleton, "mesofemur_l"));
	rest_pose.set_relative_transform(bones.mesotibia_l->index(), get_bone_transform(legs_skeleton, "mesotibia_l"));
	rest_pose.set_relative_transform(bones.mesotarsomere1_l->index(), get_bone_transform(legs_skeleton, "mesotarsomere1_l"));
	
	rest_pose.set_relative_transform(bones.mesocoxa_r->index(), get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_r") * get_bone_transform(legs_skeleton, "mesocoxa_r"));
	rest_pose.set_relative_transform(bones.mesofemur_r->index(), get_bone_transform(legs_skeleton, "mesofemur_r"));
	rest_pose.set_relative_transform(bones.mesotibia_r->index(), get_bone_transform(legs_skeleton, "mesotibia_r"));
	rest_pose.set_relative_transform(bones.mesotarsomere1_r->index(), get_bone_transform(legs_skeleton, "mesotarsomere1_r"));
	
	rest_pose.set_relative_transform(bones.metacoxa_l->index(), get_bone_transform(mesosoma_skeleton, "metacoxa_socket_l") * get_bone_transform(legs_skeleton, "metacoxa_l"));
	rest_pose.set_relative_transform(bones.metafemur_l->index(), get_bone_transform(legs_skeleton, "metafemur_l"));
	rest_pose.set_relative_transform(bones.metatibia_l->index(), get_bone_transform(legs_skeleton, "metatibia_l"));
	rest_pose.set_relative_transform(bones.metatarsomere1_l->index(), get_bone_transform(legs_skeleton, "metatarsomere1_l"));
	
	rest_pose.set_relative_transform(bones.metacoxa_r->index(), get_bone_transform(mesosoma_skeleton, "metacoxa_socket_r") * get_bone_transform(legs_skeleton, "metacoxa_r"));
	rest_pose.set_relative_transform(bones.metafemur_r->index(), get_bone_transform(legs_skeleton, "metafemur_r"));
	rest_pose.set_relative_transform(bones.metatibia_r->index(), get_bone_transform(legs_skeleton, "metatibia_r"));
	rest_pose.set_relative_transform(bones.metatarsomere1_r->index(), get_bone_transform(legs_skeleton, "metatarsomere1_r"));
	
	rest_pose.set_relative_transform(bones.head->index(), get_bone_transform(mesosoma_skeleton, "head_socket") * get_bone_transform(head_skeleton, "head"));
	rest_pose.set_relative_transform(bones.mandible_l->index(), get_bone_transform(head_skeleton, "mandible_socket_l") * get_bone_transform(mandibles_skeleton, "mandible_l"));
	rest_pose.set_relative_transform(bones.mandible_r->index(), get_bone_transform(head_skeleton, "mandible_socket_r") * get_bone_transform(mandibles_skeleton, "mandible_r"));
	rest_pose.set_relative_transform(bones.antennomere1_l->index(), get_bone_transform(head_skeleton, "antenna_socket_l") * get_bone_transform(antennae_skeleton, "antennomere1_l"));
	rest_pose.set_relative_transform(bones.antennomere2_l->index(), get_bone_transform(antennae_skeleton, "antennomere2_l"));
	rest_pose.set_relative_transform(bones.antennomere1_r->index(), get_bone_transform(head_skeleton, "antenna_socket_r") * get_bone_transform(antennae_skeleton, "antennomere1_r"));
	rest_pose.set_relative_transform(bones.antennomere2_r->index(), get_bone_transform(antennae_skeleton, "antennomere2_r"));
	
	if (phenome.waist->present)
	{
		rest_pose.set_relative_transform(bones.petiole->index(), get_bone_transform(mesosoma_skeleton, "petiole_socket") * get_bone_transform(waist_skeleton, "petiole"));
		
		if (phenome.waist->postpetiole_present)
		{
			rest_pose.set_relative_transform(bones.postpetiole->index(), get_bone_transform(waist_skeleton, "postpetiole"));
		}
		
		rest_pose.set_relative_transform(bones.gaster->index(), get_bone_transform(waist_skeleton, "gaster_socket") * get_bone_transform(gaster_skeleton, "gaster"));
	}
	else
	{
		rest_pose.set_relative_transform(bones.gaster->index(), get_bone_transform(mesosoma_skeleton, "petiole_socket") * get_bone_transform(gaster_skeleton, "gaster"));
	}
	
	if (phenome.sting->present)
	{
		rest_pose.set_relative_transform(bones.sting->index(), get_bone_transform(gaster_skeleton, "sting_socket") * get_bone_transform(*sting_skeleton, "sting"));
	}
	
	if (phenome.wings->present)
	{
		rest_pose.set_relative_transform(bones.forewing_l->index(), get_bone_transform(mesosoma_skeleton, "forewing_socket_l") * get_bone_transform(*wings_skeleton, "forewing_l"));
		rest_pose.set_relative_transform(bones.forewing_r->index(), get_bone_transform(mesosoma_skeleton, "forewing_socket_r") * get_bone_transform(*wings_skeleton, "forewing_r"));
		rest_pose.set_relative_transform(bones.hindwing_l->index(), get_bone_transform(mesosoma_skeleton, "hindwing_socket_l") * get_bone_transform(*wings_skeleton, "hindwing_l"));
		rest_pose.set_relative_transform(bones.hindwing_r->index(), get_bone_transform(mesosoma_skeleton, "hindwing_socket_r") * get_bone_transform(*wings_skeleton, "hindwing_r"));
	}

	rest_pose.update();
}

std::unique_ptr<skeleton_pose> generate_ant_midstance_pose(::skeleton& skeleton)
{
	const auto& rest_pose = skeleton.rest_pose();
	const auto& bones = skeleton.bones();

	auto pose = std::make_unique<skeleton_pose>(skeleton);
	
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
		
		auto procoxa_l_xf = math::identity<math::transform<float>>;
		auto procoxa_r_xf = math::identity<math::transform<float>>;
		auto profemur_l_xf = math::identity<math::transform<float>>;
		auto profemur_r_xf = math::identity<math::transform<float>>;
		auto protibia_l_xf = math::identity<math::transform<float>>;
		auto protibia_r_xf = math::identity<math::transform<float>>;
		auto protarsomere1_l_xf = math::identity<math::transform<float>>;
		auto protarsomere1_r_xf = math::identity<math::transform<float>>;
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("procoxa_l").index(), rest_pose.get_relative_transform(bones.at("procoxa_l").index()) * procoxa_l_xf);
		pose->set_relative_transform(bones.at("procoxa_r").index(), rest_pose.get_relative_transform(bones.at("procoxa_r").index()) * procoxa_r_xf);
		pose->set_relative_transform(bones.at("profemur_l").index(), rest_pose.get_relative_transform(bones.at("profemur_l").index()) * profemur_l_xf);
		pose->set_relative_transform(bones.at("profemur_r").index(), rest_pose.get_relative_transform(bones.at("profemur_r").index()) * profemur_r_xf);
		pose->set_relative_transform(bones.at("protibia_l").index(), rest_pose.get_relative_transform(bones.at("protibia_l").index()) * protibia_l_xf);
		pose->set_relative_transform(bones.at("protibia_r").index(), rest_pose.get_relative_transform(bones.at("protibia_r").index()) * protibia_r_xf);
		pose->set_relative_transform(bones.at("protarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_l").index()) * protarsomere1_l_xf);
		pose->set_relative_transform(bones.at("protarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_r").index()) * protarsomere1_r_xf);
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
		
		auto mesocoxa_l_xf = math::identity<math::transform<float>>;
		auto mesocoxa_r_xf = math::identity<math::transform<float>>;
		auto mesofemur_l_xf = math::identity<math::transform<float>>;
		auto mesofemur_r_xf = math::identity<math::transform<float>>;
		auto mesotibia_l_xf = math::identity<math::transform<float>>;
		auto mesotibia_r_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_l_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_r_xf = math::identity<math::transform<float>>;
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("mesocoxa_l").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_l").index()) * mesocoxa_l_xf);
		pose->set_relative_transform(bones.at("mesocoxa_r").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_r").index()) * mesocoxa_r_xf);
		pose->set_relative_transform(bones.at("mesofemur_l").index(), rest_pose.get_relative_transform(bones.at("mesofemur_l").index()) * mesofemur_l_xf);
		pose->set_relative_transform(bones.at("mesofemur_r").index(), rest_pose.get_relative_transform(bones.at("mesofemur_r").index()) * mesofemur_r_xf);
		pose->set_relative_transform(bones.at("mesotibia_l").index(), rest_pose.get_relative_transform(bones.at("mesotibia_l").index()) * mesotibia_l_xf);
		pose->set_relative_transform(bones.at("mesotibia_r").index(), rest_pose.get_relative_transform(bones.at("mesotibia_r").index()) * mesotibia_r_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_l").index()) * mesotarsomere1_l_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_r").index()) * mesotarsomere1_r_xf);
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
		
		auto metacoxa_l_xf = math::identity<math::transform<float>>;
		auto metacoxa_r_xf = math::identity<math::transform<float>>;
		auto metafemur_l_xf = math::identity<math::transform<float>>;
		auto metafemur_r_xf = math::identity<math::transform<float>>;
		auto metatibia_l_xf = math::identity<math::transform<float>>;
		auto metatibia_r_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_l_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_r_xf = math::identity<math::transform<float>>;
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("metacoxa_l").index(), rest_pose.get_relative_transform(bones.at("metacoxa_l").index()) * metacoxa_l_xf);
		pose->set_relative_transform(bones.at("metacoxa_r").index(), rest_pose.get_relative_transform(bones.at("metacoxa_r").index()) * metacoxa_r_xf);
		pose->set_relative_transform(bones.at("metafemur_l").index(), rest_pose.get_relative_transform(bones.at("metafemur_l").index()) * metafemur_l_xf);
		pose->set_relative_transform(bones.at("metafemur_r").index(), rest_pose.get_relative_transform(bones.at("metafemur_r").index()) * metafemur_r_xf);
		pose->set_relative_transform(bones.at("metatibia_l").index(), rest_pose.get_relative_transform(bones.at("metatibia_l").index()) * metatibia_l_xf);
		pose->set_relative_transform(bones.at("metatibia_r").index(), rest_pose.get_relative_transform(bones.at("metatibia_r").index()) * metatibia_r_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_l").index()) * metatarsomere1_l_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_r").index()) * metatarsomere1_r_xf);
	}
	
	pose->update();

	return pose;
}

std::unique_ptr<skeleton_pose> generate_ant_liftoff_pose(::skeleton& skeleton)
{
	const auto& rest_pose = skeleton.rest_pose();
	const auto& bones = skeleton.bones();

	auto pose = std::make_unique<skeleton_pose>(skeleton);
	
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
		
		auto procoxa_l_xf = math::identity<math::transform<float>>;
		auto procoxa_r_xf = math::identity<math::transform<float>>;
		auto profemur_l_xf = math::identity<math::transform<float>>;
		auto profemur_r_xf = math::identity<math::transform<float>>;
		auto protibia_l_xf = math::identity<math::transform<float>>;
		auto protibia_r_xf = math::identity<math::transform<float>>;
		auto protarsomere1_l_xf = math::identity<math::transform<float>>;
		auto protarsomere1_r_xf = math::identity<math::transform<float>>;
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("procoxa_l").index(), rest_pose.get_relative_transform(bones.at("procoxa_l").index()) * procoxa_l_xf);
		pose->set_relative_transform(bones.at("procoxa_r").index(), rest_pose.get_relative_transform(bones.at("procoxa_r").index()) * procoxa_r_xf);
		pose->set_relative_transform(bones.at("profemur_l").index(), rest_pose.get_relative_transform(bones.at("profemur_l").index()) * profemur_l_xf);
		pose->set_relative_transform(bones.at("profemur_r").index(), rest_pose.get_relative_transform(bones.at("profemur_r").index()) * profemur_r_xf);
		pose->set_relative_transform(bones.at("protibia_l").index(), rest_pose.get_relative_transform(bones.at("protibia_l").index()) * protibia_l_xf);
		pose->set_relative_transform(bones.at("protibia_r").index(), rest_pose.get_relative_transform(bones.at("protibia_r").index()) * protibia_r_xf);
		pose->set_relative_transform(bones.at("protarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_l").index()) * protarsomere1_l_xf);
		pose->set_relative_transform(bones.at("protarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_r").index()) * protarsomere1_r_xf);
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
		
		auto mesocoxa_l_xf = math::identity<math::transform<float>>;
		auto mesocoxa_r_xf = math::identity<math::transform<float>>;
		auto mesofemur_l_xf = math::identity<math::transform<float>>;
		auto mesofemur_r_xf = math::identity<math::transform<float>>;
		auto mesotibia_l_xf = math::identity<math::transform<float>>;
		auto mesotibia_r_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_l_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_r_xf = math::identity<math::transform<float>>;
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("mesocoxa_l").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_l").index()) * mesocoxa_l_xf);
		pose->set_relative_transform(bones.at("mesocoxa_r").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_r").index()) * mesocoxa_r_xf);
		pose->set_relative_transform(bones.at("mesofemur_l").index(), rest_pose.get_relative_transform(bones.at("mesofemur_l").index()) * mesofemur_l_xf);
		pose->set_relative_transform(bones.at("mesofemur_r").index(), rest_pose.get_relative_transform(bones.at("mesofemur_r").index()) * mesofemur_r_xf);
		pose->set_relative_transform(bones.at("mesotibia_l").index(), rest_pose.get_relative_transform(bones.at("mesotibia_l").index()) * mesotibia_l_xf);
		pose->set_relative_transform(bones.at("mesotibia_r").index(), rest_pose.get_relative_transform(bones.at("mesotibia_r").index()) * mesotibia_r_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_l").index()) * mesotarsomere1_l_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_r").index()) * mesotarsomere1_r_xf);
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
		
		auto metacoxa_l_xf = math::identity<math::transform<float>>;
		auto metacoxa_r_xf = math::identity<math::transform<float>>;
		auto metafemur_l_xf = math::identity<math::transform<float>>;
		auto metafemur_r_xf = math::identity<math::transform<float>>;
		auto metatibia_l_xf = math::identity<math::transform<float>>;
		auto metatibia_r_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_l_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_r_xf = math::identity<math::transform<float>>;
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("metacoxa_l").index(), rest_pose.get_relative_transform(bones.at("metacoxa_l").index()) * metacoxa_l_xf);
		pose->set_relative_transform(bones.at("metacoxa_r").index(), rest_pose.get_relative_transform(bones.at("metacoxa_r").index()) * metacoxa_r_xf);
		pose->set_relative_transform(bones.at("metafemur_l").index(), rest_pose.get_relative_transform(bones.at("metafemur_l").index()) * metafemur_l_xf);
		pose->set_relative_transform(bones.at("metafemur_r").index(), rest_pose.get_relative_transform(bones.at("metafemur_r").index()) * metafemur_r_xf);
		pose->set_relative_transform(bones.at("metatibia_l").index(), rest_pose.get_relative_transform(bones.at("metatibia_l").index()) * metatibia_l_xf);
		pose->set_relative_transform(bones.at("metatibia_r").index(), rest_pose.get_relative_transform(bones.at("metatibia_r").index()) * metatibia_r_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_l").index()) * metatarsomere1_l_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_r").index()) * metatarsomere1_r_xf);
	}
	
	pose->update();

	return pose;
}

std::unique_ptr<skeleton_pose> generate_ant_touchdown_pose(::skeleton& skeleton)
{
	const auto& rest_pose = skeleton.rest_pose();
	const auto& bones = skeleton.bones();

	auto pose = std::make_unique<skeleton_pose>(skeleton);
	
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
		
		auto procoxa_l_xf = math::identity<math::transform<float>>;
		auto procoxa_r_xf = math::identity<math::transform<float>>;
		auto profemur_l_xf = math::identity<math::transform<float>>;
		auto profemur_r_xf = math::identity<math::transform<float>>;
		auto protibia_l_xf = math::identity<math::transform<float>>;
		auto protibia_r_xf = math::identity<math::transform<float>>;
		auto protarsomere1_l_xf = math::identity<math::transform<float>>;
		auto protarsomere1_r_xf = math::identity<math::transform<float>>;
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("procoxa_l").index(), rest_pose.get_relative_transform(bones.at("procoxa_l").index()) * procoxa_l_xf);
		pose->set_relative_transform(bones.at("procoxa_r").index(), rest_pose.get_relative_transform(bones.at("procoxa_r").index()) * procoxa_r_xf);
		pose->set_relative_transform(bones.at("profemur_l").index(), rest_pose.get_relative_transform(bones.at("profemur_l").index()) * profemur_l_xf);
		pose->set_relative_transform(bones.at("profemur_r").index(), rest_pose.get_relative_transform(bones.at("profemur_r").index()) * profemur_r_xf);
		pose->set_relative_transform(bones.at("protibia_l").index(), rest_pose.get_relative_transform(bones.at("protibia_l").index()) * protibia_l_xf);
		pose->set_relative_transform(bones.at("protibia_r").index(), rest_pose.get_relative_transform(bones.at("protibia_r").index()) * protibia_r_xf);
		pose->set_relative_transform(bones.at("protarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_l").index()) * protarsomere1_l_xf);
		pose->set_relative_transform(bones.at("protarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_r").index()) * protarsomere1_r_xf);
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
		
		auto mesocoxa_l_xf = math::identity<math::transform<float>>;
		auto mesocoxa_r_xf = math::identity<math::transform<float>>;
		auto mesofemur_l_xf = math::identity<math::transform<float>>;
		auto mesofemur_r_xf = math::identity<math::transform<float>>;
		auto mesotibia_l_xf = math::identity<math::transform<float>>;
		auto mesotibia_r_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_l_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_r_xf = math::identity<math::transform<float>>;
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("mesocoxa_l").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_l").index()) * mesocoxa_l_xf);
		pose->set_relative_transform(bones.at("mesocoxa_r").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_r").index()) * mesocoxa_r_xf);
		pose->set_relative_transform(bones.at("mesofemur_l").index(), rest_pose.get_relative_transform(bones.at("mesofemur_l").index()) * mesofemur_l_xf);
		pose->set_relative_transform(bones.at("mesofemur_r").index(), rest_pose.get_relative_transform(bones.at("mesofemur_r").index()) * mesofemur_r_xf);
		pose->set_relative_transform(bones.at("mesotibia_l").index(), rest_pose.get_relative_transform(bones.at("mesotibia_l").index()) * mesotibia_l_xf);
		pose->set_relative_transform(bones.at("mesotibia_r").index(), rest_pose.get_relative_transform(bones.at("mesotibia_r").index()) * mesotibia_r_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_l").index()) * mesotarsomere1_l_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_r").index()) * mesotarsomere1_r_xf);
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
		
		auto metacoxa_l_xf = math::identity<math::transform<float>>;
		auto metacoxa_r_xf = math::identity<math::transform<float>>;
		auto metafemur_l_xf = math::identity<math::transform<float>>;
		auto metafemur_r_xf = math::identity<math::transform<float>>;
		auto metatibia_l_xf = math::identity<math::transform<float>>;
		auto metatibia_r_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_l_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_r_xf = math::identity<math::transform<float>>;
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("metacoxa_l").index(), rest_pose.get_relative_transform(bones.at("metacoxa_l").index()) * metacoxa_l_xf);
		pose->set_relative_transform(bones.at("metacoxa_r").index(), rest_pose.get_relative_transform(bones.at("metacoxa_r").index()) * metacoxa_r_xf);
		pose->set_relative_transform(bones.at("metafemur_l").index(), rest_pose.get_relative_transform(bones.at("metafemur_l").index()) * metafemur_l_xf);
		pose->set_relative_transform(bones.at("metafemur_r").index(), rest_pose.get_relative_transform(bones.at("metafemur_r").index()) * metafemur_r_xf);
		pose->set_relative_transform(bones.at("metatibia_l").index(), rest_pose.get_relative_transform(bones.at("metatibia_l").index()) * metatibia_l_xf);
		pose->set_relative_transform(bones.at("metatibia_r").index(), rest_pose.get_relative_transform(bones.at("metatibia_r").index()) * metatibia_r_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_l").index()) * metatarsomere1_l_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_r").index()) * metatarsomere1_r_xf);
	}
	
	pose->update();

	return pose;
}

std::unique_ptr<skeleton_pose> generate_ant_midswing_pose(::skeleton& skeleton)
{
	const auto& rest_pose = skeleton.rest_pose();
	const auto& bones = skeleton.bones();

	auto pose = std::make_unique<skeleton_pose>(skeleton);
	
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
		
		auto procoxa_l_xf = math::identity<math::transform<float>>;
		auto procoxa_r_xf = math::identity<math::transform<float>>;
		auto profemur_l_xf = math::identity<math::transform<float>>;
		auto profemur_r_xf = math::identity<math::transform<float>>;
		auto protibia_l_xf = math::identity<math::transform<float>>;
		auto protibia_r_xf = math::identity<math::transform<float>>;
		auto protarsomere1_l_xf = math::identity<math::transform<float>>;
		auto protarsomere1_r_xf = math::identity<math::transform<float>>;
		
		procoxa_l_xf.rotation = math::euler_xyz_to_quat(procoxa_l_angles);
		procoxa_r_xf.rotation = math::euler_xyz_to_quat(procoxa_r_angles);
		profemur_l_xf.rotation = math::euler_xyz_to_quat(profemur_l_angles);
		profemur_r_xf.rotation = math::euler_xyz_to_quat(profemur_r_angles);
		protibia_l_xf.rotation = math::euler_xyz_to_quat(protibia_l_angles);
		protibia_r_xf.rotation = math::euler_xyz_to_quat(protibia_r_angles);
		protarsomere1_l_xf.rotation = math::euler_xyz_to_quat(protarsomere1_l_angles);
		protarsomere1_r_xf.rotation = math::euler_xyz_to_quat(protarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("procoxa_l").index(), rest_pose.get_relative_transform(bones.at("procoxa_l").index()) * procoxa_l_xf);
		pose->set_relative_transform(bones.at("procoxa_r").index(), rest_pose.get_relative_transform(bones.at("procoxa_r").index()) * procoxa_r_xf);
		pose->set_relative_transform(bones.at("profemur_l").index(), rest_pose.get_relative_transform(bones.at("profemur_l").index()) * profemur_l_xf);
		pose->set_relative_transform(bones.at("profemur_r").index(), rest_pose.get_relative_transform(bones.at("profemur_r").index()) * profemur_r_xf);
		pose->set_relative_transform(bones.at("protibia_l").index(), rest_pose.get_relative_transform(bones.at("protibia_l").index()) * protibia_l_xf);
		pose->set_relative_transform(bones.at("protibia_r").index(), rest_pose.get_relative_transform(bones.at("protibia_r").index()) * protibia_r_xf);
		pose->set_relative_transform(bones.at("protarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_l").index()) * protarsomere1_l_xf);
		pose->set_relative_transform(bones.at("protarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_r").index()) * protarsomere1_r_xf);
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
		
		auto mesocoxa_l_xf = math::identity<math::transform<float>>;
		auto mesocoxa_r_xf = math::identity<math::transform<float>>;
		auto mesofemur_l_xf = math::identity<math::transform<float>>;
		auto mesofemur_r_xf = math::identity<math::transform<float>>;
		auto mesotibia_l_xf = math::identity<math::transform<float>>;
		auto mesotibia_r_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_l_xf = math::identity<math::transform<float>>;
		auto mesotarsomere1_r_xf = math::identity<math::transform<float>>;
		
		mesocoxa_l_xf.rotation = math::euler_xyz_to_quat(mesocoxa_l_angles);
		mesocoxa_r_xf.rotation = math::euler_xyz_to_quat(mesocoxa_r_angles);
		mesofemur_l_xf.rotation = math::euler_xyz_to_quat(mesofemur_l_angles);
		mesofemur_r_xf.rotation = math::euler_xyz_to_quat(mesofemur_r_angles);
		mesotibia_l_xf.rotation = math::euler_xyz_to_quat(mesotibia_l_angles);
		mesotibia_r_xf.rotation = math::euler_xyz_to_quat(mesotibia_r_angles);
		mesotarsomere1_l_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_l_angles);
		mesotarsomere1_r_xf.rotation = math::euler_xyz_to_quat(mesotarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("mesocoxa_l").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_l").index()) * mesocoxa_l_xf);
		pose->set_relative_transform(bones.at("mesocoxa_r").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_r").index()) * mesocoxa_r_xf);
		pose->set_relative_transform(bones.at("mesofemur_l").index(), rest_pose.get_relative_transform(bones.at("mesofemur_l").index()) * mesofemur_l_xf);
		pose->set_relative_transform(bones.at("mesofemur_r").index(), rest_pose.get_relative_transform(bones.at("mesofemur_r").index()) * mesofemur_r_xf);
		pose->set_relative_transform(bones.at("mesotibia_l").index(), rest_pose.get_relative_transform(bones.at("mesotibia_l").index()) * mesotibia_l_xf);
		pose->set_relative_transform(bones.at("mesotibia_r").index(), rest_pose.get_relative_transform(bones.at("mesotibia_r").index()) * mesotibia_r_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_l").index()) * mesotarsomere1_l_xf);
		pose->set_relative_transform(bones.at("mesotarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_r").index()) * mesotarsomere1_r_xf);
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
		
		auto metacoxa_l_xf = math::identity<math::transform<float>>;
		auto metacoxa_r_xf = math::identity<math::transform<float>>;
		auto metafemur_l_xf = math::identity<math::transform<float>>;
		auto metafemur_r_xf = math::identity<math::transform<float>>;
		auto metatibia_l_xf = math::identity<math::transform<float>>;
		auto metatibia_r_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_l_xf = math::identity<math::transform<float>>;
		auto metatarsomere1_r_xf = math::identity<math::transform<float>>;
		
		metacoxa_l_xf.rotation = math::euler_xyz_to_quat(metacoxa_l_angles);
		metacoxa_r_xf.rotation = math::euler_xyz_to_quat(metacoxa_r_angles);
		metafemur_l_xf.rotation = math::euler_xyz_to_quat(metafemur_l_angles);
		metafemur_r_xf.rotation = math::euler_xyz_to_quat(metafemur_r_angles);
		metatibia_l_xf.rotation = math::euler_xyz_to_quat(metatibia_l_angles);
		metatibia_r_xf.rotation = math::euler_xyz_to_quat(metatibia_r_angles);
		metatarsomere1_l_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_l_angles);
		metatarsomere1_r_xf.rotation = math::euler_xyz_to_quat(metatarsomere1_r_angles);
		
		pose->set_relative_transform(bones.at("metacoxa_l").index(), rest_pose.get_relative_transform(bones.at("metacoxa_l").index()) * metacoxa_l_xf);
		pose->set_relative_transform(bones.at("metacoxa_r").index(), rest_pose.get_relative_transform(bones.at("metacoxa_r").index()) * metacoxa_r_xf);
		pose->set_relative_transform(bones.at("metafemur_l").index(), rest_pose.get_relative_transform(bones.at("metafemur_l").index()) * metafemur_l_xf);
		pose->set_relative_transform(bones.at("metafemur_r").index(), rest_pose.get_relative_transform(bones.at("metafemur_r").index()) * metafemur_r_xf);
		pose->set_relative_transform(bones.at("metatibia_l").index(), rest_pose.get_relative_transform(bones.at("metatibia_l").index()) * metatibia_l_xf);
		pose->set_relative_transform(bones.at("metatibia_r").index(), rest_pose.get_relative_transform(bones.at("metatibia_r").index()) * metatibia_r_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_l").index()) * metatarsomere1_l_xf);
		pose->set_relative_transform(bones.at("metatarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_r").index()) * metatarsomere1_r_xf);
	}
	
	pose->update();

	return pose;
}

std::unique_ptr<skeleton_pose> generate_ant_pupa_pose(::skeleton& skeleton)
{
	const auto& rest_pose = skeleton.rest_pose();
	const auto& bones = skeleton.bones();

	auto pose = std::make_unique<skeleton_pose>(skeleton);
	
	// Fold forelegs
	{
		constexpr float procoxa_fold_angle_y = math::radians(30.0f);
		constexpr float procoxa_fold_angle_z = math::radians(25.0f);
		constexpr float procoxa_fold_angle_x = math::radians(15.0f);
		auto fold_procoxa_l = math::identity<math::transform<float>>;
		auto fold_procoxa_r = math::identity<math::transform<float>>;
		fold_procoxa_l.rotation = math::angle_axis(procoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(procoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_procoxa_r.rotation = math::angle_axis(-procoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-procoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(procoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float profemur_fold_angle_z = math::radians(20.0f);
		constexpr float profemur_fold_angle_x = math::radians(80.0f);
		auto fold_profemur_l = math::identity<math::transform<float>>;
		auto fold_profemur_r = math::identity<math::transform<float>>;
		fold_profemur_l.rotation = math::angle_axis(profemur_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, math::fvec3{1, 0, 0});
		fold_profemur_r.rotation = math::angle_axis(-profemur_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(profemur_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float protibia_fold_angle = math::radians(165.0f);
		auto fold_protibia_l = math::identity<math::transform<float>>;
		auto fold_protibia_r = math::identity<math::transform<float>>;
		fold_protibia_l.rotation = math::angle_axis(-protibia_fold_angle, math::fvec3{1, 0, 0});
		fold_protibia_r.rotation = math::angle_axis(-protibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float protarsomere1_fold_angle = math::radians(20.0f);
		auto fold_protarsomere1_l = math::identity<math::transform<float>>;
		auto fold_protarsomere1_r = math::identity<math::transform<float>>;
		fold_protarsomere1_l.rotation = math::angle_axis(-protarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_protarsomere1_r.rotation = math::angle_axis(-protarsomere1_fold_angle, math::fvec3{1, 0, 0});
		
		pose->set_relative_transform(bones.at("procoxa_l").index(), rest_pose.get_relative_transform(bones.at("procoxa_l").index()) * fold_procoxa_l);
		pose->set_relative_transform(bones.at("procoxa_r").index(), rest_pose.get_relative_transform(bones.at("procoxa_r").index()) * fold_procoxa_r);
		pose->set_relative_transform(bones.at("profemur_l").index(), rest_pose.get_relative_transform(bones.at("profemur_l").index()) * fold_profemur_l);
		pose->set_relative_transform(bones.at("profemur_r").index(), rest_pose.get_relative_transform(bones.at("profemur_r").index()) * fold_profemur_r);
		pose->set_relative_transform(bones.at("protibia_l").index(), rest_pose.get_relative_transform(bones.at("protibia_l").index()) * fold_protibia_l);
		pose->set_relative_transform(bones.at("protibia_r").index(), rest_pose.get_relative_transform(bones.at("protibia_r").index()) * fold_protibia_r);
		pose->set_relative_transform(bones.at("protarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_l").index()) * fold_protarsomere1_l);
		pose->set_relative_transform(bones.at("protarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("protarsomere1_r").index()) * fold_protarsomere1_r);
	}
	
	// Fold midlegs
	{
		constexpr float mesocoxa_fold_angle_z = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_y = math::radians(45.0f);
		constexpr float mesocoxa_fold_angle_x = math::radians(10.0f);
		auto fold_mesocoxa_l = math::identity<math::transform<float>>;
		auto fold_mesocoxa_r = math::identity<math::transform<float>>;
		fold_mesocoxa_l.rotation = math::angle_axis(-mesocoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(mesocoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_mesocoxa_r.rotation = math::angle_axis(mesocoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(-mesocoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-mesocoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float mesofemur_fold_angle = math::radians(90.0f);
		auto fold_mesofemur_l = math::identity<math::transform<float>>;
		auto fold_mesofemur_r = math::identity<math::transform<float>>;
		fold_mesofemur_l.rotation = math::angle_axis(mesofemur_fold_angle, math::fvec3{1, 0, 0});
		fold_mesofemur_r.rotation = math::angle_axis(mesofemur_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float mesotibia_fold_angle = math::radians(162.0f);
		auto fold_mesotibia_l = math::identity<math::transform<float>>;
		auto fold_mesotibia_r = math::identity<math::transform<float>>;
		fold_mesotibia_l.rotation = math::angle_axis(-mesotibia_fold_angle, math::fvec3{1, 0, 0});
		fold_mesotibia_r.rotation = math::angle_axis(-mesotibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float mesotarsomere1_fold_angle = math::radians(20.0f);
		auto fold_mesotarsomere1_l = math::identity<math::transform<float>>;
		auto fold_mesotarsomere1_r = math::identity<math::transform<float>>;
		fold_mesotarsomere1_l.rotation = math::angle_axis(-mesotarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_mesotarsomere1_r.rotation = math::angle_axis(-mesotarsomere1_fold_angle, math::fvec3{1, 0, 0});
		
		pose->set_relative_transform(bones.at("mesocoxa_l").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_l").index()) * fold_mesocoxa_l);
		pose->set_relative_transform(bones.at("mesocoxa_r").index(), rest_pose.get_relative_transform(bones.at("mesocoxa_r").index()) * fold_mesocoxa_r);
		pose->set_relative_transform(bones.at("mesofemur_l").index(), rest_pose.get_relative_transform(bones.at("mesofemur_l").index()) * fold_mesofemur_l);
		pose->set_relative_transform(bones.at("mesofemur_r").index(), rest_pose.get_relative_transform(bones.at("mesofemur_r").index()) * fold_mesofemur_r);
		pose->set_relative_transform(bones.at("mesotibia_l").index(), rest_pose.get_relative_transform(bones.at("mesotibia_l").index()) * fold_mesotibia_l);
		pose->set_relative_transform(bones.at("mesotibia_r").index(), rest_pose.get_relative_transform(bones.at("mesotibia_r").index()) * fold_mesotibia_r);
		pose->set_relative_transform(bones.at("mesotarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_l").index()) * fold_mesotarsomere1_l);
		pose->set_relative_transform(bones.at("mesotarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("mesotarsomere1_r").index()) * fold_mesotarsomere1_r);
	}
	
	// Fold hindlegs
	{
		constexpr float metacoxa_fold_angle_z = math::radians(15.0f);
		constexpr float metacoxa_fold_angle_y = math::radians(10.0f);
		constexpr float metacoxa_fold_angle_x = math::radians(25.0f);
		auto fold_metacoxa_l = math::identity<math::transform<float>>;
		auto fold_metacoxa_r = math::identity<math::transform<float>>;
		fold_metacoxa_l.rotation = math::angle_axis(-metacoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(-metacoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, math::fvec3{1, 0, 0});
		fold_metacoxa_r.rotation = math::angle_axis(metacoxa_fold_angle_z, math::fvec3{0, 0, 1}) * math::angle_axis(metacoxa_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(metacoxa_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float metafemur_fold_angle = math::radians(105.0f);
		auto fold_metafemur_l = math::identity<math::transform<float>>;
		auto fold_metafemur_r = math::identity<math::transform<float>>;
		fold_metafemur_l.rotation = math::angle_axis(metafemur_fold_angle, math::fvec3{1, 0, 0});
		fold_metafemur_r.rotation = math::angle_axis(metafemur_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float metatibia_fold_angle = math::radians(165.0f);
		auto fold_metatibia_l = math::identity<math::transform<float>>;
		auto fold_metatibia_r = math::identity<math::transform<float>>;
		fold_metatibia_l.rotation = math::angle_axis(-metatibia_fold_angle, math::fvec3{1, 0, 0});
		fold_metatibia_r.rotation = math::angle_axis(-metatibia_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float metatarsomere1_fold_angle = math::radians(0.0f);
		auto fold_metatarsomere1_l = math::identity<math::transform<float>>;
		auto fold_metatarsomere1_r = math::identity<math::transform<float>>;
		fold_metatarsomere1_l.rotation = math::angle_axis(-metatarsomere1_fold_angle, math::fvec3{1, 0, 0});
		fold_metatarsomere1_r.rotation = math::angle_axis(-metatarsomere1_fold_angle, math::fvec3{1, 0, 0});
			
		pose->set_relative_transform(bones.at("metacoxa_l").index(), rest_pose.get_relative_transform(bones.at("metacoxa_l").index()) * fold_metacoxa_l);
		pose->set_relative_transform(bones.at("metacoxa_r").index(), rest_pose.get_relative_transform(bones.at("metacoxa_r").index()) * fold_metacoxa_r);
		pose->set_relative_transform(bones.at("metafemur_l").index(), rest_pose.get_relative_transform(bones.at("metafemur_l").index()) * fold_metafemur_l);
		pose->set_relative_transform(bones.at("metafemur_r").index(), rest_pose.get_relative_transform(bones.at("metafemur_r").index()) * fold_metafemur_r);
		pose->set_relative_transform(bones.at("metatibia_l").index(), rest_pose.get_relative_transform(bones.at("metatibia_l").index()) * fold_metatibia_l);
		pose->set_relative_transform(bones.at("metatibia_r").index(), rest_pose.get_relative_transform(bones.at("metatibia_r").index()) * fold_metatibia_r);
		pose->set_relative_transform(bones.at("metatarsomere1_l").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_l").index()) * fold_metatarsomere1_l);
		pose->set_relative_transform(bones.at("metatarsomere1_r").index(), rest_pose.get_relative_transform(bones.at("metatarsomere1_r").index()) * fold_metatarsomere1_r);
	}
	
	// Fold head
	{
		constexpr float head_fold_angle = math::radians(80.0f);
		auto fold_head = math::identity<math::transform<float>>;
		fold_head.rotation = math::angle_axis(-head_fold_angle, math::fvec3{1, 0, 0});
		
		pose->set_relative_transform(bones.at("head").index(), rest_pose.get_relative_transform(bones.at("head").index()) * fold_head);
	}
	
	// Fold antennae
	{
		constexpr float antennomere1_fold_angle_y = math::radians(70.0f);
		constexpr float antennomere1_fold_angle_x = math::radians(45.0f);
		auto fold_antennomere1_l = math::identity<math::transform<float>>;
		auto fold_antennomere1_r = math::identity<math::transform<float>>;
		fold_antennomere1_l.rotation = math::angle_axis(-antennomere1_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, math::fvec3{1, 0, 0});
		fold_antennomere1_r.rotation = math::angle_axis(antennomere1_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere1_fold_angle_x, math::fvec3{1, 0, 0});
		
		constexpr float antennomere2_fold_angle_y = math::radians(75.0f);
		constexpr float antennomere2_fold_angle_x = math::radians(25.0f);
		auto fold_antennomere2_l = math::identity<math::transform<float>>;
		auto fold_antennomere2_r = math::identity<math::transform<float>>;
		fold_antennomere2_l.rotation = math::angle_axis(antennomere2_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, math::fvec3{1, 0, 0});
		fold_antennomere2_r.rotation = math::angle_axis(-antennomere2_fold_angle_y, math::fvec3{0, 1, 0}) * math::angle_axis(-antennomere2_fold_angle_x, math::fvec3{1, 0, 0});
		
		pose->set_relative_transform(bones.at("antennomere1_l").index(), rest_pose.get_relative_transform(bones.at("antennomere1_l").index()) * fold_antennomere1_l);
		pose->set_relative_transform(bones.at("antennomere1_r").index(), rest_pose.get_relative_transform(bones.at("antennomere1_r").index()) * fold_antennomere1_r);
		pose->set_relative_transform(bones.at("antennomere2_l").index(), rest_pose.get_relative_transform(bones.at("antennomere2_l").index()) * fold_antennomere2_l);
		pose->set_relative_transform(bones.at("antennomere2_r").index(), rest_pose.get_relative_transform(bones.at("antennomere2_r").index()) * fold_antennomere2_r);
	}
	
	// Fold waist + gaster
	{
		constexpr float petiole_fold_angle = math::radians(40.0f);
		auto fold_petiole = math::identity<math::transform<float>>;
		fold_petiole.rotation = math::angle_axis(petiole_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float postpetiole_fold_angle = math::radians(35.0f);
		auto fold_postpetiole = math::identity<math::transform<float>>;
		fold_postpetiole.rotation = math::angle_axis(-postpetiole_fold_angle, math::fvec3{1, 0, 0});
		
		constexpr float gaster_fold_angle = math::radians(0.0f);
		auto fold_gaster = math::identity<math::transform<float>>;
		fold_gaster.rotation = math::angle_axis(-gaster_fold_angle, math::fvec3{1, 0, 0});
		
		if (bones.contains("petiole"))
		{
			pose->set_relative_transform(bones.at("petiole").index(), rest_pose.get_relative_transform(bones.at("petiole").index()) * fold_petiole);
		}
		
		if (bones.contains("postpetiole"))
		{
			pose->set_relative_transform(bones.at("postpetiole").index(), rest_pose.get_relative_transform(bones.at("postpetiole").index()) * fold_postpetiole);
		}
		
		pose->set_relative_transform(bones.at("gaster").index(), rest_pose.get_relative_transform(bones.at("gaster").index()) * fold_gaster);
	}
	
	pose->update();

	return pose;
}

void generate_ant_skeleton(::skeleton& skeleton, ant_bone_set& bones, const ant_phenome& phenome)
{
	// Count bones
	std::size_t bone_count = 33;
	{
		if (phenome.waist->present)
		{
			++bone_count;
			
			if (phenome.waist->postpetiole_present)
			{
				++bone_count;
			}
		}
		
		if (phenome.sting->present)
		{
			++bone_count;
		}
		
		if (phenome.wings->present)
		{
			bone_count += 4;
		}
	}
	
	// Allocate bones
	skeleton = ::skeleton(bone_count);
	
	// Construct bones
	{
		std::size_t i = 0;

		bones.mesosoma = &skeleton.bones().at(i++);
		bones.procoxa_l = &skeleton.bones().at(i++);
		bones.profemur_l = &skeleton.bones().at(i++);
		bones.protibia_l = &skeleton.bones().at(i++);
		bones.protarsomere1_l = &skeleton.bones().at(i++);
		bones.procoxa_r = &skeleton.bones().at(i++);
		bones.profemur_r = &skeleton.bones().at(i++);
		bones.protibia_r = &skeleton.bones().at(i++);
		bones.protarsomere1_r = &skeleton.bones().at(i++);
		bones.mesocoxa_l = &skeleton.bones().at(i++);
		bones.mesofemur_l = &skeleton.bones().at(i++);
		bones.mesotibia_l = &skeleton.bones().at(i++);
		bones.mesotarsomere1_l = &skeleton.bones().at(i++);
		bones.mesocoxa_r = &skeleton.bones().at(i++);
		bones.mesofemur_r = &skeleton.bones().at(i++);
		bones.mesotibia_r = &skeleton.bones().at(i++);
		bones.mesotarsomere1_r = &skeleton.bones().at(i++);
		bones.metacoxa_l = &skeleton.bones().at(i++);
		bones.metafemur_l = &skeleton.bones().at(i++);
		bones.metatibia_l = &skeleton.bones().at(i++);
		bones.metatarsomere1_l = &skeleton.bones().at(i++);
		bones.metacoxa_r = &skeleton.bones().at(i++);
		bones.metafemur_r = &skeleton.bones().at(i++);
		bones.metatibia_r = &skeleton.bones().at(i++);
		bones.metatarsomere1_r = &skeleton.bones().at(i++);
		bones.head = &skeleton.bones().at(i++);
		bones.mandible_l = &skeleton.bones().at(i++);
		bones.mandible_r = &skeleton.bones().at(i++);
		bones.antennomere1_l = &skeleton.bones().at(i++);
		bones.antennomere2_l = &skeleton.bones().at(i++);
		bones.antennomere1_r = &skeleton.bones().at(i++);
		bones.antennomere2_r = &skeleton.bones().at(i++);
		
		if (phenome.waist->present)
		{
			bones.petiole = &skeleton.bones().at(i++);
			
			if (phenome.waist->postpetiole_present)
			{
				bones.postpetiole = &skeleton.bones().at(i++);
			}
		}
		
		bones.gaster = &skeleton.bones().at(i++);
		
		if (phenome.sting->present)
		{
			bones.sting = &skeleton.bones().at(i++);
		}
		
		if (phenome.wings->present)
		{
			bones.forewing_l = &skeleton.bones().at(i++);
			bones.forewing_r = &skeleton.bones().at(i++);
			bones.hindwing_l = &skeleton.bones().at(i++);
			bones.hindwing_r = &skeleton.bones().at(i++);
		}
	}

	// Construct bone hierarchy
	{
		bones.procoxa_l->reparent(bones.mesosoma);
		bones.profemur_l->reparent(bones.procoxa_l);
		bones.protibia_l->reparent(bones.profemur_l);
		bones.protarsomere1_l->reparent(bones.protibia_l);
		bones.procoxa_r->reparent(bones.mesosoma);
		bones.profemur_r->reparent(bones.procoxa_r);
		bones.protibia_r->reparent(bones.profemur_r);
		bones.protarsomere1_r->reparent(bones.protibia_r);
		bones.mesocoxa_l->reparent(bones.mesosoma);
		bones.mesofemur_l->reparent(bones.mesocoxa_l);
		bones.mesotibia_l->reparent(bones.mesofemur_l);
		bones.mesotarsomere1_l->reparent(bones.mesotibia_l);
		bones.mesocoxa_r->reparent(bones.mesosoma);
		bones.mesofemur_r->reparent(bones.mesocoxa_r);
		bones.mesotibia_r->reparent(bones.mesofemur_r);
		bones.mesotarsomere1_r->reparent(bones.mesotibia_r);
		bones.metacoxa_l->reparent(bones.mesosoma);
		bones.metafemur_l->reparent(bones.metacoxa_l);
		bones.metatibia_l->reparent(bones.metafemur_l);
		bones.metatarsomere1_l->reparent(bones.metatibia_l);
		bones.metacoxa_r->reparent(bones.mesosoma);
		bones.metafemur_r->reparent(bones.metacoxa_r);
		bones.metatibia_r->reparent(bones.metafemur_r);
		bones.metatarsomere1_r->reparent(bones.metatibia_r);
		bones.head->reparent(bones.mesosoma);
		bones.mandible_l->reparent(bones.head);
		bones.mandible_r->reparent(bones.head);
		bones.antennomere1_l->reparent(bones.head);
		bones.antennomere2_l->reparent(bones.antennomere1_l);
		bones.antennomere1_r->reparent(bones.head);
		bones.antennomere2_r->reparent(bones.antennomere1_r);
		
		if (phenome.waist->present)
		{
			bones.petiole->reparent(bones.mesosoma);
			
			if (phenome.waist->postpetiole_present)
			{
				bones.postpetiole->reparent(bones.petiole);
				bones.gaster->reparent(bones.postpetiole);
			}
			else
			{
				bones.gaster->reparent(bones.petiole);
			}
		}
		else
		{
			bones.gaster->reparent(bones.mesosoma);
		}
		
		if (phenome.sting->present)
		{
			bones.sting->reparent(bones.gaster);
		}
		
		if (phenome.wings->present)
		{
			bones.forewing_l->reparent(bones.mesosoma);
			bones.forewing_r->reparent(bones.mesosoma);
			bones.hindwing_l->reparent(bones.mesosoma);
			bones.hindwing_r->reparent(bones.mesosoma);
		}
	}

	// Name bones
	{
		bones.mesosoma->rename("mesosoma");
		bones.procoxa_l->rename("procoxa_l");
		bones.profemur_l->rename("profemur_l");
		bones.protibia_l->rename("protibia_l");
		bones.protarsomere1_l->rename("protarsomere1_l");
		bones.procoxa_r->rename("procoxa_r");
		bones.profemur_r->rename("profemur_r");
		bones.protibia_r->rename("protibia_r");
		bones.protarsomere1_r->rename("protarsomere1_r");
		bones.mesocoxa_l->rename("mesocoxa_l");
		bones.mesofemur_l->rename("mesofemur_l");
		bones.mesotibia_l->rename("mesotibia_l");
		bones.mesotarsomere1_l->rename("mesotarsomere1_l");
		bones.mesocoxa_r->rename("mesocoxa_r");
		bones.mesofemur_r->rename("mesofemur_r");
		bones.mesotibia_r->rename("mesotibia_r");
		bones.mesotarsomere1_r->rename("mesotarsomere1_r");
		bones.metacoxa_l->rename("metacoxa_l");
		bones.metafemur_l->rename("metafemur_l");
		bones.metatibia_l->rename("metatibia_l");
		bones.metatarsomere1_l->rename("metatarsomere1_l");
		bones.metacoxa_r->rename("metacoxa_r");
		bones.metafemur_r->rename("metafemur_r");
		bones.metatibia_r->rename("metatibia_r");
		bones.metatarsomere1_r->rename("metatarsomere1_r");
		bones.head->rename("head");
		bones.mandible_l->rename("mandible_l");
		bones.mandible_r->rename("mandible_r");
		bones.antennomere1_l->rename("antennomere1_l");
		bones.antennomere2_l->rename("antennomere2_l");
		bones.antennomere1_r->rename("antennomere1_r");
		bones.antennomere2_r->rename("antennomere2_r");
		
		if (phenome.waist->present)
		{
			bones.petiole->rename("petiole");
			
			if (phenome.waist->postpetiole_present)
			{
				bones.postpetiole->rename("postpetiole");
			}
		}
		
		bones.gaster->rename("gaster");
		
		if (phenome.sting->present)
		{
			bones.sting->rename("sting");
		}
		
		if (phenome.wings->present)
		{
			bones.forewing_l->rename("forewing_l");
			bones.forewing_r->rename("forewing_r");
			bones.hindwing_l->rename("hindwing_l");
			bones.hindwing_r->rename("hindwing_r");
		}
	}
	
	// Generate poses
	generate_ant_rest_pose(skeleton, bones, phenome);
}
