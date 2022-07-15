/*
 * Copyright (C) 2021  Christopher J. Howard
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
#include <unordered_set>
#include <iostream>

namespace game {
namespace ant {

static render::model* generate_queen(const ant::breed& breed);
static render::model* generate_worker(const ant::breed& breed);
static render::model* generate_soldier(const ant::breed& breed);
static render::model* generate_male(const ant::breed& breed);
static render::material* build_exoskeleton_material
(
	const ant::trait::pigmentation& pigmentation,
	const ant::trait::sculpturing& sculpturing
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

render::model* morphogenesis(const ant::breed& breed, ant::caste caste)
{
	switch (caste)
	{
		case ant::caste::queen:
			return generate_queen(breed);
		case ant::caste::worker:
			return generate_worker(breed);
		case ant::caste::soldier:
			return generate_soldier(breed);
		case ant::caste::male:
			return generate_male(breed);
	}
	
	return nullptr;
}

render::model* generate_queen(const ant::breed& breed)
{
	return nullptr;
}

render::model* generate_worker(const ant::breed& breed)
{
	// Build exoskeleton material
	render::material* exoskeleton_material = build_exoskeleton_material(*breed.pigmentation, *breed.sculpturing);
	
	// Get worker body part models
	render::model* antennae_model = breed.antennae->model;
	render::model* eyes_model = breed.eyes->model;
	render::model* gaster_model = breed.gaster->model;
	render::model* head_model = breed.head->model;
	render::model* legs_model = breed.legs->model;
	render::model* mandibles_model = breed.mandibles->model;
	render::model* mesosoma_model = breed.mesosoma->model;
	render::model* sting_model = breed.sting->model;
	render::model* waist_model = breed.waist->model;
	//render::model* lateral_ocelli_model = breed.ocelli->lateral_ocelli_model;
	//render::model* median_ocellus_model = breed.ocelli->median_ocellus_model;
	
	// Build worker model
	render::model* model = build_model
	(
		exoskeleton_material,
		antennae_model,
		eyes_model,
		nullptr, // forewings
		gaster_model,
		head_model,
		nullptr, // hindwings
		legs_model,
		mandibles_model,
		mesosoma_model,
		nullptr, // lateral ocelli
		nullptr, // median ocellus
		sting_model,
		waist_model
	);
	
	return model;
}


render::model* generate_soldier(const ant::breed& breed)
{
	return nullptr;
}

render::model* generate_male(const ant::breed& breed)
{
	return nullptr;
}

render::material* build_exoskeleton_material
(
	const ant::trait::pigmentation& pigmentation,
	const ant::trait::sculpturing& sculpturing
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

render::model* build_model
(
	render::material* exoskeleton_material,
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
)
{
	// Get vertex buffers of required body parts
	const gl::vertex_buffer* mesosoma_vbo = mesosoma->get_vertex_buffer();
	const gl::vertex_buffer* legs_vbo = legs->get_vertex_buffer();
	const gl::vertex_buffer* head_vbo = head->get_vertex_buffer();
	const gl::vertex_buffer* mandibles_vbo = mandibles->get_vertex_buffer();
	const gl::vertex_buffer* antennae_vbo = antennae->get_vertex_buffer();
	const gl::vertex_buffer* waist_vbo = waist->get_vertex_buffer();
	const gl::vertex_buffer* gaster_vbo = gaster->get_vertex_buffer();
	
	// Get vertex buffers of optional body parts
	const gl::vertex_buffer* sting_vbo = (sting) ? sting->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* eyes_vbo = (eyes) ? eyes->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* lateral_ocelli_vbo = (lateral_ocelli) ? lateral_ocelli->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* median_ocellus_vbo = (median_ocellus) ? median_ocellus->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* forewings_vbo = (forewings) ? forewings->get_vertex_buffer() : nullptr;
	const gl::vertex_buffer* hindwings_vbo = (hindwings) ? hindwings->get_vertex_buffer() : nullptr;
	
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
	if (sting)
		vertex_buffer_size += sting_vbo->get_size();
	std::size_t eyes_vbo_offset = vertex_buffer_size;
	if (eyes)
		vertex_buffer_size += eyes_vbo->get_size();
	std::size_t lateral_ocelli_vbo_offset = vertex_buffer_size;
	if (lateral_ocelli)
		vertex_buffer_size += lateral_ocelli_vbo->get_size();
	std::size_t median_ocellus_vbo_offset = vertex_buffer_size;
	if (median_ocellus)
		vertex_buffer_size += median_ocellus_vbo->get_size();
	std::size_t forewings_vbo_offset = vertex_buffer_size;
	if (forewings)
		vertex_buffer_size += forewings_vbo->get_size();
	std::size_t hindwings_vbo_offset = vertex_buffer_size;
	if (hindwings)
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
	if (sting)
		sting_vbo->read(0, sting_vbo->get_size(), vertex_buffer_data + sting_vbo_offset);
	if (eyes)
		eyes_vbo->read(0, eyes_vbo->get_size(), vertex_buffer_data + eyes_vbo_offset);
	if (lateral_ocelli)
		lateral_ocelli_vbo->read(0, lateral_ocelli_vbo->get_size(), vertex_buffer_data + lateral_ocelli_vbo_offset);
	if (median_ocellus)
		median_ocellus_vbo->read(0, median_ocellus_vbo->get_size(), vertex_buffer_data + median_ocellus_vbo_offset);
	if (forewings)
		forewings_vbo->read(0, forewings_vbo->get_size(), vertex_buffer_data + forewings_vbo_offset);
	if (hindwings)
		hindwings_vbo->read(0, hindwings_vbo->get_size(), vertex_buffer_data + hindwings_vbo_offset);
	
	// Allocate model
	render::model* model = new render::model();
	
	// Setup model VAO
	gl::vertex_array* model_vao = model->get_vertex_array();
	for (auto [location, attribute]: mesosoma->get_vertex_array()->get_attributes())
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
	const ::skeleton& mesosoma_skeleton = mesosoma->get_skeleton();
	const ::skeleton& legs_skeleton = legs->get_skeleton();
	const ::skeleton& head_skeleton = head->get_skeleton();
	const ::skeleton& mandibles_skeleton = mandibles->get_skeleton();
	const ::skeleton& antennae_skeleton = antennae->get_skeleton();
	const ::skeleton& waist_skeleton = waist->get_skeleton();
	const ::skeleton& gaster_skeleton = gaster->get_skeleton();
	const ::skeleton* sting_skeleton = (sting) ? &sting->get_skeleton() : nullptr;
	const ::skeleton* eyes_skeleton = (eyes) ? &eyes->get_skeleton() : nullptr;
	const ::skeleton* lateral_ocelli_skeleton = (lateral_ocelli) ? &lateral_ocelli->get_skeleton() : nullptr;
	const ::skeleton* median_ocellus_skeleton = (median_ocellus) ? &median_ocellus->get_skeleton() : nullptr;
	bool postpetiole = (waist_skeleton.bone_map.find("postpetiole") != waist_skeleton.bone_map.end());
	
	// Allocate skeleton bones
	::skeleton& skeleton = model->get_skeleton();
	std::size_t bone_count = 34;
	if (postpetiole)
		bone_count += 1;
	if (sting)
		bone_count += 1;
	if (forewings)
		bone_count += 2;
	if (hindwings)
		bone_count += 2;
	
	// Assign bone indices
	std::uint8_t bone_index = 0;
	std::uint8_t mesosoma_bone_index = bone_index++;
	std::uint8_t foreleg_coxa_l_bone_index = bone_index++;
	std::uint8_t foreleg_coxa_r_bone_index = bone_index++;
	std::uint8_t foreleg_femur_l_bone_index = bone_index++;
	std::uint8_t foreleg_femur_r_bone_index = bone_index++;
	std::uint8_t foreleg_tibia_l_bone_index = bone_index++;
	std::uint8_t foreleg_tibia_r_bone_index = bone_index++;
	std::uint8_t foreleg_tarsus_l_bone_index = bone_index++;
	std::uint8_t foreleg_tarsus_r_bone_index = bone_index++;
	std::uint8_t midleg_coxa_l_bone_index = bone_index++;
	std::uint8_t midleg_coxa_r_bone_index = bone_index++;
	std::uint8_t midleg_femur_l_bone_index = bone_index++;
	std::uint8_t midleg_femur_r_bone_index = bone_index++;
	std::uint8_t midleg_tibia_l_bone_index = bone_index++;
	std::uint8_t midleg_tibia_r_bone_index = bone_index++;
	std::uint8_t midleg_tarsus_l_bone_index = bone_index++;
	std::uint8_t midleg_tarsus_r_bone_index = bone_index++;
	std::uint8_t hindleg_coxa_l_bone_index = bone_index++;
	std::uint8_t hindleg_coxa_r_bone_index = bone_index++;
	std::uint8_t hindleg_femur_l_bone_index = bone_index++;
	std::uint8_t hindleg_femur_r_bone_index = bone_index++;
	std::uint8_t hindleg_tibia_l_bone_index = bone_index++;
	std::uint8_t hindleg_tibia_r_bone_index = bone_index++;
	std::uint8_t hindleg_tarsus_l_bone_index = bone_index++;
	std::uint8_t hindleg_tarsus_r_bone_index = bone_index++;
	std::uint8_t head_bone_index = bone_index++;
	std::uint8_t mandible_l_bone_index = bone_index++;
	std::uint8_t mandible_r_bone_index = bone_index++;
	std::uint8_t scape_l_bone_index = bone_index++;
	std::uint8_t scape_r_bone_index = bone_index++;
	std::uint8_t pedicel_l_bone_index = bone_index++;
	std::uint8_t pedicel_r_bone_index = bone_index++;
	std::uint8_t petiole_bone_index = bone_index++;
	std::uint8_t postpetiole_bone_index = (postpetiole) ? bone_index++ : static_cast<std::uint8_t>(bone_count);
	std::uint8_t gaster_bone_index = bone_index++;
	std::uint8_t sting_bone_index = (sting) ? bone_index++ : static_cast<std::uint8_t>(bone_count);
	
	// Construct bone identifiers
	::bone mesosoma_bone = make_bone(mesosoma_bone_index);
	::bone foreleg_coxa_l_bone = make_bone(foreleg_coxa_l_bone_index, mesosoma_bone_index);
	::bone foreleg_coxa_r_bone = make_bone(foreleg_coxa_r_bone_index, mesosoma_bone_index);
	::bone foreleg_femur_l_bone = make_bone(foreleg_femur_l_bone_index, foreleg_coxa_l_bone_index);
	::bone foreleg_femur_r_bone = make_bone(foreleg_femur_r_bone_index, foreleg_coxa_r_bone_index);
	::bone foreleg_tibia_l_bone = make_bone(foreleg_tibia_l_bone_index, foreleg_femur_l_bone_index);
	::bone foreleg_tibia_r_bone = make_bone(foreleg_tibia_r_bone_index, foreleg_femur_r_bone_index);
	::bone foreleg_tarsus_l_bone = make_bone(foreleg_tarsus_l_bone_index, foreleg_tibia_l_bone_index);
	::bone foreleg_tarsus_r_bone = make_bone(foreleg_tarsus_r_bone_index, foreleg_tibia_r_bone_index);
	::bone midleg_coxa_l_bone = make_bone(midleg_coxa_l_bone_index, mesosoma_bone_index);
	::bone midleg_coxa_r_bone = make_bone(midleg_coxa_r_bone_index, mesosoma_bone_index);
	::bone midleg_femur_l_bone = make_bone(midleg_femur_l_bone_index, midleg_coxa_l_bone_index);
	::bone midleg_femur_r_bone = make_bone(midleg_femur_r_bone_index, midleg_coxa_r_bone_index);
	::bone midleg_tibia_l_bone = make_bone(midleg_tibia_l_bone_index, midleg_femur_l_bone_index);
	::bone midleg_tibia_r_bone = make_bone(midleg_tibia_r_bone_index, midleg_femur_r_bone_index);
	::bone midleg_tarsus_l_bone = make_bone(midleg_tarsus_l_bone_index, midleg_tibia_l_bone_index);
	::bone midleg_tarsus_r_bone = make_bone(midleg_tarsus_r_bone_index, midleg_tibia_r_bone_index);
	::bone hindleg_coxa_l_bone = make_bone(hindleg_coxa_l_bone_index, mesosoma_bone_index);
	::bone hindleg_coxa_r_bone = make_bone(hindleg_coxa_r_bone_index, mesosoma_bone_index);
	::bone hindleg_femur_l_bone = make_bone(hindleg_femur_l_bone_index, hindleg_coxa_l_bone_index);
	::bone hindleg_femur_r_bone = make_bone(hindleg_femur_r_bone_index, hindleg_coxa_r_bone_index);
	::bone hindleg_tibia_l_bone = make_bone(hindleg_tibia_l_bone_index, hindleg_femur_l_bone_index);
	::bone hindleg_tibia_r_bone = make_bone(hindleg_tibia_r_bone_index, hindleg_femur_r_bone_index);
	::bone hindleg_tarsus_l_bone = make_bone(hindleg_tarsus_l_bone_index, hindleg_tibia_l_bone_index);
	::bone hindleg_tarsus_r_bone = make_bone(hindleg_tarsus_r_bone_index, hindleg_tibia_r_bone_index);
	::bone head_bone = make_bone(head_bone_index, mesosoma_bone_index);
	::bone mandible_l_bone = make_bone(mandible_l_bone_index, head_bone_index);
	::bone mandible_r_bone = make_bone(mandible_r_bone_index, head_bone_index);
	::bone scape_l_bone = make_bone(scape_l_bone_index, head_bone_index);
	::bone scape_r_bone = make_bone(scape_r_bone_index, head_bone_index);
	::bone pedicel_l_bone = make_bone(pedicel_l_bone_index, scape_l_bone_index);
	::bone pedicel_r_bone = make_bone(pedicel_r_bone_index, scape_r_bone_index);
	::bone petiole_bone = make_bone(petiole_bone_index, mesosoma_bone_index);
	::bone postpetiole_bone = make_bone(postpetiole_bone_index, petiole_bone_index);
	::bone gaster_bone = make_bone(gaster_bone_index, (postpetiole) ? postpetiole_bone_index : petiole_bone_index);
	::bone sting_bone = make_bone(sting_bone_index, gaster_bone_index);
	
	// Map bone names to bones
	skeleton.bone_map["mesosoma"] = mesosoma_bone;
	skeleton.bone_map["foreleg_coxa_l"] = foreleg_coxa_l_bone;
	skeleton.bone_map["foreleg_coxa_r"] = foreleg_coxa_r_bone;
	skeleton.bone_map["foreleg_femur_l"] = foreleg_femur_l_bone;
	skeleton.bone_map["foreleg_femur_r"] = foreleg_femur_r_bone;
	skeleton.bone_map["foreleg_tibia_l"] = foreleg_tibia_l_bone;
	skeleton.bone_map["foreleg_tibia_r"] = foreleg_tibia_r_bone;
	skeleton.bone_map["foreleg_tarsus_l"] = foreleg_tarsus_l_bone;
	skeleton.bone_map["foreleg_tarsus_r"] = foreleg_tarsus_r_bone;
	skeleton.bone_map["midleg_coxa_l"] = midleg_coxa_l_bone;
	skeleton.bone_map["midleg_coxa_r"] = midleg_coxa_r_bone;
	skeleton.bone_map["midleg_femur_l"] = midleg_femur_l_bone;
	skeleton.bone_map["midleg_femur_r"] = midleg_femur_r_bone;
	skeleton.bone_map["midleg_tibia_l"] = midleg_tibia_l_bone;
	skeleton.bone_map["midleg_tibia_r"] = midleg_tibia_r_bone;
	skeleton.bone_map["midleg_tarsus_l"] = midleg_tarsus_l_bone;
	skeleton.bone_map["midleg_tarsus_r"] = midleg_tarsus_r_bone;
	skeleton.bone_map["hindleg_coxa_l"] = hindleg_coxa_l_bone;
	skeleton.bone_map["hindleg_coxa_r"] = hindleg_coxa_r_bone;
	skeleton.bone_map["hindleg_femur_l"] = hindleg_femur_l_bone;
	skeleton.bone_map["hindleg_femur_r"] = hindleg_femur_r_bone;
	skeleton.bone_map["hindleg_tibia_l"] = hindleg_tibia_l_bone;
	skeleton.bone_map["hindleg_tibia_r"] = hindleg_tibia_r_bone;
	skeleton.bone_map["hindleg_tarsus_l"] = hindleg_tarsus_l_bone;
	skeleton.bone_map["hindleg_tarsus_r"] = hindleg_tarsus_r_bone;
	skeleton.bone_map["head"] = head_bone;
	skeleton.bone_map["mandible_l"] = mandible_l_bone;
	skeleton.bone_map["mandible_r"] = mandible_r_bone;
	skeleton.bone_map["scape_l"] = scape_l_bone;
	skeleton.bone_map["scape_r"] = scape_r_bone;
	skeleton.bone_map["pedicel_l"] = pedicel_l_bone;
	skeleton.bone_map["pedicel_r"] = pedicel_r_bone;
	skeleton.bone_map["petiole"] = petiole_bone;
	if (postpetiole)
		skeleton.bone_map["postpetiole"] = postpetiole_bone;
	skeleton.bone_map["gaster"] = gaster_bone;
	if (sting)
		skeleton.bone_map["sting"] = sting_bone;
	
	// Get reference to skeleton bind pose
	pose& bind_pose = skeleton.bind_pose;
	
	// Skeleton mesosoma pose
	if (auto it = mesosoma_skeleton.bone_map.find("mesosoma"); it != mesosoma_skeleton.bone_map.end())
		bind_pose[mesosoma_bone] = mesosoma_skeleton.bind_pose.at(it->second);
	
	// Skeleton forelegs pose
	if (auto it = legs_skeleton.bone_map.find("foreleg_coxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_coxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_coxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_coxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_femur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_femur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_femur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_femur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_tibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_tibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_tarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[foreleg_tarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton midlegs pose
	if (auto it = legs_skeleton.bone_map.find("midleg_coxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_coxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_coxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_coxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_femur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_femur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_femur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_femur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_tibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_tibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_tarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[midleg_tarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton hindlegs pose
	if (auto it = legs_skeleton.bone_map.find("hindleg_coxa_l"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_coxa_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_coxa_r"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_coxa_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_femur_l"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_femur_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_femur_r"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_femur_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tibia_l"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_tibia_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tibia_r"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_tibia_r_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_tarsus_l_bone] = legs_skeleton.bind_pose.at(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		bind_pose[hindleg_tarsus_r_bone] = legs_skeleton.bind_pose.at(it->second);
	
	// Skeleton head pose
	bind_pose[head_bone] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("head")) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("head"));
	
	// Skeleton mandibles pose
	bind_pose[mandible_l_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_l")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_l"));
	bind_pose[mandible_r_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_r")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_r"));
	
	// Skeleton antennae pose
	bind_pose[scape_l_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("scape_l")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("scape_l"));
	bind_pose[scape_r_bone] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("scape_r")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("scape_r"));
	bind_pose[pedicel_l_bone] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("pedicel_l"));
	bind_pose[pedicel_r_bone] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("pedicel_r"));
	
	// Skeleton waist pose
	bind_pose[petiole_bone] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole")) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole"));
	if (postpetiole)
	{
		bind_pose[postpetiole_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole"));
	}
	
	// Skeleton gaster pose
	if (postpetiole)
	{
		bind_pose[gaster_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	else
	{
		bind_pose[gaster_bone] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	
	// Skeleton sting pose
	if (sting)
	{
		bind_pose[sting_bone] = gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting")) * sting_skeleton->bind_pose.at(sting_skeleton->bone_map.at("sting"));
	}
	
	// Calculate the skeleton-space bind pose
	pose bind_pose_ss;
	::concatenate(bind_pose, bind_pose_ss);
	
	// Calculate inverse skeleton-space bind pose
	::inverse(bind_pose_ss, skeleton.inverse_bind_pose);
	
	// Get number of vertex indices for each body part
	std::size_t mesosoma_index_count = (*mesosoma->get_groups())[0]->get_index_count();
	std::size_t legs_index_count = (*legs->get_groups())[0]->get_index_count();
	std::size_t head_index_count = (*head->get_groups())[0]->get_index_count();
	std::size_t mandibles_index_count = (*mandibles->get_groups())[0]->get_index_count();
	std::size_t antennae_index_count = (*antennae->get_groups())[0]->get_index_count();
	std::size_t waist_index_count = (*waist->get_groups())[0]->get_index_count();
	std::size_t gaster_index_count = (*gaster->get_groups())[0]->get_index_count();
	std::size_t sting_index_count = (sting) ? (*sting->get_groups())[0]->get_index_count() : 0;
	std::size_t eyes_index_count = (eyes) ? (*eyes->get_groups())[0]->get_index_count() : 0;
	std::size_t lateral_ocelli_index_count = (lateral_ocelli) ? (*lateral_ocelli->get_groups())[0]->get_index_count() : 0;
	std::size_t median_ocellus_index_count = (median_ocellus) ? (*median_ocellus->get_groups())[0]->get_index_count() : 0;
	std::size_t forewings_index_count = (forewings) ? (*forewings->get_groups())[0]->get_index_count() : 0;
	std::size_t hindwings_index_count = (hindwings) ? (*hindwings->get_groups())[0]->get_index_count() : 0;
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
	const math::transform<float>& legs_to_body = math::identity_transform<float>;
	
	// Reskin leg bones
	std::unordered_set<std::uint8_t> old_foreleg_coxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_coxa_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_coxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_coxa_l_indices, foreleg_coxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_femur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_femur_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_femur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_femur_l_indices, foreleg_femur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_tibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_tibia_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_tibia_l_indices, foreleg_tibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_tarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus2_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus3_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus4_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus5_l"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_tarsus_l_indices, foreleg_tarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_foreleg_coxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_coxa_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_coxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_coxa_r_indices, foreleg_coxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_femur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_femur_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_femur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_femur_r_indices, foreleg_femur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_tibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_tibia_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_tibia_r_indices, foreleg_tibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_foreleg_tarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus2_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus3_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus4_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("foreleg_tarsus5_r"); it != legs_skeleton.bone_map.end())
		old_foreleg_tarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_foreleg_tarsus_r_indices, foreleg_tarsus_r_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_midleg_coxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_coxa_l"); it != legs_skeleton.bone_map.end())
		old_midleg_coxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_coxa_l_indices, midleg_coxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_femur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_femur_l"); it != legs_skeleton.bone_map.end())
		old_midleg_femur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_femur_l_indices, midleg_femur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_tibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_tibia_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_tibia_l_indices, midleg_tibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_tarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus2_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus3_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus4_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus5_l"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_tarsus_l_indices, midleg_tarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_midleg_coxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_coxa_r"); it != legs_skeleton.bone_map.end())
		old_midleg_coxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_coxa_r_indices, midleg_coxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_femur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_femur_r"); it != legs_skeleton.bone_map.end())
		old_midleg_femur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_femur_r_indices, midleg_femur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_tibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_tibia_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_tibia_r_indices, midleg_tibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_midleg_tarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus2_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus3_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus4_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("midleg_tarsus5_r"); it != legs_skeleton.bone_map.end())
		old_midleg_tarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_midleg_tarsus_r_indices, midleg_tarsus_r_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_hindleg_coxa_l_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_coxa_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_coxa_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_coxa_l_indices, hindleg_coxa_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_femur_l_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_femur_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_femur_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_femur_l_indices, hindleg_femur_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_tibia_l_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_tibia_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tibia_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_tibia_l_indices, hindleg_tibia_l_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_tarsus_l_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus1_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus2_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus3_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus4_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_l_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus5_l"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_tarsus_l_indices, hindleg_tarsus_l_bone_index, legs_to_body);
	
	std::unordered_set<std::uint8_t> old_hindleg_coxa_r_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_coxa_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_coxa_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_coxa_r_indices, hindleg_coxa_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_femur_r_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_femur_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_femur_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_femur_r_indices, hindleg_femur_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_tibia_r_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_tibia_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tibia_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_tibia_r_indices, hindleg_tibia_r_bone_index, legs_to_body);
	std::unordered_set<std::uint8_t> old_hindleg_tarsus_r_indices;
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus1_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus2_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus3_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus4_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_r_indices.emplace(it->second);
	if (auto it = legs_skeleton.bone_map.find("hindleg_tarsus5_r"); it != legs_skeleton.bone_map.end())
		old_hindleg_tarsus_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + legs_vbo_offset, legs_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_hindleg_tarsus_r_indices, hindleg_tarsus_r_bone_index, legs_to_body);
	
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
	math::transform<float> antenna_l_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("scape_l"));
	math::transform<float> antenna_r_to_body = bind_pose_ss.at(head_bone) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("scape_r"));
	
	// Reskin scape bones
	std::unordered_set<std::uint8_t> old_scape_l_indices;
	if (auto it = antennae_skeleton.bone_map.find("scape_l"); it != antennae_skeleton.bone_map.end())
		old_scape_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_scape_l_indices, scape_l_bone_index, antenna_l_to_body);
	std::unordered_set<std::uint8_t> old_scape_r_indices;
	if (auto it = antennae_skeleton.bone_map.find("scape_r"); it != antennae_skeleton.bone_map.end())
		old_scape_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_scape_r_indices, scape_r_bone_index, antenna_r_to_body);
	
	// Reskin pedicel bones
	const std::vector<std::string> pedicel_bone_names =
	{
		"pedicel",
		"flagellomere_1",
		"flagellomere_2",
		"flagellomere_3",
		"flagellomere_4",
		"flagellomere_5",
		"flagellomere_6",
		"flagellomere_7",
		"flagellomere_8",
		"flagellomere_9",
		"flagellomere_10",
		"flagellomere_11",
		"flagellomere_12"
	};
	std::unordered_set<std::uint8_t> old_pedicel_l_indices;
	for (const std::string& bone_name: pedicel_bone_names)
		if (auto it = antennae_skeleton.bone_map.find(bone_name + "_l"); it != antennae_skeleton.bone_map.end())
			old_pedicel_l_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_pedicel_l_indices, pedicel_l_bone_index, antenna_l_to_body);
	std::unordered_set<std::uint8_t> old_pedicel_r_indices;
	for (const std::string& bone_name: pedicel_bone_names)
		if (auto it = antennae_skeleton.bone_map.find(bone_name + "_r"); it != antennae_skeleton.bone_map.end())
			old_pedicel_r_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + antennae_vbo_offset, antennae_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_pedicel_r_indices, pedicel_r_bone_index, antenna_r_to_body);
	
	// Calculate transform from waist space to body space
	math::transform<float> waist_to_body = bind_pose_ss.at(mesosoma_bone) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole"));
	
	// Reskin waist bones
	std::unordered_set<std::uint8_t> old_petiole_bone_indices;
	if (auto it = waist_skeleton.bone_map.find("petiole"); it != waist_skeleton.bone_map.end())
		old_petiole_bone_indices.emplace(it->second);
	reskin_vertices(vertex_buffer_data + waist_vbo_offset, waist_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_petiole_bone_indices, petiole_bone_index, waist_to_body);
	if (postpetiole)
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
	
	if (sting)
	{
		// Calculate transform from sting space to body space
		math::transform<float> sting_to_body = bind_pose_ss.at(gaster_bone) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting"));

		// Reskin sting bones
		std::unordered_set<std::uint8_t> old_sting_bone_indices;
		if (auto it = sting_skeleton->bone_map.find("sting"); it != sting_skeleton->bone_map.end())
			old_sting_bone_indices.emplace(it->second);
		reskin_vertices(vertex_buffer_data + sting_vbo_offset, sting_index_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, old_sting_bone_indices, sting_bone_index, sting_to_body);
	}
	
	if (eyes)
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
	
	if (lateral_ocelli)
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
	
	if (median_ocellus)
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
	if (eyes)
	{
		// Construct eyes model group
		render::model_group* eyes_group = model->add_group("eyes");
		eyes_group->set_material((*eyes->get_groups())[0]->get_material());
		eyes_group->set_drawing_mode(gl::drawing_mode::triangles);
		eyes_group->set_start_index(index_offset);
		eyes_group->set_index_count(eyes_index_count);
		index_offset += eyes_index_count;
	}
	
	if (lateral_ocelli || median_ocellus)
	{
		// Construct ocelli model group
		render::model_group* ocelli_group = model->add_group("ocelli");
		ocelli_group->set_drawing_mode(gl::drawing_mode::triangles);
		ocelli_group->set_start_index(index_offset);
		
		std::size_t index_count = 0;
		if (lateral_ocelli)
		{
			index_count += lateral_ocelli_index_count;
			index_offset += lateral_ocelli_index_count;
			ocelli_group->set_material((*lateral_ocelli->get_groups())[0]->get_material());
		}
		if (median_ocellus)
		{
			index_count += median_ocellus_index_count;
			index_offset += median_ocellus_index_count;
			if (!lateral_ocelli)
				ocelli_group->set_material((*median_ocellus->get_groups())[0]->get_material());
		}
		
		ocelli_group->set_index_count(index_count);
	}
	
	if (forewings)
	{
		// Construct forewings model group
		render::model_group* forewings_group = model->add_group("forewings");
		forewings_group->set_material((*forewings->get_groups())[0]->get_material());
		forewings_group->set_drawing_mode(gl::drawing_mode::triangles);
		forewings_group->set_start_index(index_offset);
		forewings_group->set_index_count(forewings_index_count);
		index_offset += forewings_index_count;
	}
	
	if (hindwings)
	{
		// Construct hindwings model group
		render::model_group* hindwings_group = model->add_group("hindwings");
		hindwings_group->set_material((*hindwings->get_groups())[0]->get_material());
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
		*x = position.x;
		*y = position.y;
		*z = position.z;
		*nx = normal.x;
		*ny = normal.y;
		*nz = normal.z;
		*tx = tangent.x;
		*ty = tangent.y;
		*tz = tangent.z;
		//*bts = ...
		*bone_index = static_cast<float>(new_bone_index);
	}
}

geom::aabb<float> calculate_bounds(std::uint8_t* vertex_data, std::size_t index_count, const gl::vertex_attribute& position_attribute)
{
	std::uint8_t* position_data = vertex_data + position_attribute.offset;
	
	geom::aabb<float> bounds;
	bounds.min_point.x = std::numeric_limits<float>::infinity();
	bounds.min_point.y = std::numeric_limits<float>::infinity();
	bounds.min_point.z = std::numeric_limits<float>::infinity();
	bounds.max_point.x = -std::numeric_limits<float>::infinity();
	bounds.max_point.y = -std::numeric_limits<float>::infinity();
	bounds.max_point.z = -std::numeric_limits<float>::infinity();
	
	for (std::size_t i = 0; i < index_count; ++i)
	{
		// Get vertex position
		float* x = reinterpret_cast<float*>(position_data + position_attribute.stride * i);
		float* y = x + 1;
		float* z = y + 1;
		
		bounds.min_point.x = std::min<float>(*x, bounds.min_point.x);
		bounds.min_point.y = std::min<float>(*y, bounds.min_point.y);
		bounds.min_point.z = std::min<float>(*z, bounds.min_point.z);
		bounds.max_point.x = std::max<float>(*x, bounds.max_point.x);
		bounds.max_point.y = std::max<float>(*y, bounds.max_point.y);
		bounds.max_point.z = std::max<float>(*z, bounds.max_point.z);
	}
	
	return bounds;
}

} // namespace ant
} // namespace game
