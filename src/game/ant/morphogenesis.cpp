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

#include "game/ant/morphogenesis.hpp"
#include "render/material.hpp"
#include "render/vertex-attribute.hpp"
#include "math/transform-operators.hpp"
#include "math/quaternion.hpp"
#include <unordered_set>

namespace game {
namespace ant {

static render::material* build_exoskeleton_material
(
	const phene::pigmentation& pigmentation,
	const phene::sculpturing& sculpturing
);
static void reskin_vertices
(
	std::uint8_t* vertex_data,
	std::size_t index_count,
	const gl::vertex_attribute& position_attribute,
	const gl::vertex_attribute& normal_attribute,
	const gl::vertex_attribute& tangent_attribute,
	const gl::vertex_attribute& bone_index_attribute,
	const std::unordered_set<std::uint8_t>& old_bone_indices,
	std::uint8_t new_bone_index,
	const math::transform<float>& transform
);
static geom::aabb<float> calculate_bounds(std::uint8_t* vertex_data, std::size_t index_count, const gl::vertex_attribute& position_attribute);
static render::model* build_model
(
	render::material* material,
	const render::model* antennae,
	const render::model* eyes,
	const render::model* forewings,
	const render::model* gaster,
	const render::model* head,
	const render::model* hindwings,
	const render::model* legs,
	const render::model* mandibles,
	const render::model* mesosoma,
	const render::model* lateral_ocelli,
	const render::model* median_ocellus,
	const render::model* sting,
	const render::model* waist
);

render::material* build_exoskeleton_material
(
	const phene::pigmentation& pigmentation,
	const phene::sculpturing& sculpturing
)
{
	// Allocate copy of pigmentation material
	render::material* exoskeleton_material = new render::material(*pigmentation.material);
	
	// Adjust roughness parameter
	if (render::material_property_base* property = exoskeleton_material->get_property("roughness"); property != nullptr)
	{
		static_cast<render::material_property<float>*>(property)->set_value(sculpturing.roughness);
	}
	else
	{
		exoskeleton_material->add_property<float>("roughness")->set_value(sculpturing.roughness);
	}
	
	// Adjust normal map parameter
	if (render::material_property_base* property = exoskeleton_material->get_property("normal_map"); property != nullptr)
	{
		static_cast<render::material_property<const gl::texture_2d*>*>(property)->set_value(sculpturing.normal_map);
	}
	else
	{
		exoskeleton_material->add_property<const gl::texture_2d*>("normal_map")->set_value(sculpturing.normal_map);
	}
	
	return exoskeleton_material;
}

render::model* morphogenesis(const phenome& phenome)
{
	// Build exoskeleton material
	render::material* exoskeleton_material = build_exoskeleton_material(*phenome.pigmentation, *phenome.sculpturing);
	
	// Determine presence of optional parts
	bool eyes_present = phenome.eyes->present;
	bool lateral_ocelli_present = phenome.ocelli->lateral_ocelli_present;
	bool median_ocellus_present = phenome.ocelli->median_ocellus_present;
	bool petiole_present = phenome.waist->petiole_present;
	bool postpetiole_present = phenome.waist->postpetiole_present;
	bool sting_present = phenome.sting->present;
	bool wings_present = phenome.wings->present;
	
	// Get body part models
	render::model* antennae_model = phenome.antennae->model;
	render::model* eyes_model = phenome.eyes->model;
	render::model* forewings_model = phenome.wings->forewings_model;
	render::model* gaster_model = phenome.gaster->model;
	render::model* head_model = phenome.head->model;
	render::model* hindwings_model = phenome.wings->hindwings_model;
	render::model* lateral_ocelli_model = phenome.ocelli->lateral_ocelli_model;
	render::model* legs_model = phenome.legs->model;
	render::model* mandibles_model = phenome.mandibles->model;
	render::model* median_ocellus_model = phenome.ocelli->median_ocellus_model;
	render::model* mesosoma_model = phenome.mesosoma->model;
	render::model* sting_model = phenome.sting->model;
	render::model* waist_model = phenome.waist->model;
	
	// Get body part vertex buffers
	const gl::vertex_buffer* antennae_vbo = antennae_model->get_vertex_buffer();
	const gl::vertex_buffer* eyes_vbo = (eyes_present) ? eyes_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* forewings_vbo = (wings_present) ? forewings_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* gaster_vbo = gaster_model->get_vertex_buffer();
	const gl::vertex_buffer* head_vbo = head_model->get_vertex_buffer();
	const gl::vertex_buffer* hindwings_vbo = (wings_present) ? hindwings_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* lateral_ocelli_vbo = (lateral_ocelli_model) ? lateral_ocelli_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* legs_vbo = legs_model->get_vertex_buffer();
	const gl::vertex_buffer* mandibles_vbo = mandibles_model->get_vertex_buffer();
	const gl::vertex_buffer* median_ocellus_vbo = (median_ocellus_model) ? median_ocellus_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* mesosoma_vbo = mesosoma_model->get_vertex_buffer();
	const gl::vertex_buffer* sting_vbo = (sting_present) ? sting_model->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* waist_vbo = waist_model->get_vertex_buffer();
	
	// Determine combined size of vertex buffers and save offsets
	std::size_t vertex_buffer_size = 0;
	std::size_t mesosoma_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += mesosoma_vbo->get_size();
	std::size_t legs_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += legs_vbo->get_size();
	std::size_t head_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += head_vbo->get_size();
	std::size_t mandibles_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += mandibles_vbo->get_size();
	std::size_t antennae_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += antennae_vbo->get_size();
	std::size_t waist_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += waist_vbo->get_size();
	std::size_t gaster_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += gaster_vbo->get_size();
	std::size_t sting_vbo_offset = vertex_buffer_size;
	if (sting_present)
		vertex_buffer_size += sting_vbo->get_size();
	std::size_t eyes_vbo_offset = vertex_buffer_size;
	if (eyes_present)
		vertex_buffer_size += eyes_vbo->get_size();
	std::size_t lateral_ocelli_vbo_offset = vertex_buffer_size;
	if (lateral_ocelli_present)
		vertex_buffer_size += lateral_ocelli_vbo->get_size();
	std::size_t median_ocellus_vbo_offset = vertex_buffer_size;
	if (median_ocellus_present)
		vertex_buffer_size += median_ocellus_vbo->get_size();
	std::size_t forewings_vbo_offset = vertex_buffer_size;
	if (wings_present)
		vertex_buffer_size += forewings_vbo->get_size();
	std::size_t hindwings_vbo_offset = vertex_buffer_size;
	if (wings_present)
		vertex_buffer_size += hindwings_vbo->get_size();
	
	// Allocate combined vertex buffer data
	std::uint8_t* vertex_buffer_data = new std::uint8_t[vertex_buffer_size];
	
	// Read body part vertex buffer data into combined vertex buffer data
	mesosoma_vbo->read(0, mesosoma_vbo->get_size(), vertex_buffer_data + mesosoma_vbo_offset);
	legs_vbo->read(0, legs_vbo->get_size(), vertex_buffer_data + legs_vbo_offset);
	head_vbo->read(0, head_vbo->get_size(), vertex_buffer_data + head_vbo_offset);
	mandibles_vbo->read(0, mandibles_vbo->get_size(), vertex_buffer_data + mandibles_vbo_offset);
	antennae_vbo->read(0, antennae_vbo->get_size(), vertex_buffer_data + antennae_vbo_offset);
	waist_vbo->read(0, waist_vbo->get_size(), vertex_buffer_data + waist_vbo_offset);
	gaster_vbo->read(0, gaster_vbo->get_size(), vertex_buffer_data + gaster_vbo_offset);
	if (sting_present)
		sting_vbo->read(0, sting_vbo->get_size(), vertex_buffer_data + sting_vbo_offset);
	if (eyes_present)
		eyes_vbo->read(0, eyes_vbo->get_size(), vertex_buffer_data + eyes_vbo_offset);
	if (lateral_ocelli_present)
		lateral_ocelli_vbo->read(0, lateral_ocelli_vbo->get_size(), vertex_buffer_data + lateral_ocelli_vbo_offset);
	if (median_ocellus_present)
		median_ocellus_vbo->read(0, median_ocellus_vbo->get_size(), vertex_buffer_data + median_ocellus_vbo_offset);
	if (wings_present)
	{
		forewings_vbo->read(0, forewings_vbo->get_size(), vertex_buffer_data + forewings_vbo_offset);
		hindwings_vbo->read(0, hindwings_vbo->get_size(), vertex_buffer_data + hindwings_vbo_offset);
	}
	
	// Allocate model
	render::model* model = new render::model();
	
	// Setup model VAO
	gl::vertex_array* model_vao = model->get_vertex_array();
	for (auto [location, attribute]: mesosoma_model->get_vertex_array()->get_attributes())
	{
		attribute.buffer = model->get_vertex_buffer();
		model_vao->bind(location, attribute);
	}
	
	// Get vertex attributes
	const gl::vertex_attribute* position_attribute = nullptr;
	const gl::vertex_attribute* normal_attribute = nullptr;
	const gl::vertex_attribute* tangent_attribute = nullptr;
	const gl::vertex_attribute* bone_index_attribute = nullptr;
	const auto& vertex_attribute_map = model_vao->get_attributes();
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::position); it != vertex_attribute_map.end())
		position_attribute = &it->second;
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::normal); it != vertex_attribute_map.end())
		normal_attribute = &it->second;
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::tangent); it != vertex_attribute_map.end())
		tangent_attribute = &it->second;
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::bone_index); it != vertex_attribute_map.end())
		bone_index_attribute = &it->second;
	
	// Get body part skeletons
	const ::skeleton& mesosoma_skeleton = mesosoma_model->get_skeleton();
	const ::skeleton& legs_skeleton = legs_model->get_skeleton();
	const ::skeleton& head_skeleton = head_model->get_skeleton();
	const ::skeleton& mandibles_skeleton = mandibles_model->get_skeleton();
	const ::skeleton& antennae_skeleton = antennae_model->get_skeleton();
	const ::skeleton& waist_skeleton = waist_model->get_skeleton();
	const ::skeleton& gaster_skeleton = gaster_model->get_skeleton();
	const ::skeleton* sting_skeleton = (sting_present) ? &sting_model->get_skeleton() : nullptr;
	const ::skeleton* eyes_skeleton = (eyes_present) ? &eyes_model->get_skeleton() : nullptr;
	const ::skeleton* lateral_ocelli_skeleton = (lateral_ocelli_present) ? &lateral_ocelli_model->get_skeleton() : nullptr;
	const ::skeleton* median_ocellus_skeleton = (median_ocellus_present) ? &median_ocellus_model->get_skeleton() : nullptr;
	
	// Allocate skeleton bones
	::skeleton& skeleton = model->get_skeleton();
	std::size_t bone_count = 33;
	if (petiole_present)
		bone_count += 1;
	if (postpetiole_present)
		bone_count += 1;
	if (sting_present)
		bone_count += 1;
	if (wings_present)
		bone_count += 4;
	
	// Assign bone indices
	std::uint8_t bone_index = 0;
	std::uint8_t mesosoma_bone_index = bone_index++;
	std::uint8_t procoxa_l_bone_index = bone_index++;
	std::uint8_t procoxa_r_bone_index = bone_index++;
	std::uint8_t profemur_l_bone_index = bone_index++;
	std::uint8_t profemur_r_bone_index = bone_index++;
	std::uint8_t protibia_l_bone_index = bone_index++;
	std::uint8_t protibia_r_bone_index = bone_index++;
	std::uint8_t protarsus_l_bone_index = bone_index++;
	std::uint8_t protarsus_r_bone_index = bone_index++;
	std::uint8_t mesocoxa_l_bone_index = bone_index++;
	std::uint8_t mesocoxa_r_bone_index = bone_index++;
	std::uint8_t mesofemur_l_bone_index = bone_index++;
	std::uint8_t mesofemur_r_bone_index = bone_index++;
	std::uint8_t mesotibia_l_bone_index = bone_index++;
	std::uint8_t mesotibia_r_bone_index = bone_index++;
	std::uint8_t mesotarsus_l_bone_index = bone_index++;
	std::uint8_t mesotarsus_r_bone_index = bone_index++;
	std::uint8_t metacoxa_l_bone_index = bone_index++;
	std::uint8_t metacoxa_r_bone_index = bone_index++;
	std::uint8_t metafemur_l_bone_index = bone_index++;
	std::uint8_t metafemur_r_bone_index = bone_index++;
	std::uint8_t metatibia_l_bone_index = bone_index++;
	std::uint8_t metatibia_r_bone_index = bone_index++;
	std::uint8_t metatarsus_l_bone_index = bone_index++;
	std::uint8_t metatarsus_r_bone_index = bone_index++;
	std::uint8_t head_bone_index = bone_index++;
	std::uint8_t mandible_l_bone_index = bone_index++;
	std::uint8_t mandible_r_bone_index = bone_index++;
	std::uint8_t antennomere1_l_bone_index = bone_index++;
	std::uint8_t antennomere1_r_bone_index = bone_index++;
	std::uint8_t antennomere2_l_bone_index = bone_index++;
	std::uint8_t antennomere2_r_bone_index = bone_index++;
	std::uint8_t petiole_bone_index =(petiole_present) ? bone_index++ : static_cast<std::uint8_t>(bone_count);
	std::uint8_t postpetiole_bone_index = (postpetiole_present) ? bone_index++ : static_cast<std::uint8_t>(bone_count);
	std::uint8_t gaster_bone_index = bone_index++;
	std::uint8_t sting_bone_index = (sting_present) ? bone_index++ : static_cast<std::uint8_t>(bone_count);
	
	// Construct bone identifiers
	::bone mesosoma_bone = make_bone(mesosoma_bone_index);
	::bone procoxa_l_bone = make_bone(procoxa_l_bone_index, mesosoma_bone_index);
	::bone procoxa_r_bone = make_bone(procoxa_r_bone_index, mesosoma_bone_index);
	::bone profemur_l_bone = make_bone(profemur_l_bone_index, procoxa_l_bone_index);
	::bone profemur_r_bone = make_bone(profemur_r_bone_index, procoxa_r_bone_index);
	::bone protibia_l_bone = make_bone(protibia_l_bone_index, profemur_l_bone_index);
	::bone protibia_r_bone = make_bone(protibia_r_bone_index, profemur_r_bone_index);
	::bone protarsus_l_bone = make_bone(protarsus_l_bone_index, protibia_l_bone_index);
	::bone protarsus_r_bone = make_bone(protarsus_r_bone_index, protibia_r_bone_index);
	::bone mesocoxa_l_bone = make_bone(mesocoxa_l_bone_index, mesosoma_bone_index);
	::bone mesocoxa_r_bone = make_bone(mesocoxa_r_bone_index, mesosoma_bone_index);
	::bone mesofemur_l_bone = make_bone(mesofemur_l_bone_index, mesocoxa_l_bone_index);
	::bone mesofemur_r_bone = make_bone(mesofemur_r_bone_index, mesocoxa_r_bone_index);
	::bone mesotibia_l_bone = make_bone(mesotibia_l_bone_index, mesofemur_l_bone_index);
	::bone mesotibia_r_bone = make_bone(mesotibia_r_bone_index, mesofemur_r_bone_index);
	::bone mesotarsus_l_bone = make_bone(mesotarsus_l_bone_index, mesotibia_l_bone_index);
	::bone mesotarsus_r_bone = make_bone(mesotarsus_r_bone_index, mesotibia_r_bone_index);
	::bone metacoxa_l_bone = make_bone(metacoxa_l_bone_index, mesosoma_bone_index);
	::bone metacoxa_r_bone = make_bone(metacoxa_r_bone_index, mesosoma_bone_index);
	::bone metafemur_l_bone = make_bone(metafemur_l_bone_index, metacoxa_l_bone_index);
	::bone metafemur_r_bone = make_bone(metafemur_r_bone_index, metacoxa_r_bone_index);
	::bone metatibia_l_bone = make_bone(metatibia_l_bone_index, metafemur_l_bone_index);
	::bone metatibia_r_bone = make_bone(metatibia_r_bone_index, metafemur_r_bone_index);
	::bone metatarsus_l_bone = make_bone(metatarsus_l_bone_index, metatibia_l_bone_index);
	::bone metatarsus_r_bone = make_bone(metatarsus_r_bone_index, metatibia_r_bone_index);
	::bone head_bone = make_bone(head_bone_index, mesosoma_bone_index);
	::bone mandible_l_bone = make_bone(mandible_l_bone_index, head_bone_index);
	::bone mandible_r_bone = make_bone(mandible_r_bone_index, head_bone_index);
	::bone antennomere1_l_bone = make_bone(antennomere1_l_bone_index, head_bone_index);
	::bone antennomere1_r_bone = make_bone(antennomere1_r_bone_index, head_bone_index);
	::bone antennomere2_l_bone = make_bone(antennomere2_l_bone_index, antennomere1_l_bone_index);
	::bone antennomere2_r_bone = make_bone(antennomere2_r_bone_index, antennomere1_r_bone_index);
	::bone petiole_bone = make_bone(petiole_bone_index, mesosoma_bone_index);
	::bone postpetiole_bone = make_bone(postpetiole_bone_index, petiole_bone_index);
	::bone gaster_bone = make_bone(gaster_bone_index, (postpetiole_present) ? postpetiole_bone_index : petiole_bone_index);
	::bone sting_bone = make_bone(sting_bone_index, gaster_bone_index);
	
	// Map bone names to bones
	skeleton.bone_map["mesosoma"] = mesosoma_bone;
	skeleton.bone_map["procoxa_l"] = procoxa_l_bone;
	skeleton.bone_map["procoxa_r"] = procoxa_r_bone;
	skeleton.bone_map["profemur_l"] = profemur_l_bone;
	skeleton.bone_map["profemur_r"] = profemur_r_bone;
	skeleton.bone_map["protibia_l"] = protibia_l_bone;
	skeleton.bone_map["protibia_r"] = protibia_r_bone;
	skeleton.bone_map["protarsus_l"] = protarsus_l_bone;
	skeleton.bone_map["protarsus_r"] = protarsus_r_bone;
	skeleton.bone_map["mesocoxa_l"] = mesocoxa_l_bone;
	skeleton.bone_map["mesocoxa_r"] = mesocoxa_r_bone;
	skeleton.bone_map["mesofemur_l"] = mesofemur_l_bone;
	skeleton.bone_map["mesofemur_r"] = mesofemur_r_bone;
	skeleton.bone_map["mesotibia_l"] = mesotibia_l_bone;
	skeleton.bone_map["mesotibia_r"] = mesotibia_r_bone;
	skeleton.bone_map["mesotarsus_l"] = mesotarsus_l_bone;
	skeleton.bone_map["mesotarsus_r"] = mesotarsus_r_bone;
	skeleton.bone_map["metacoxa_l"] = metacoxa_l_bone;
	skeleton.bone_map["metacoxa_r"] = metacoxa_r_bone;
	skeleton.bone_map["metafemur_l"] = metafemur_l_bone;
	skeleton.bone_map["metafemur_r"] = metafemur_r_bone;
	skeleton.bone_map["metatibia_l"] = metatibia_l_bone;
	skeleton.bone_map["metatibia_r"] = metatibia_r_bone;
	skeleton.bone_map["metatarsus_l"] = metatarsus_l_bone;
	skeleton.bone_map["metatarsus_r"] = metatarsus_r_bone;
	skeleton.bone_map["head"] = head_bone;
	skeleton.bone_map["mandible_l"] = mandible_l_bone;
	skeleton.bone_map["mandible_r"] = mandible_r_bone;
	skeleton.bone_map["antennomere1_l"] = antennomere1_l_bone;
	skeleton.bone_map["antennomere1_r"] = antennomere1_r_bone;
	skeleton.bone_map["antennomere2_l"] = antennomere2_l_bone;
	skeleton.bone_map["antennomere2_r"] = antennomere2_r_bone;
	if (petiole_present)
		skeleton.bone_map["petiole"] = petiole_bone;
	if (postpetiole_present)
		skeleton.bone_map["postpetiole"] = postpetiole_bone;
	skeleton.bone_map["gaster"] = gaster_bone;
	if (sting_present)
		skeleton.bone_map["sting"] = sting_bone;
	
	// Get reference to skeleton bind pose
	pose& bind_pose = skeleton.bind_pose;
	
	// Skeleton mesosoma pose
	if (auto it = mesosoma_skeleton.bone_map.find("mesosoma"); it != mesosoma_skeleton.bone_map.end())
		bind_pose[mesosoma_bone] = mesosoma_skeleton.bind_pose.at(it->second);
	
	// Skeleton forelegs pose
	if (auto it = legs_skeleton.bone_map.find("procoxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[procoxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("procoxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[procoxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("profemur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[profemur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("profemur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[profemur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("protibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[protibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("protibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[protibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[protarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[protarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton midlegs pose
	if (auto it = legs_skeleton.bone_map.find("mesocoxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[mesocoxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesocoxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[mesocoxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesofemur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[mesofemur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesofemur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[mesofemur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[mesotibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[mesotibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[mesotarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[mesotarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton hindlegs pose
	if (auto it = legs_skeleton.bone_map.find("metacoxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[metacoxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metacoxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[metacoxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metafemur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[metafemur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metafemur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[metafemur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[metatibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[metatibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[metatarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[metatarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton head pose
	bind_pose[head_bone] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("head")) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("head"));
	
	// Skeleton mandibles pose
	bind_pose[mandible_l_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_l")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_l"));
	bind_pose[mandible_r_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_r")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_r"));
	
	// Skeleton antennae pose
	bind_pose[antennomere1_l_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_l")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere1_l"));
	bind_pose[antennomere1_r_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_r")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere1_r"));
	bind_pose[antennomere2_l_bone] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere2_l"));
	bind_pose[antennomere2_r_bone] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere2_r"));
	
	// Skeleton waist pose
	if (petiole_present)
	{
		bind_pose[petiole_bone] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole")) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole"));
	}
	if (postpetiole_present)
	{
		bind_pose[postpetiole_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole"));
	}
	
	// Skeleton gaster pose
	if (postpetiole_present)
	{
		bind_pose[gaster_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	else if (petiole_present)
	{
		bind_pose[gaster_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	else
	{
		bind_pose[gaster_bone] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	
	// Skeleton sting pose
	if (sting_present)
	{
		bind_pose[sting_bone] = gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting")) * sting_skeleton->bind_pose.at(sting_skeleton->bone_map.at("sting"));
	}
	
	// Calculate the skeleton-space bind pose
	pose bind_pose_ss;
	::concatenate(bind_pose, bind_pose_ss);
	
	// Calculate inverse skeleton-space bind pose
	::inverse(bind_pose_ss, skeleton.inverse_bind_pose);
	
	// Get number of vertex indices for each body part
	std::size_t mesosoma_index_count = (*mesosoma_model->get_groups())[0]->get_index_count();
	std::size_t legs_index_count = (*legs_model->get_groups())[0]->get_index_count();
	std::size_t head_index_count = (*head_model->get_groups())[0]->get_index_count();
	std::size_t mandibles_index_count = (*mandibles_model->get_groups())[0]->get_index_count();
	std::size_t antennae_index_count = (*antennae_model->get_groups())[0]->get_index_count();
	std::size_t waist_index_count = (*waist_model->get_groups())[0]->get_index_count();
	std::size_t gaster_index_count = (*gaster_model->get_groups())[0]->get_index_count();
	std::size_t sting_index_count = (sting_present) ? (*sting_model->get_groups())[0]->get_index_count() : 0;
	std::size_t eyes_index_count = (eyes_present) ? (*eyes_model->get_groups())[0]->get_index_count() : 0;
	std::size_t lateral_ocelli_index_count = (lateral_ocelli_present) ? (*lateral_ocelli_model->get_groups())[0]->get_index_count() : 0;
	std::size_t median_ocellus_index_count = (median_ocellus_present) ? (*median_ocellus_model->get_groups())[0]->get_index_count() : 0;
	std::size_t forewings_index_count = (wings_present) ? (*forewings_model->get_groups())[0]->get_index_count() : 0;
	std::size_t hindwings_index_count = (wings_present) ? (*hindwings_model->get_groups())[0]->get_index_count() : 0;
	std::size_t exoskeleton_index_count =
		mesosoma_index_count
		+ legs_index_count
		+ head_index_count
		+ mandibles_index_count
		+ antennae_index_count
		+ waist_index_count
		+ gaster_index_count
		+ sting_index_count;
	
	// Calculate transform from legs space to body space
	const math::transform<float>& legs_to_body = math::transform<float>::identity;
	
	// Reskin leg bones
	std::unordered_set<std::uint8_t> old_procoxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("procoxa_l"); it != legs_skeleton.bone_map.end())
		old_procoxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_procoxa_l_indices, procoxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_profemur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("profemur_l"); it != legs_skeleton.bone_map.end())
		old_profemur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_profemur_l_indices, profemur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_protibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("protibia_l"); it != legs_skeleton.bone_map.end())
		old_protibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_protibia_l_indices, protibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_protarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("protarsus1_l"); it != legs_skeleton.bone_map.end())
		old_protarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus2_l"); it != legs_skeleton.bone_map.end())
		old_protarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus3_l"); it != legs_skeleton.bone_map.end())
		old_protarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus4_l"); it != legs_skeleton.bone_map.end())
		old_protarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus5_l"); it != legs_skeleton.bone_map.end())
		old_protarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_protarsus_l_indices, protarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_procoxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("procoxa_r"); it != legs_skeleton.bone_map.end())
		old_procoxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_procoxa_r_indices, procoxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_profemur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("profemur_r"); it != legs_skeleton.bone_map.end())
		old_profemur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_profemur_r_indices, profemur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_protibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("protibia_r"); it != legs_skeleton.bone_map.end())
		old_protibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_protibia_r_indices, protibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_protarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("protarsus1_r"); it != legs_skeleton.bone_map.end())
		old_protarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus2_r"); it != legs_skeleton.bone_map.end())
		old_protarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus3_r"); it != legs_skeleton.bone_map.end())
		old_protarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus4_r"); it != legs_skeleton.bone_map.end())
		old_protarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("protarsus5_r"); it != legs_skeleton.bone_map.end())
		old_protarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_protarsus_r_indices, protarsus_r_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_mesocoxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("mesocoxa_l"); it != legs_skeleton.bone_map.end())
		old_mesocoxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesocoxa_l_indices, mesocoxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesofemur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("mesofemur_l"); it != legs_skeleton.bone_map.end())
		old_mesofemur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesofemur_l_indices, mesofemur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesotibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("mesotibia_l"); it != legs_skeleton.bone_map.end())
		old_mesotibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesotibia_l_indices, mesotibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesotarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("mesotarsus1_l"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus2_l"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus3_l"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus4_l"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus5_l"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesotarsus_l_indices, mesotarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_mesocoxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("mesocoxa_r"); it != legs_skeleton.bone_map.end())
		old_mesocoxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesocoxa_r_indices, mesocoxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesofemur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("mesofemur_r"); it != legs_skeleton.bone_map.end())
		old_mesofemur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesofemur_r_indices, mesofemur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesotibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("mesotibia_r"); it != legs_skeleton.bone_map.end())
		old_mesotibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesotibia_r_indices, mesotibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_mesotarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("mesotarsus1_r"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus2_r"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus3_r"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus4_r"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("mesotarsus5_r"); it != legs_skeleton.bone_map.end())
		old_mesotarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_mesotarsus_r_indices, mesotarsus_r_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_metacoxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("metacoxa_l"); it != legs_skeleton.bone_map.end())
		old_metacoxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metacoxa_l_indices, metacoxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metafemur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("metafemur_l"); it != legs_skeleton.bone_map.end())
		old_metafemur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metafemur_l_indices, metafemur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metatibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("metatibia_l"); it != legs_skeleton.bone_map.end())
		old_metatibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metatibia_l_indices, metatibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metatarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("metatarsus1_l"); it != legs_skeleton.bone_map.end())
		old_metatarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus2_l"); it != legs_skeleton.bone_map.end())
		old_metatarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus3_l"); it != legs_skeleton.bone_map.end())
		old_metatarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus4_l"); it != legs_skeleton.bone_map.end())
		old_metatarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus5_l"); it != legs_skeleton.bone_map.end())
		old_metatarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metatarsus_l_indices, metatarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_metacoxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("metacoxa_r"); it != legs_skeleton.bone_map.end())
		old_metacoxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metacoxa_r_indices, metacoxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metafemur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("metafemur_r"); it != legs_skeleton.bone_map.end())
		old_metafemur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metafemur_r_indices, metafemur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metatibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("metatibia_r"); it != legs_skeleton.bone_map.end())
		old_metatibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metatibia_r_indices, metatibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_metatarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("metatarsus1_r"); it != legs_skeleton.bone_map.end())
		old_metatarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus2_r"); it != legs_skeleton.bone_map.end())
		old_metatarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus3_r"); it != legs_skeleton.bone_map.end())
		old_metatarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus4_r"); it != legs_skeleton.bone_map.end())
		old_metatarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("metatarsus5_r"); it != legs_skeleton.bone_map.end())
		old_metatarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_metatarsus_r_indices, metatarsus_r_bone_index, legs_to_body);
	
	// Calculate transform from head space to body space
	math::transform<float> head_to_body = bind_pose_ss.at(mesosoma_bone) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("head"));
	
	// Reskin head bone
	std::unordered_set<std::uint8_t> old_head_bone_indices;
	if (auto it = head_skeleton.bone_map.find("head"); it != head_skeleton.bone_map.end())
		old_head_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + head_vbo_offset, head_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_head_bone_indices, head_bone_index, head_to_body);
	
	// Calculate transforms from mandibles space to body space
	math::transform<float> mandible_l_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_l"));
	math::transform<float> mandible_r_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_r"));
	
	// Reskin mandible bones
	std::unordered_set<std::uint8_t> old_head_mandible_l_bone_indices;
	if (auto it = mandibles_skeleton.bone_map.find("mandible_l"); it != mandibles_skeleton.bone_map.end())
		old_head_mandible_l_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + mandibles_vbo_offset, mandibles_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_head_mandible_l_bone_indices, mandible_l_bone_index, mandible_l_to_body);
	std::unordered_set<std::uint8_t> old_head_mandible_r_bone_indices;
	if (auto it = mandibles_skeleton.bone_map.find("mandible_r"); it != mandibles_skeleton.bone_map.end())
		old_head_mandible_r_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + mandibles_vbo_offset, mandibles_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_head_mandible_r_bone_indices, mandible_r_bone_index, mandible_r_to_body);
	
	// Calculate transforms from antennae space to body space
	math::transform<float> antenna_l_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_l"));
	math::transform<float> antenna_r_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_r"));
	
	// Reskin antennomere1 bones
	std::unordered_set<std::uint8_t> old_antennomere1_l_indices;
	if (auto it = antennae_skeleton.bone_map.find("antennomere1_l"); it != antennae_skeleton.bone_map.end())
		old_antennomere1_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_antennomere1_l_indices, antennomere1_l_bone_index, antenna_l_to_body);
	std::unordered_set<std::uint8_t> old_antennomere1_r_indices;
	if (auto it = antennae_skeleton.bone_map.find("antennomere1_r"); it != antennae_skeleton.bone_map.end())
		old_antennomere1_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_antennomere1_r_indices, antennomere1_r_bone_index, antenna_r_to_body);
	
	// Reskin antennomere2+ bones
	const std::vector<std::string> antennomere_bone_names =
	{
		"antennomere2",
		"antennomere3",
		"antennomere4",
		"antennomere5",
		"antennomere6",
		"antennomere7",
		"antennomere8",
		"antennomere9",
		"antennomere10",
		"antennomere11",
		"antennomere12",
		"antennomere13"
	};
	std::unordered_set<std::uint8_t> old_antennomere_l_indices;
	for (const std::string& bone_name: antennomere_bone_names)
		if (auto it = antennae_skeleton.bone_map.find(bone_name + "_l"); it != antennae_skeleton.bone_map.end())
			old_antennomere_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_antennomere_l_indices, antennomere2_l_bone_index, antenna_l_to_body);
	std::unordered_set<std::uint8_t> old_antennomere_r_indices;
	for (const std::string& bone_name: antennomere_bone_names)
		if (auto it = antennae_skeleton.bone_map.find(bone_name + "_r"); it != antennae_skeleton.bone_map.end())
			old_antennomere_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_antennomere_r_indices, antennomere2_r_bone_index, antenna_r_to_body);
	
	// Calculate transform from waist space to body space
	math::transform<float> waist_to_body = bind_pose_ss.at(mesosoma_bone) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole"));
	
	// Reskin waist bones
	std::unordered_set<std::uint8_t> old_petiole_bone_indices;
	if (auto it = waist_skeleton.bone_map.find("petiole"); it != waist_skeleton.bone_map.end())
		old_petiole_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + waist_vbo_offset, waist_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_petiole_bone_indices, petiole_bone_index, waist_to_body);
	if (postpetiole_present)
	{
		std::unordered_set<std::uint8_t> old_postpetiole_bone_indices;
		if (auto it = waist_skeleton.bone_map.find("postpetiole"); it != waist_skeleton.bone_map.end())
			old_postpetiole_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + waist_vbo_offset, waist_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_postpetiole_bone_indices, postpetiole_bone_index, waist_to_body);
	}
	
	// Calculate transform from gaster space to body space
	math::transform<float> gaster_to_body = bind_pose_ss.at(bone_parent_index(gaster_bone)) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("gaster"));
	
	// Reskin gaster bones
	std::unordered_set<std::uint8_t> old_gaster_bone_indices;
	if (auto it = gaster_skeleton.bone_map.find("gaster"); it != gaster_skeleton.bone_map.end())
		old_gaster_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + gaster_vbo_offset, gaster_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_gaster_bone_indices, gaster_bone_index, gaster_to_body);
	
	if (sting_present)
	{
		// Calculate transform from sting space to body space
		math::transform<float> sting_to_body = gaster_to_body * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting"));

		// Reskin sting bones
		std::unordered_set<std::uint8_t> old_sting_bone_indices;
		if (auto it = sting_skeleton->bone_map.find("sting"); it != sting_skeleton->bone_map.end())
			old_sting_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + sting_vbo_offset, sting_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_sting_bone_indices, sting_bone_index, sting_to_body);
	}
	
	if (eyes_present)
	{
		// Calculate transforms from eyes space to body space
		math::transform<float> eye_l_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("eye_l"));
		math::transform<float> eye_r_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("eye_r"));
		
		// Reskin eye bones
		std::unordered_set<std::uint8_t> old_eye_l_bone_indices;
		if (auto it = eyes_skeleton->bone_map.find("eye_l"); it != eyes_skeleton->bone_map.end())
			old_eye_l_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + eyes_vbo_offset, eyes_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_eye_l_bone_indices, head_bone_index, eye_l_to_body);
		std::unordered_set<std::uint8_t> old_eye_r_bone_indices;
		if (auto it = eyes_skeleton->bone_map.find("eye_r"); it != eyes_skeleton->bone_map.end())
			old_eye_r_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + eyes_vbo_offset, eyes_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_eye_r_bone_indices, head_bone_index, eye_r_to_body);
	}
	
	if (lateral_ocelli_present)
	{
		// Calculate transforms from lateral ocelli space to body space
		math::transform<float> ocellus_l_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_l"));
		math::transform<float> ocellus_r_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_r"));
		
		// Reskin lateral ocelli bones
		std::unordered_set<std::uint8_t> old_ocellus_l_bone_indices;
		if (auto it = lateral_ocelli_skeleton->bone_map.find("ocellus_l"); it != lateral_ocelli_skeleton->bone_map.end())
			old_ocellus_l_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + lateral_ocelli_vbo_offset, lateral_ocelli_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_ocellus_l_bone_indices, head_bone_index, ocellus_l_to_body);
		std::unordered_set<std::uint8_t> old_ocellus_r_bone_indices;
		if (auto it = lateral_ocelli_skeleton->bone_map.find("ocellus_r"); it != lateral_ocelli_skeleton->bone_map.end())
			old_ocellus_r_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + lateral_ocelli_vbo_offset, lateral_ocelli_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_ocellus_r_bone_indices, head_bone_index, ocellus_r_to_body);
	}
	
	if (median_ocellus_present)
	{
		// Calculate transforms from lateral ocelli space to body space
		math::transform<float> ocellus_m_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_m"));
		
		// Reskin lateral ocelli bones
		std::unordered_set<std::uint8_t> old_ocellus_m_bone_indices;
		if (auto it = median_ocellus_skeleton->bone_map.find("ocellus_m"); it != median_ocellus_skeleton->bone_map.end())
			old_ocellus_m_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + median_ocellus_vbo_offset, median_ocellus_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_ocellus_m_bone_indices, head_bone_index, ocellus_m_to_body);
	}
	
	// Upload vertex buffer data to model VBO
	model->get_vertex_buffer()->repurpose(gl::buffer_usage::static_draw, vertex_buffer_size, vertex_buffer_data);
	
	// Construct exoskeleton model group
	render::model_group* exoskeleton_group = model->add_group("exoskeleton");
	exoskeleton_group->set_material(exoskeleton_material);
	exoskeleton_group->set_drawing_mode(gl::drawing_mode::triangles);
	exoskeleton_group->set_start_index(0);
	exoskeleton_group->set_index_count(exoskeleton_index_count);
	
	std::size_t index_offset = exoskeleton_index_count;
	if (eyes_present)
	{
		// Construct eyes model group
		render::model_group* eyes_group = model->add_group("eyes");
		eyes_group->set_material((*eyes_model->get_groups())[0]->get_material());
		eyes_group->set_drawing_mode(gl::drawing_mode::triangles);
		eyes_group->set_start_index(index_offset);
		eyes_group->set_index_count(eyes_index_count);
		index_offset += eyes_index_count;
	}
	
	if (lateral_ocelli_present || median_ocellus_present)
	{
		// Construct ocelli model group
		render::model_group* ocelli_group = model->add_group("ocelli");
		ocelli_group->set_drawing_mode(gl::drawing_mode::triangles);
		ocelli_group->set_start_index(index_offset);
		
		std::size_t index_count = 0;
		if (lateral_ocelli_present)
		{
			index_count += lateral_ocelli_index_count;
			index_offset += lateral_ocelli_index_count;
			ocelli_group->set_material((*lateral_ocelli_model->get_groups())[0]->get_material());
		}
		if (median_ocellus_present)
		{
			index_count += median_ocellus_index_count;
			index_offset += median_ocellus_index_count;
			if (!lateral_ocelli_present)
				ocelli_group->set_material((*median_ocellus_model->get_groups())[0]->get_material());
		}
		
		ocelli_group->set_index_count(index_count);
	}
	
	if (wings_present)
	{
		// Construct forewings model group
		render::model_group* forewings_group = model->add_group("forewings");
		forewings_group->set_material((*forewings_model->get_groups())[0]->get_material());
		forewings_group->set_drawing_mode(gl::drawing_mode::triangles);
		forewings_group->set_start_index(index_offset);
		forewings_group->set_index_count(forewings_index_count);
		index_offset += forewings_index_count;
		
		// Construct hindwings model group
		render::model_group* hindwings_group = model->add_group("hindwings");
		hindwings_group->set_material((*hindwings_model->get_groups())[0]->get_material());
		hindwings_group->set_drawing_mode(gl::drawing_mode::triangles);
		hindwings_group->set_start_index(index_offset);
		hindwings_group->set_index_count(hindwings_index_count);
		index_offset += hindwings_index_count;
	}
	
	// Calculate model bounding box
	geom::aabb<float> bounds = calculate_bounds(vertex_buffer_data, index_offset, *position_attribute);
	model->set_bounds(bounds);
	
	// Free vertex buffer data
	delete[] vertex_buffer_data;
	
	return model;
}

void reskin_vertices
(
	std::uint8_t* vertex_data,
	std::size_t index_count,
	const gl::vertex_attribute& position_attribute,
	const gl::vertex_attribute& normal_attribute,
	const gl::vertex_attribute& tangent_attribute,
	const gl::vertex_attribute& bone_index_attribute,
	const std::unordered_set<std::uint8_t>& old_bone_indices,
	std::uint8_t new_bone_index,
	const math::transform<float>& transform
)
{
	std::uint8_t* position_data = vertex_data + position_attribute.offset;
	std::uint8_t* normal_data = vertex_data + normal_attribute.offset;
	std::uint8_t* tangent_data = vertex_data + tangent_attribute.offset;
	std::uint8_t* bone_index_data = vertex_data + bone_index_attribute.offset;
	
	for (std::size_t i = 0; i < index_count; ++i)
	{
		// Get bone index of current vertex
		float* bone_index = reinterpret_cast<float*>(bone_index_data + bone_index_attribute.stride * i);
		
		// Skip irrelevant bones
		if (!old_bone_indices.count(static_cast<std::uint8_t>(*bone_index + 0.5f)))
			continue;
		
		// Get vertex position
		float* x = reinterpret_cast<float*>(position_data + position_attribute.stride * i);
		float* y = x + 1;
		float* z = y + 1;
		
		// Get vertex normal
		float* nx = reinterpret_cast<float*>(normal_data + normal_attribute.stride * i);
		float* ny = nx + 1;
		float* nz = ny + 1;
		
		// Get vertex tangent
		float* tx = reinterpret_cast<float*>(tangent_data + tangent_attribute.stride * i);
		float* ty = tx + 1;
		float* tz = ty + 1;
		//float* bts = tz + 1;
		
		// Transform vertex attributes
		float3 position = transform * float3{*x, *y, *z};
		float3 normal = math::normalize(transform.rotation * float3{*nx, *ny, *nz});
		float3 tangent = transform.rotation * float3{*tx, *ty, *tz};
		
		// Update vertex data
		*x = position.x();
		*y = position.y();
		*z = position.z();
		*nx = normal.x();
		*ny = normal.y();
		*nz = normal.z();
		*tx = tangent.x();
		*ty = tangent.y();
		*tz = tangent.z();
		//*bts = ...
		*bone_index = static_cast<float>(new_bone_index);
	}
}

geom::aabb<float> calculate_bounds(std::uint8_t* vertex_data, std::size_t index_count, const gl::vertex_attribute& position_attribute)
{
	std::uint8_t* position_data = vertex_data + position_attribute.offset;
	
	geom::aabb<float> bounds;
	bounds.min_point.x() = std::numeric_limits<float>::infinity();
	bounds.min_point.y() = std::numeric_limits<float>::infinity();
	bounds.min_point.z() = std::numeric_limits<float>::infinity();
	bounds.max_point.x() = -std::numeric_limits<float>::infinity();
	bounds.max_point.y() = -std::numeric_limits<float>::infinity();
	bounds.max_point.z() = -std::numeric_limits<float>::infinity();
	
	for (std::size_t i = 0; i < index_count; ++i)
	{
		// Get vertex position
		float* x = reinterpret_cast<float*>(position_data + position_attribute.stride * i);
		float* y = x + 1;
		float* z = y + 1;
		
		bounds.min_point.x() = std::min<float>(*x, bounds.min_point.x());
		bounds.min_point.y() = std::min<float>(*y, bounds.min_point.y());
		bounds.min_point.z() = std::min<float>(*z, bounds.min_point.z());
		bounds.max_point.x() = std::max<float>(*x, bounds.max_point.x());
		bounds.max_point.y() = std::max<float>(*y, bounds.max_point.y());
		bounds.max_point.z() = std::max<float>(*z, bounds.max_point.z());
	}
	
	return bounds;
}

} // namespace ant
} // namespace game
