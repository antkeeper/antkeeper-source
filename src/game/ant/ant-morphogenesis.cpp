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

#include "game/ant/ant-morphogenesis.hpp"
#include <engine/render/material.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/math/transform-operators.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/debug/log.hpp>
#include <unordered_set>

namespace {

/// Set of pointers to all possible ant skeleton bones.
struct ant_bone_set
{
	::bone* mesosoma{nullptr};
	::bone* procoxa_l{nullptr};
	::bone* profemur_l{nullptr};
	::bone* protibia_l{nullptr};
	::bone* protarsus_l{nullptr};
	::bone* procoxa_r{nullptr};
	::bone* profemur_r{nullptr};
	::bone* protibia_r{nullptr};
	::bone* protarsus_r{nullptr};
	::bone* mesocoxa_l{nullptr};
	::bone* mesofemur_l{nullptr};
	::bone* mesotibia_l{nullptr};
	::bone* mesotarsus_l{nullptr};
	::bone* mesocoxa_r{nullptr};
	::bone* mesofemur_r{nullptr};
	::bone* mesotibia_r{nullptr};
	::bone* mesotarsus_r{nullptr};
	::bone* metacoxa_l{nullptr};
	::bone* metafemur_l{nullptr};
	::bone* metatibia_l{nullptr};
	::bone* metatarsus_l{nullptr};
	::bone* metacoxa_r{nullptr};
	::bone* metafemur_r{nullptr};
	::bone* metatibia_r{nullptr};
	::bone* metatarsus_r{nullptr};
	::bone* head{nullptr};
	::bone* mandible_l{nullptr};
	::bone* mandible_r{nullptr};
	::bone* antennomere1_l{nullptr};
	::bone* antennomere2_l{nullptr};
	::bone* antennomere1_r{nullptr};
	::bone* antennomere2_r{nullptr};
	::bone* petiole{nullptr};
	::bone* postpetiole{nullptr};
	::bone* gaster{nullptr};
	::bone* sting{nullptr};
	::bone* forewing_l{nullptr};
	::bone* forewing_r{nullptr};
	::bone* hindwing_l{nullptr};
	::bone* hindwing_r{nullptr};
};

/**
 * Reskins model vertices.
 *
 * @param vertex_data Vertex buffer data.
 * @param vertex_count Number of vertices to reskin.
 * @param position_attribute Vertex position attribute.
 * @param normal_attribute Vertex normal attribute.
 * @param tangent_attribute Vertex tangent attribute.
 * @param bone_index_attribute Vertex bone index attribute.
 * @param reskin_map Map of old bone index to a tuple containing the new bone index and a vertex transformation.
 */
void reskin_vertices
(
	std::byte* vertex_data,
	std::size_t vertex_count,
	const gl::vertex_attribute& position_attribute,
	const gl::vertex_attribute& normal_attribute,
	const gl::vertex_attribute& tangent_attribute,
	const gl::vertex_attribute& bone_index_attribute,
	const std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>>& reskin_map
)
{
	std::byte* position_data = vertex_data + position_attribute.offset;
	std::byte* normal_data = vertex_data + normal_attribute.offset;
	std::byte* tangent_data = vertex_data + tangent_attribute.offset;
	std::byte* bone_index_data = vertex_data + bone_index_attribute.offset;
	
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		// Get bone index of current vertex
		std::uint32_t& bone_index = reinterpret_cast<std::uint32_t&>(*(bone_index_data + bone_index_attribute.stride * i));
		
		// Ignore vertices with unmapped bone indices
		auto entry = reskin_map.find(static_cast<std::uint16_t>(bone_index));
		if (entry == reskin_map.end())
		{
			continue;
		}
		
		const auto& [new_bone_index, transform] = entry->second;
		
		// Update bone index
		bone_index = static_cast<std::uint32_t>(new_bone_index);
		
		// Get vertex attributes
		float3& position = reinterpret_cast<float3&>(*(position_data + position_attribute.stride * i));
		float3& normal = reinterpret_cast<float3&>(*(normal_data + normal_attribute.stride * i));
		float3& tangent = reinterpret_cast<float3&>(*(tangent_data + tangent_attribute.stride * i));
		
		// Transform vertex attributes
		position = (*transform) * position;
		normal = math::normalize(transform->rotation * normal);
		tangent = math::normalize(transform->rotation * tangent);
	}
}

/**
 * Calculates the bounds of vertex data.
 *
 * @param vertex_data Pointer to vertex data.
 * @param vertex_count Number of vertices.
 * @param position_attribute Vertex position attribute.
 *
 * @return Bounds of the vertex data.
 */
[[nodiscard]] geom::aabb<float> calculate_bounds
(
	const std::byte* vertex_data,
	std::size_t vertex_count,
	const gl::vertex_attribute& position_attribute
)
{
	const std::byte* position_data = vertex_data + position_attribute.offset;
	
	geom::aabb<float> bounds;
	bounds.min_point = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
	bounds.max_point = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
	
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		// Get vertex position
		const float3& position = reinterpret_cast<const float3&>(*(position_data + position_attribute.stride * i));
		
		bounds.min_point = math::min(bounds.min_point, position);
		bounds.max_point = math::max(bounds.max_point, position);
	}
	
	return bounds;
}

/**
 * Generates an ant exoskeleton material.
 *
 * @param pigmentation Ant pigmentation phene.
 * @param sculpturing Ant sculpturing phene.
 *
 * @return Generated ant exoskeleton material.
 */
[[nodiscard]] std::unique_ptr<render::material> generate_ant_exoskeleton_material
(
	const ant_pigmentation_phene& pigmentation,
	const ant_sculpturing_phene& sculpturing
)
{
	// Allocate copy of pigmentation material
	std::unique_ptr<render::material> exoskeleton_material = std::make_unique<render::material>(*pigmentation.material);
	
	// Set roughness variable
	exoskeleton_material->set_variable("roughness", std::make_shared<render::material_float>(1, sculpturing.roughness));
	
	// Set normal map variable
	exoskeleton_material->set_variable("normal_map", std::make_shared<render::material_texture_2d>(1, sculpturing.normal_map));
	
	return exoskeleton_material;
}

/**
 * Calculates the number of bones in an ant model skeleton.
 *
 * @return Ant model skeleton bone count.
 *
 * Required bones: 
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
 * Optional bones:
 *
 * * forewing_l
 * * forewing_r
 * * hindwing_l
 * * hindwing_r
 * * petiole
 * * postpetiole
 * * sting
 *
 */
[[nodiscard]] std::uint16_t count_ant_skeleton_bones(const ant_phenome& phenome) noexcept
{
	constexpr std::uint16_t minimum_bone_count = 33;
	
	std::uint16_t bone_count = minimum_bone_count;
	
	if (phenome.waist->petiole_present)
	{
		bone_count += 1;
	}
	if (phenome.waist->postpetiole_present)
	{
		bone_count += 1;
	}
	if (phenome.sting->present)
	{
		bone_count += 1;
	}
	if (phenome.wings->present)
	{
		bone_count += 4;
	}
	
	return bone_count;
}

/**
 * Builds an ant model skeleton.
 *
 * @param[in] phenome Ant phenome.
 * @param[out] skeleton Ant model skeleton.
 * @param[out] bones Set of ant model skeleton bones.
 */
void build_ant_skeleton(const ant_phenome& phenome, ::skeleton& skeleton, ant_bone_set& bones)
{
	// Allocate bones
	const std::uint16_t bone_count = count_ant_skeleton_bones(phenome);
	skeleton.bones.resize(bone_count);
	
	// Assign bone indices
	for (std::uint16_t i = 0; i < bone_count; ++i)
	{
		skeleton.bones[i] = i;
	}
	
	// Construct ant bone set
	::bone* current_bone = skeleton.bones.data();
	bones.mesosoma = current_bone;
	bones.procoxa_l = ++current_bone;
	bones.profemur_l = ++current_bone;
	bones.protibia_l = ++current_bone;
	bones.protarsus_l = ++current_bone;
	bones.procoxa_r = ++current_bone;
	bones.profemur_r = ++current_bone;
	bones.protibia_r = ++current_bone;
	bones.protarsus_r = ++current_bone;
	bones.mesocoxa_l = ++current_bone;
	bones.mesofemur_l = ++current_bone;
	bones.mesotibia_l = ++current_bone;
	bones.mesotarsus_l = ++current_bone;
	bones.mesocoxa_r = ++current_bone;
	bones.mesofemur_r = ++current_bone;
	bones.mesotibia_r = ++current_bone;
	bones.mesotarsus_r = ++current_bone;
	bones.metacoxa_l = ++current_bone;
	bones.metafemur_l = ++current_bone;
	bones.metatibia_l = ++current_bone;
	bones.metatarsus_l = ++current_bone;
	bones.metacoxa_r = ++current_bone;
	bones.metafemur_r = ++current_bone;
	bones.metatibia_r = ++current_bone;
	bones.metatarsus_r = ++current_bone;
	bones.head = ++current_bone;
	bones.mandible_l = ++current_bone;
	bones.mandible_r = ++current_bone;
	bones.antennomere1_l = ++current_bone;
	bones.antennomere2_l = ++current_bone;
	bones.antennomere1_r = ++current_bone;
	bones.antennomere2_r = ++current_bone;
	bones.petiole = phenome.waist->petiole_present ? ++current_bone : nullptr;
	bones.postpetiole = phenome.waist->postpetiole_present ? ++current_bone : nullptr;
	bones.gaster = ++current_bone;
	bones.sting = phenome.sting->present ? ++current_bone : nullptr;
	bones.forewing_l = phenome.wings->present ? ++current_bone : nullptr;
	bones.forewing_r = phenome.wings->present ? ++current_bone : nullptr;
	bones.hindwing_l = phenome.wings->present ? ++current_bone : nullptr;
	bones.hindwing_r = phenome.wings->present ? ++current_bone : nullptr;
	
	// Assign bone parents
	reparent_bone(*bones.mesosoma,       *bones.mesosoma);
	reparent_bone(*bones.procoxa_l,      *bones.mesosoma);
	reparent_bone(*bones.profemur_l,     *bones.procoxa_l);
	reparent_bone(*bones.protibia_l,     *bones.profemur_l);
	reparent_bone(*bones.protarsus_l,    *bones.protibia_l);
	reparent_bone(*bones.procoxa_r,      *bones.mesosoma);
	reparent_bone(*bones.profemur_r,     *bones.procoxa_r);
	reparent_bone(*bones.protibia_r,     *bones.profemur_r);
	reparent_bone(*bones.protarsus_r,    *bones.protibia_r);
	reparent_bone(*bones.mesocoxa_l,     *bones.mesosoma);
	reparent_bone(*bones.mesofemur_l,    *bones.mesocoxa_l);
	reparent_bone(*bones.mesotibia_l,    *bones.mesofemur_l);
	reparent_bone(*bones.mesotarsus_l,   *bones.mesotibia_l);
	reparent_bone(*bones.mesocoxa_r,     *bones.mesosoma);
	reparent_bone(*bones.mesofemur_r,    *bones.mesocoxa_r);
	reparent_bone(*bones.mesotibia_r,    *bones.mesofemur_r);
	reparent_bone(*bones.mesotarsus_r,   *bones.mesotibia_r);
	reparent_bone(*bones.metacoxa_l,     *bones.mesosoma);
	reparent_bone(*bones.metafemur_l,    *bones.metacoxa_l);
	reparent_bone(*bones.metatibia_l,    *bones.metafemur_l);
	reparent_bone(*bones.metatarsus_l,   *bones.metatibia_l);
	reparent_bone(*bones.metacoxa_r,     *bones.mesosoma);
	reparent_bone(*bones.metafemur_r,    *bones.metacoxa_r);
	reparent_bone(*bones.metatibia_r,    *bones.metafemur_r);
	reparent_bone(*bones.metatarsus_r,   *bones.metatibia_r);
	reparent_bone(*bones.head,           *bones.mesosoma);
	reparent_bone(*bones.mandible_l,     *bones.head);
	reparent_bone(*bones.mandible_r,     *bones.head);
	reparent_bone(*bones.antennomere1_l, *bones.head);
	reparent_bone(*bones.antennomere2_l, *bones.antennomere1_l);
	reparent_bone(*bones.antennomere1_r, *bones.head);
	reparent_bone(*bones.antennomere2_r, *bones.antennomere1_r);
	
	if (bones.petiole)
	{
		reparent_bone(*bones.petiole, *bones.mesosoma);
	}
	if (bones.postpetiole)
	{
		if (bones.petiole)
		{
			reparent_bone(*bones.postpetiole, *bones.petiole);
		}
		else
		{
			reparent_bone(*bones.postpetiole, *bones.mesosoma);
		}
	}
	
	if (bones.postpetiole)
	{
		reparent_bone(*bones.gaster, *bones.postpetiole);
	}
	else
	{
		if (bones.petiole)
		{
			reparent_bone(*bones.gaster, *bones.petiole);
		}
		else
		{
			reparent_bone(*bones.gaster, *bones.mesosoma);
		}
	}
	
	if (bones.sting)
	{
		reparent_bone(*bones.sting, *bones.gaster);
	}
	
	if (bones.forewing_l)
	{
		reparent_bone(*bones.forewing_l, *bones.mesosoma);
		reparent_bone(*bones.forewing_r, *bones.mesosoma);
		reparent_bone(*bones.hindwing_l, *bones.mesosoma);
		reparent_bone(*bones.hindwing_r, *bones.mesosoma);
	}
	
	// Map bone names to bones
	skeleton.bone_map["mesosoma"]       = *bones.mesosoma;
	skeleton.bone_map["procoxa_l"]      = *bones.procoxa_l;
	skeleton.bone_map["profemur_l"]     = *bones.profemur_l;
	skeleton.bone_map["protibia_l"]     = *bones.protibia_l;
	skeleton.bone_map["protarsus_l"]    = *bones.protarsus_l;
	skeleton.bone_map["procoxa_r"]      = *bones.procoxa_r;
	skeleton.bone_map["profemur_r"]     = *bones.profemur_r;
	skeleton.bone_map["protibia_r"]     = *bones.protibia_r;
	skeleton.bone_map["protarsus_r"]    = *bones.protarsus_r;
	skeleton.bone_map["mesocoxa_l"]     = *bones.mesocoxa_l;
	skeleton.bone_map["mesofemur_l"]    = *bones.mesofemur_l;
	skeleton.bone_map["mesotibia_l"]    = *bones.mesotibia_l;
	skeleton.bone_map["mesotarsus_l"]   = *bones.mesotarsus_l;
	skeleton.bone_map["mesocoxa_r"]     = *bones.mesocoxa_r;
	skeleton.bone_map["mesofemur_r"]    = *bones.mesofemur_r;
	skeleton.bone_map["mesotibia_r"]    = *bones.mesotibia_r;
	skeleton.bone_map["mesotarsus_r"]   = *bones.mesotarsus_r;
	skeleton.bone_map["metacoxa_l"]     = *bones.metacoxa_l;
	skeleton.bone_map["metafemur_l"]    = *bones.metafemur_l;
	skeleton.bone_map["metatibia_l"]    = *bones.metatibia_l;
	skeleton.bone_map["metatarsus_l"]   = *bones.metatarsus_l;
	skeleton.bone_map["metacoxa_r"]     = *bones.metacoxa_r;
	skeleton.bone_map["metafemur_r"]    = *bones.metafemur_r;
	skeleton.bone_map["metatibia_r"]    = *bones.metatibia_r;
	skeleton.bone_map["metatarsus_r"]   = *bones.metatarsus_r;
	skeleton.bone_map["head"]           = *bones.head;
	skeleton.bone_map["mandible_l"]     = *bones.mandible_l;
	skeleton.bone_map["mandible_r"]     = *bones.mandible_r;
	skeleton.bone_map["antennomere1_l"] = *bones.antennomere1_l;
	skeleton.bone_map["antennomere2_l"] = *bones.antennomere2_l;
	skeleton.bone_map["antennomere1_r"] = *bones.antennomere1_r;
	skeleton.bone_map["antennomere2_r"] = *bones.antennomere2_r;
	if (bones.petiole)
	{
		skeleton.bone_map["petiole"] = *bones.petiole;
	}
	if (bones.postpetiole)
	{
		skeleton.bone_map["postpetiole"] = *bones.postpetiole;
	}
	skeleton.bone_map["gaster"] = *bones.gaster;
	if (bones.sting)
	{
		skeleton.bone_map["sting"] = *bones.sting;
	}
	if (bones.forewing_l)
	{
		skeleton.bone_map["forewing_l"] = *bones.forewing_l;
		skeleton.bone_map["forewing_r"] = *bones.forewing_r;
		skeleton.bone_map["hindwing_l"] = *bones.hindwing_l;
		skeleton.bone_map["hindwing_r"] = *bones.hindwing_r;
	}
}

/**
 * Builds the bind pose of an ant skeleton.
 *
 * @param[in] phenome Ant phenome.
 * @param[in] bones Set of ant skeleton bones.
 * @param[in,out] skeleton Ant skeleton.
 * @param[out] bind_pose_ss Skeleton-space bind pose.
 */
void build_ant_bind_pose(const ant_phenome& phenome, const ant_bone_set& bones, ::skeleton& skeleton, ::pose& bind_pose_ss)
{
	// Get body part skeletons
	const ::skeleton& mesosoma_skeleton = phenome.mesosoma->model->get_skeleton();
	const ::skeleton& legs_skeleton = phenome.legs->model->get_skeleton();
	const ::skeleton& head_skeleton = phenome.head->model->get_skeleton();
	const ::skeleton& mandibles_skeleton = phenome.mandibles->model->get_skeleton();
	const ::skeleton& antennae_skeleton = phenome.antennae->model->get_skeleton();
	const ::skeleton& waist_skeleton = phenome.waist->model->get_skeleton();
	const ::skeleton& gaster_skeleton = phenome.gaster->model->get_skeleton();
	const ::skeleton* sting_skeleton = (phenome.sting->present) ? &phenome.sting->model->get_skeleton() : nullptr;
	const ::skeleton* eyes_skeleton = (phenome.eyes->present) ? &phenome.eyes->model->get_skeleton() : nullptr;
	const ::skeleton* lateral_ocelli_skeleton = (phenome.ocelli->lateral_ocelli_present) ? &phenome.ocelli->lateral_ocelli_model->get_skeleton() : nullptr;
	const ::skeleton* median_ocellus_skeleton = (phenome.ocelli->median_ocellus_present) ? &phenome.ocelli->median_ocellus_model->get_skeleton() : nullptr;
	const ::skeleton* forewings_skeleton = (phenome.wings->present) ? &phenome.wings->forewings_model->get_skeleton() : nullptr;
	const ::skeleton* hindwings_skeleton = (phenome.wings->present) ? &phenome.wings->hindwings_model->get_skeleton() : nullptr;
	
	// Get reference to skeleton bind pose
	pose& bind_pose = skeleton.bind_pose;
	
	// Build skeleton bind pose
	bind_pose[*bones.mesosoma] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("mesosoma"));
	bind_pose[*bones.procoxa_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("procoxa_l"));
	bind_pose[*bones.profemur_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("profemur_l"));
	bind_pose[*bones.protibia_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("protibia_l"));
	bind_pose[*bones.protarsus_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("protarsus1_l"));
	bind_pose[*bones.procoxa_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("procoxa_r"));
	bind_pose[*bones.profemur_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("profemur_r"));
	bind_pose[*bones.protibia_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("protibia_r"));
	bind_pose[*bones.protarsus_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("protarsus1_r"));
	bind_pose[*bones.mesocoxa_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesocoxa_l"));
	bind_pose[*bones.mesofemur_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesofemur_l"));
	bind_pose[*bones.mesotibia_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesotibia_l"));
	bind_pose[*bones.mesotarsus_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesotarsus1_l"));
	bind_pose[*bones.mesocoxa_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesocoxa_r"));
	bind_pose[*bones.mesofemur_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesofemur_r"));
	bind_pose[*bones.mesotibia_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesotibia_r"));
	bind_pose[*bones.mesotarsus_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("mesotarsus1_r"));
	bind_pose[*bones.metacoxa_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metacoxa_l"));
	bind_pose[*bones.metafemur_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metafemur_l"));
	bind_pose[*bones.metatibia_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metatibia_l"));
	bind_pose[*bones.metatarsus_l] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metatarsus1_l"));
	bind_pose[*bones.metacoxa_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metacoxa_r"));
	bind_pose[*bones.metafemur_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metafemur_r"));
	bind_pose[*bones.metatibia_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metatibia_r"));
	bind_pose[*bones.metatarsus_r] = legs_skeleton.bind_pose.at(legs_skeleton.bone_map.at("metatarsus1_r"));
	bind_pose[*bones.head] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("head")) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("head"));
	bind_pose[*bones.mandible_l] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_l")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_l"));
	bind_pose[*bones.mandible_r] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_r")) * mandibles_skeleton.bind_pose.at(mandibles_skeleton.bone_map.at("mandible_r"));
	bind_pose[*bones.antennomere1_l] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_l")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere1_l"));
	bind_pose[*bones.antennomere2_l] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere2_l"));
	bind_pose[*bones.antennomere1_r] = head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_r")) * antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere1_r"));
	bind_pose[*bones.antennomere2_r] = antennae_skeleton.bind_pose.at(antennae_skeleton.bone_map.at("antennomere2_r"));
	
	if (bones.petiole)
	{
		bind_pose[*bones.petiole] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole")) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole"));
	}
	
	if (bones.postpetiole)
	{
		bind_pose[*bones.postpetiole] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole"));
		bind_pose[*bones.gaster] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("postpetiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	else if (bones.petiole)
	{
		bind_pose[*bones.gaster] = waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("petiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	else
	{
		bind_pose[*bones.gaster] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole")) * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("gaster"));
	}
	
	if (bones.sting)
	{
		bind_pose[*bones.sting] = gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting")) * sting_skeleton->bind_pose.at(sting_skeleton->bone_map.at("sting"));
	}
	
	if (bones.forewing_l)
	{
		bind_pose[*bones.forewing_l] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("forewing_l")) * forewings_skeleton->bind_pose.at(forewings_skeleton->bone_map.at("forewing_l"));
		bind_pose[*bones.forewing_r] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("forewing_r")) * forewings_skeleton->bind_pose.at(forewings_skeleton->bone_map.at("forewing_r"));
		bind_pose[*bones.hindwing_l] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("hindwing_l")) * hindwings_skeleton->bind_pose.at(hindwings_skeleton->bone_map.at("hindwing_l"));
		bind_pose[*bones.hindwing_r] = mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("hindwing_r")) * hindwings_skeleton->bind_pose.at(hindwings_skeleton->bone_map.at("hindwing_r"));
	}
	
	// Calculate the skeleton-space bind pose
	::concatenate(bind_pose, bind_pose_ss);
	
	// Calculate inverse skeleton-space bind pose
	::inverse(bind_pose_ss, skeleton.inverse_bind_pose);
}

} // namespace

std::unique_ptr<render::model> ant_morphogenesis(const ant_phenome& phenome)
{
	// Generate exoskeleton material
	std::shared_ptr<render::material> exoskeleton_material = generate_ant_exoskeleton_material(*phenome.pigmentation, *phenome.sculpturing);
	
	// Get body part models
	const render::model* mesosoma_model = phenome.mesosoma->model.get();
	const render::model* legs_model = phenome.legs->model.get();
	const render::model* head_model = phenome.head->model.get();
	const render::model* mandibles_model = phenome.mandibles->model.get();
	const render::model* antennae_model = phenome.antennae->model.get();
	const render::model* waist_model = phenome.waist->model.get();
	const render::model* gaster_model = phenome.gaster->model.get();
	const render::model* sting_model = phenome.sting->model.get();
	const render::model* eyes_model = phenome.eyes->model.get();
	const render::model* lateral_ocelli_model = phenome.ocelli->lateral_ocelli_model.get();
	const render::model* median_ocellus_model = phenome.ocelli->median_ocellus_model.get();
	const render::model* forewings_model = phenome.wings->forewings_model.get();
	const render::model* hindwings_model = phenome.wings->hindwings_model.get();
	
	// Check for presence of required part models
	if (!mesosoma_model)
	{
		throw std::runtime_error("Ant phenome missing mesosoma model");
	}
	if (!legs_model)
	{
		throw std::runtime_error("Ant phenome missing legs model");
	}
	if (!head_model)
	{
		throw std::runtime_error("Ant phenome missing head model");
	}
	if (!mandibles_model)
	{
		throw std::runtime_error("Ant phenome missing mandibles model");
	}
	if (!antennae_model)
	{
		throw std::runtime_error("Ant phenome missing antennae model");
	}
	if (!waist_model)
	{
		throw std::runtime_error("Ant phenome missing waist model");
	}
	if (!gaster_model)
	{
		throw std::runtime_error("Ant phenome missing gaster model");
	}
	if (phenome.sting->present && !sting_model)
	{
		throw std::runtime_error("Ant phenome missing sting model");
	}
	if (phenome.eyes->present && !eyes_model)
	{
		throw std::runtime_error("Ant phenome missing eyes model");
	}
	if (phenome.ocelli->lateral_ocelli_present && !lateral_ocelli_model)
	{
		throw std::runtime_error("Ant phenome missing lateral ocelli model");
	}
	if (phenome.ocelli->median_ocellus_present && !median_ocellus_model)
	{
		throw std::runtime_error("Ant phenome missing median ocellus model");
	}
	if (phenome.wings->present)
	{
		if (!forewings_model)
		{
			throw std::runtime_error("Ant phenome missing forewings model");
		}
		if (!hindwings_model)
		{
			throw std::runtime_error("Ant phenome missing hindwings model");
		}
	}
	
	// Get body part vertex buffers
	const gl::vertex_buffer* mesosoma_vbo = mesosoma_model->get_vertex_buffer().get();
	const gl::vertex_buffer* legs_vbo = legs_model->get_vertex_buffer().get();
	const gl::vertex_buffer* head_vbo = head_model->get_vertex_buffer().get();
	const gl::vertex_buffer* mandibles_vbo = mandibles_model->get_vertex_buffer().get();
	const gl::vertex_buffer* antennae_vbo = antennae_model->get_vertex_buffer().get();
	const gl::vertex_buffer* waist_vbo = waist_model->get_vertex_buffer().get();
	const gl::vertex_buffer* gaster_vbo = gaster_model->get_vertex_buffer().get();
	const gl::vertex_buffer* sting_vbo = (phenome.sting->present) ? sting_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* eyes_vbo = (phenome.eyes->present) ? eyes_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* lateral_ocelli_vbo = (phenome.ocelli->lateral_ocelli_present) ? lateral_ocelli_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* median_ocellus_vbo = (phenome.ocelli->median_ocellus_present) ? median_ocellus_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* forewings_vbo = (phenome.wings->present) ? forewings_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* hindwings_vbo = (phenome.wings->present) ? hindwings_model->get_vertex_buffer().get() : nullptr;
	
	// Determine combined size of vertex buffers and save offsets
	std::size_t vertex_buffer_size = 0;
	const std::size_t mesosoma_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += mesosoma_vbo->size();
	const std::size_t legs_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += legs_vbo->size();
	const std::size_t head_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += head_vbo->size();
	const std::size_t mandibles_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += mandibles_vbo->size();
	const std::size_t antennae_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += antennae_vbo->size();
	const std::size_t waist_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += waist_vbo->size();
	const std::size_t gaster_vbo_offset = vertex_buffer_size;
	vertex_buffer_size += gaster_vbo->size();
	const std::size_t sting_vbo_offset = vertex_buffer_size;
	if (phenome.sting->present)
	{
		vertex_buffer_size += sting_vbo->size();
	}
	const std::size_t eyes_vbo_offset = vertex_buffer_size;
	if (phenome.eyes->present)
	{
		vertex_buffer_size += eyes_vbo->size();
	}
	const std::size_t lateral_ocelli_vbo_offset = vertex_buffer_size;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		vertex_buffer_size += lateral_ocelli_vbo->size();
	}
	const std::size_t median_ocellus_vbo_offset = vertex_buffer_size;
	if (phenome.ocelli->median_ocellus_present)
	{
		vertex_buffer_size += median_ocellus_vbo->size();
	}
	std::size_t forewings_vbo_offset;
	std::size_t hindwings_vbo_offset;
	if (phenome.wings->present)
	{
		forewings_vbo_offset = vertex_buffer_size;
		vertex_buffer_size += forewings_vbo->size();
		hindwings_vbo_offset = vertex_buffer_size;
		vertex_buffer_size += hindwings_vbo->size();
	}
	
	// Allocate combined vertex buffer data
	std::vector<std::byte> vertex_buffer_data(vertex_buffer_size);
	
	// Read body part vertex buffer data into combined vertex buffer data
	mesosoma_vbo->read({vertex_buffer_data.data() + mesosoma_vbo_offset, mesosoma_vbo->size()});
	legs_vbo->read({vertex_buffer_data.data() + legs_vbo_offset, legs_vbo->size()});
	head_vbo->read({vertex_buffer_data.data() + head_vbo_offset, head_vbo->size()});
	mandibles_vbo->read({vertex_buffer_data.data() + mandibles_vbo_offset, mandibles_vbo->size()});
	antennae_vbo->read({vertex_buffer_data.data() + antennae_vbo_offset, antennae_vbo->size()});
	waist_vbo->read({vertex_buffer_data.data() + waist_vbo_offset, waist_vbo->size()});
	gaster_vbo->read({vertex_buffer_data.data() + gaster_vbo_offset, gaster_vbo->size()});
	if (phenome.sting->present)
	{
		sting_vbo->read({vertex_buffer_data.data() + sting_vbo_offset, sting_vbo->size()});
	}
	if (phenome.eyes->present)
	{
		eyes_vbo->read({vertex_buffer_data.data() + eyes_vbo_offset, eyes_vbo->size()});
	}
	if (phenome.ocelli->lateral_ocelli_present)
	{
		lateral_ocelli_vbo->read({vertex_buffer_data.data() + lateral_ocelli_vbo_offset, lateral_ocelli_vbo->size()});
	}
	if (phenome.ocelli->median_ocellus_present)
	{
		median_ocellus_vbo->read({vertex_buffer_data.data() + median_ocellus_vbo_offset, median_ocellus_vbo->size()});
	}
	if (phenome.wings->present)
	{
		forewings_vbo->read({vertex_buffer_data.data() + forewings_vbo_offset, forewings_vbo->size()});
		hindwings_vbo->read({vertex_buffer_data.data() + hindwings_vbo_offset, hindwings_vbo->size()});
	}
	
	// Allocate model
	std::unique_ptr<render::model> model = std::make_unique<render::model>();
	
	// Setup model VAO
	gl::vertex_array& model_vao = *model->get_vertex_array();
	for (auto [location, attribute]: mesosoma_model->get_vertex_array()->attributes())
	{
		attribute.buffer = model->get_vertex_buffer().get();
		model_vao.bind(location, attribute);
	}
	
	// Get vertex attributes
	const gl::vertex_attribute* position_attribute = nullptr;
	const gl::vertex_attribute* normal_attribute = nullptr;
	const gl::vertex_attribute* tangent_attribute = nullptr;
	const gl::vertex_attribute* bone_index_attribute = nullptr;
	const auto& vertex_attribute_map = model_vao.attributes();
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::position); it != vertex_attribute_map.end())
	{
		position_attribute = &it->second;
	}
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::normal); it != vertex_attribute_map.end())
	{
		normal_attribute = &it->second;
	}
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::tangent); it != vertex_attribute_map.end())
	{
		tangent_attribute = &it->second;
	}
	if (auto it = vertex_attribute_map.find(render::vertex_attribute::bone_index); it != vertex_attribute_map.end())
	{
		bone_index_attribute = &it->second;
	}
	
	// Build ant skeleton
	::skeleton& skeleton = model->get_skeleton();
	ant_bone_set bones;
	build_ant_skeleton(phenome, skeleton, bones);
	
	// Build ant bind pose
	pose bind_pose_ss;
	build_ant_bind_pose(phenome, bones, skeleton, bind_pose_ss);
	
	// Get number of vertices for each body part
	const std::uint32_t mesosoma_vertex_count = (mesosoma_model->get_groups()).front().index_count;
	const std::uint32_t legs_vertex_count = (legs_model->get_groups()).front().index_count;
	const std::uint32_t head_vertex_count = (head_model->get_groups()).front().index_count;
	const std::uint32_t mandibles_vertex_count = (mandibles_model->get_groups()).front().index_count;
	const std::uint32_t antennae_vertex_count = (antennae_model->get_groups()).front().index_count;
	const std::uint32_t waist_vertex_count = (waist_model->get_groups()).front().index_count;
	const std::uint32_t gaster_vertex_count = (gaster_model->get_groups()).front().index_count;
	const std::uint32_t sting_vertex_count = (phenome.sting->present) ? (sting_model->get_groups()).front().index_count : 0;
	const std::uint32_t exoskeleton_vertex_count =
		mesosoma_vertex_count +
		legs_vertex_count +
		head_vertex_count +
		mandibles_vertex_count +
		antennae_vertex_count +
		waist_vertex_count +
		gaster_vertex_count +
		sting_vertex_count;
	const std::uint32_t eyes_vertex_count = (phenome.eyes->present) ? (eyes_model->get_groups()).front().index_count : 0;
	const std::uint32_t lateral_ocelli_vertex_count = (phenome.ocelli->lateral_ocelli_present) ? (lateral_ocelli_model->get_groups()).front().index_count : 0;
	const std::uint32_t median_ocellus_vertex_count = (phenome.ocelli->median_ocellus_present) ? (median_ocellus_model->get_groups()).front().index_count : 0;
	const std::uint32_t forewings_vertex_count = (phenome.wings->present) ? (forewings_model->get_groups()).front().index_count : 0;
	const std::uint32_t hindwings_vertex_count = (phenome.wings->present) ? (hindwings_model->get_groups()).front().index_count : 0;
	
	// Get body part skeletons
	const ::skeleton& mesosoma_skeleton = phenome.mesosoma->model->get_skeleton();
	const ::skeleton& legs_skeleton = phenome.legs->model->get_skeleton();
	const ::skeleton& head_skeleton = phenome.head->model->get_skeleton();
	const ::skeleton& mandibles_skeleton = phenome.mandibles->model->get_skeleton();
	const ::skeleton& antennae_skeleton = phenome.antennae->model->get_skeleton();
	const ::skeleton& waist_skeleton = phenome.waist->model->get_skeleton();
	const ::skeleton& gaster_skeleton = phenome.gaster->model->get_skeleton();
	const ::skeleton* sting_skeleton = (phenome.sting->present) ? &phenome.sting->model->get_skeleton() : nullptr;
	const ::skeleton* eyes_skeleton = (phenome.eyes->present) ? &phenome.eyes->model->get_skeleton() : nullptr;
	const ::skeleton* lateral_ocelli_skeleton = (phenome.ocelli->lateral_ocelli_present) ? &phenome.ocelli->lateral_ocelli_model->get_skeleton() : nullptr;
	const ::skeleton* median_ocellus_skeleton = (phenome.ocelli->median_ocellus_present) ? &phenome.ocelli->median_ocellus_model->get_skeleton() : nullptr;
	const ::skeleton* forewings_skeleton = (phenome.wings->present) ? &phenome.wings->forewings_model->get_skeleton() : nullptr;
	const ::skeleton* hindwings_skeleton = (phenome.wings->present) ? &phenome.wings->hindwings_model->get_skeleton() : nullptr;
	
	// Calculate transformations from part space to body space
	const math::transform<float> legs_to_body = math::transform<float>::identity;
	const math::transform<float> head_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("head"));
	const math::transform<float> mandible_l_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_l"));
	const math::transform<float> mandible_r_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("mandible_r"));
	const math::transform<float> antenna_l_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_l"));
	const math::transform<float> antenna_r_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("antenna_r"));
	const math::transform<float> waist_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("petiole"));
	
	math::transform<float> gaster_to_body;
	if (phenome.waist->postpetiole_present)
	{
		gaster_to_body = bind_pose_ss.at(*bones.postpetiole) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("gaster"));
	}
	else if (phenome.waist->petiole_present)
	{
		gaster_to_body = bind_pose_ss.at(*bones.petiole) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("gaster"));
	}
	else
	{
		gaster_to_body = bind_pose_ss.at(*bones.mesosoma) * waist_skeleton.bind_pose.at(waist_skeleton.bone_map.at("gaster"));
	}
	
	math::transform<float> sting_to_body;
	if (phenome.sting->present)
	{
		sting_to_body = gaster_to_body * gaster_skeleton.bind_pose.at(gaster_skeleton.bone_map.at("sting"));
	}
	
	math::transform<float> eye_l_to_body;
	math::transform<float> eye_r_to_body;
	if (phenome.eyes->present)
	{
		eye_l_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("eye_l"));
		eye_r_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("eye_r"));
	}
	
	math::transform<float> ocellus_l_to_body;
	math::transform<float> ocellus_r_to_body;
	math::transform<float> ocellus_m_to_body;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		ocellus_l_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_l"));
		ocellus_r_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_r"));
	}
	if (phenome.ocelli->median_ocellus_present)
	{
		ocellus_m_to_body = bind_pose_ss.at(*bones.head) * head_skeleton.bind_pose.at(head_skeleton.bone_map.at("ocellus_m"));
	}
	
	math::transform<float> forewing_l_to_body;
	math::transform<float> forewing_r_to_body;
	math::transform<float> hindwing_l_to_body;
	math::transform<float> hindwing_r_to_body;
	if (phenome.wings->present)
	{
		forewing_l_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("forewing_l"));
		forewing_r_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("forewing_r"));
		hindwing_l_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("hindwing_l"));
		hindwing_r_to_body = bind_pose_ss.at(*bones.mesosoma) * mesosoma_skeleton.bind_pose.at(mesosoma_skeleton.bone_map.at("hindwing_r"));
	}
	
	// Build legs vertex reskin map
	const std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> legs_reskin_map
	{
		{bone_index(legs_skeleton.bone_map.at("procoxa_l")),     {bone_index(*bones.procoxa_l),    &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("profemur_l")),    {bone_index(*bones.profemur_l),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protibia_l")),    {bone_index(*bones.protibia_l),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus1_l")),  {bone_index(*bones.protarsus_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus2_l")),  {bone_index(*bones.protarsus_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus3_l")),  {bone_index(*bones.protarsus_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus4_l")),  {bone_index(*bones.protarsus_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus5_l")),  {bone_index(*bones.protarsus_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("procoxa_r")),     {bone_index(*bones.procoxa_r),    &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("profemur_r")),    {bone_index(*bones.profemur_r),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protibia_r")),    {bone_index(*bones.protibia_r),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus1_r")),  {bone_index(*bones.protarsus_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus2_r")),  {bone_index(*bones.protarsus_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus3_r")),  {bone_index(*bones.protarsus_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus4_r")),  {bone_index(*bones.protarsus_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("protarsus5_r")),  {bone_index(*bones.protarsus_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesocoxa_l")),    {bone_index(*bones.mesocoxa_l),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesofemur_l")),   {bone_index(*bones.mesofemur_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotibia_l")),   {bone_index(*bones.mesotibia_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus1_l")), {bone_index(*bones.mesotarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus2_l")), {bone_index(*bones.mesotarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus3_l")), {bone_index(*bones.mesotarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus4_l")), {bone_index(*bones.mesotarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus5_l")), {bone_index(*bones.mesotarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesocoxa_r")),    {bone_index(*bones.mesocoxa_r),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesofemur_r")),   {bone_index(*bones.mesofemur_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotibia_r")),   {bone_index(*bones.mesotibia_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus1_r")), {bone_index(*bones.mesotarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus2_r")), {bone_index(*bones.mesotarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus3_r")), {bone_index(*bones.mesotarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus4_r")), {bone_index(*bones.mesotarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("mesotarsus5_r")), {bone_index(*bones.mesotarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metacoxa_l")),    {bone_index(*bones.metacoxa_l),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metafemur_l")),   {bone_index(*bones.metafemur_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatibia_l")),   {bone_index(*bones.metatibia_l),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus1_l")), {bone_index(*bones.metatarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus2_l")), {bone_index(*bones.metatarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus3_l")), {bone_index(*bones.metatarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus4_l")), {bone_index(*bones.metatarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus5_l")), {bone_index(*bones.metatarsus_l), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metacoxa_r")),    {bone_index(*bones.metacoxa_r),   &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metafemur_r")),   {bone_index(*bones.metafemur_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatibia_r")),   {bone_index(*bones.metatibia_r),  &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus1_r")), {bone_index(*bones.metatarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus2_r")), {bone_index(*bones.metatarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus3_r")), {bone_index(*bones.metatarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus4_r")), {bone_index(*bones.metatarsus_r), &legs_to_body}},
		{bone_index(legs_skeleton.bone_map.at("metatarsus5_r")), {bone_index(*bones.metatarsus_r), &legs_to_body}}
	};
	
	// Build head vertex reskin map
	const std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> head_reskin_map
	{
		{bone_index(head_skeleton.bone_map.at("head")), {bone_index(*bones.head), &head_to_body}}
	};
	
	// Build mandibles vertex reskin map
	const std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> mandibles_reskin_map
	{
		{bone_index(mandibles_skeleton.bone_map.at("mandible_l")), {bone_index(*bones.mandible_l), &mandible_l_to_body}},
		{bone_index(mandibles_skeleton.bone_map.at("mandible_r")), {bone_index(*bones.mandible_r), &mandible_r_to_body}}
	};
	
	// Build antennae vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> antennae_reskin_map
	{
		{bone_index(antennae_skeleton.bone_map.at("antennomere1_l")), {bone_index(*bones.antennomere1_l), &antenna_l_to_body}},
		{bone_index(antennae_skeleton.bone_map.at("antennomere2_l")), {bone_index(*bones.antennomere2_l), &antenna_l_to_body}},
		{bone_index(antennae_skeleton.bone_map.at("antennomere1_r")), {bone_index(*bones.antennomere1_r), &antenna_r_to_body}},
		{bone_index(antennae_skeleton.bone_map.at("antennomere2_r")), {bone_index(*bones.antennomere2_r), &antenna_r_to_body}}
	};
	for (std::uint8_t i = 3; i <= phenome.antennae->total_antennomere_count; ++i)
	{
		const std::string antennomere_l_name = std::format("antennomere{}_l", i);
		const std::string antennomere_r_name = std::format("antennomere{}_r", i);
		
		const hash::fnv1a32_t antennomere_l_key = hash::fnv1a32<char>(antennomere_l_name);
		const hash::fnv1a32_t antennomere_r_key = hash::fnv1a32<char>(antennomere_r_name);
		
		antennae_reskin_map.emplace(bone_index(antennae_skeleton.bone_map.at(antennomere_l_key)), std::tuple(bone_index(*bones.antennomere2_l), &antenna_l_to_body));
		antennae_reskin_map.emplace(bone_index(antennae_skeleton.bone_map.at(antennomere_r_key)), std::tuple(bone_index(*bones.antennomere2_r), &antenna_r_to_body));
	}
	
	// Build waist vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> waist_reskin_map;
	if (phenome.waist->petiole_present)
	{
		waist_reskin_map.emplace(bone_index(waist_skeleton.bone_map.at("petiole")), std::tuple(bone_index(*bones.petiole), &waist_to_body));
	}
	if (phenome.waist->postpetiole_present)
	{
		waist_reskin_map.emplace(bone_index(waist_skeleton.bone_map.at("postpetiole")), std::tuple(bone_index(*bones.postpetiole), &waist_to_body));
	}
	
	// Build gaster vertex reskin map
	const std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> gaster_reskin_map
	{
		{bone_index(gaster_skeleton.bone_map.at("gaster")), {bone_index(*bones.gaster), &gaster_to_body}}
	};
	
	// Build sting vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> sting_reskin_map;
	if (phenome.sting->present)
	{
		sting_reskin_map.emplace(bone_index(sting_skeleton->bone_map.at("sting")), std::tuple(bone_index(*bones.sting), &sting_to_body));
	}
	
	// Build eyes vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> eyes_reskin_map;
	if (phenome.eyes->present)
	{
		eyes_reskin_map.emplace(bone_index(eyes_skeleton->bone_map.at("eye_l")), std::tuple(bone_index(*bones.head), &eye_l_to_body));
		eyes_reskin_map.emplace(bone_index(eyes_skeleton->bone_map.at("eye_r")), std::tuple(bone_index(*bones.head), &eye_r_to_body));
	}
	
	// Build lateral ocelli vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> lateral_ocelli_reskin_map;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		lateral_ocelli_reskin_map.emplace(bone_index(lateral_ocelli_skeleton->bone_map.at("ocellus_l")), std::tuple(bone_index(*bones.head), &ocellus_l_to_body));
		lateral_ocelli_reskin_map.emplace(bone_index(lateral_ocelli_skeleton->bone_map.at("ocellus_r")), std::tuple(bone_index(*bones.head), &ocellus_r_to_body));
	}
	
	// Build median ocellus vertex reskin map
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> median_ocellus_reskin_map;
	if (phenome.ocelli->median_ocellus_present)
	{
		median_ocellus_reskin_map.emplace(bone_index(median_ocellus_skeleton->bone_map.at("ocellus_m")), std::tuple(bone_index(*bones.head), &ocellus_m_to_body));
	}
	
	// Build wings vertex reskin maps
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> forewings_reskin_map;
	std::unordered_map<std::uint16_t, std::tuple<std::uint16_t, const math::transform<float>*>> hindwings_reskin_map;
	if (phenome.wings->present)
	{
		forewings_reskin_map.emplace(bone_index(forewings_skeleton->bone_map.at("forewing_l")), std::tuple(bone_index(*bones.forewing_l), &forewing_l_to_body));
		forewings_reskin_map.emplace(bone_index(forewings_skeleton->bone_map.at("forewing_r")), std::tuple(bone_index(*bones.forewing_r), &forewing_r_to_body));
		hindwings_reskin_map.emplace(bone_index(hindwings_skeleton->bone_map.at("hindwing_l")), std::tuple(bone_index(*bones.hindwing_l), &hindwing_l_to_body));
		hindwings_reskin_map.emplace(bone_index(hindwings_skeleton->bone_map.at("hindwing_r")), std::tuple(bone_index(*bones.hindwing_r), &hindwing_r_to_body));
	}
	
	// Reskin legs vertices
	reskin_vertices(vertex_buffer_data.data() + legs_vbo_offset, legs_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, legs_reskin_map);
	
	// Reskin head vertices
	reskin_vertices(vertex_buffer_data.data() + head_vbo_offset, head_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, head_reskin_map);
	
	// Reskin mandibles vertices
	reskin_vertices(vertex_buffer_data.data() + mandibles_vbo_offset, mandibles_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, mandibles_reskin_map);
	
	// Reskin antennae vertices
	reskin_vertices(vertex_buffer_data.data() + antennae_vbo_offset, antennae_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, antennae_reskin_map);
	
	// Reskin waist vertices
	reskin_vertices(vertex_buffer_data.data() + waist_vbo_offset, waist_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, waist_reskin_map);
	
	// Reskin gaster vertices
	reskin_vertices(vertex_buffer_data.data() + gaster_vbo_offset, gaster_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, gaster_reskin_map);
	
	// Reskin sting vertices
	if (phenome.sting->present)
	{
		reskin_vertices(vertex_buffer_data.data() + sting_vbo_offset, sting_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, sting_reskin_map);
	}
	
	// Reskin eyes vertices
	if (phenome.eyes->present)
	{
		reskin_vertices(vertex_buffer_data.data() + eyes_vbo_offset, eyes_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, eyes_reskin_map);
	}
	
	// Reskin lateral ocelli vertices
	if (phenome.ocelli->lateral_ocelli_present)
	{
		reskin_vertices(vertex_buffer_data.data() + lateral_ocelli_vbo_offset, lateral_ocelli_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, lateral_ocelli_reskin_map);
	}
	
	// Reskin median ocellus vertices
	if (phenome.ocelli->median_ocellus_present)
	{
		reskin_vertices(vertex_buffer_data.data() + median_ocellus_vbo_offset, median_ocellus_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, median_ocellus_reskin_map);
	}
	
	// Reskin wings vertices
	if (phenome.wings->present)
	{
		reskin_vertices(vertex_buffer_data.data() + forewings_vbo_offset, forewings_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, forewings_reskin_map);
		
		reskin_vertices(vertex_buffer_data.data() + hindwings_vbo_offset, hindwings_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, hindwings_reskin_map);
	}
	
	// Upload vertex buffer data to model VBO
	model->get_vertex_buffer()->repurpose(gl::buffer_usage::static_draw, vertex_buffer_size, vertex_buffer_data);
	
	// Count number of model groups
	std::size_t model_group_count = 1;
	if (phenome.eyes->present)
	{
		++model_group_count;
	}
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		++model_group_count;
	}
	if (phenome.wings->present)
	{
		model_group_count += 2;
	}
	
	// Allocate model groups
	model->get_groups().resize(model_group_count);
	
	std::size_t model_group_index = 0;
	std::uint32_t index_offset = 0;
	
	// Construct exoskeleton model group
	render::model_group& exoskeleton_group = model->get_groups()[model_group_index];
	exoskeleton_group.id = "exoskeleton";
	exoskeleton_group.material = exoskeleton_material;
	exoskeleton_group.drawing_mode = gl::drawing_mode::triangles;
	exoskeleton_group.start_index = index_offset;
	exoskeleton_group.index_count = exoskeleton_vertex_count;
	
	index_offset += exoskeleton_group.index_count;
	
	if (phenome.eyes->present)
	{
		// Construct eyes model group
		render::model_group& eyes_group = model->get_groups()[++model_group_index];
		eyes_group.id = "eyes";
		eyes_group.material = eyes_model->get_groups().front().material;
		eyes_group.drawing_mode = gl::drawing_mode::triangles;
		eyes_group.start_index = index_offset;
		eyes_group.index_count = eyes_vertex_count;
		
		index_offset += eyes_group.index_count;
	}
	
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		// Construct ocelli model group
		render::model_group& ocelli_group = model->get_groups()[++model_group_index];
		ocelli_group.id = "ocelli";
		ocelli_group.drawing_mode = gl::drawing_mode::triangles;
		ocelli_group.start_index = index_offset;
		ocelli_group.index_count = 0;
		
		if (phenome.ocelli->lateral_ocelli_present)
		{
			ocelli_group.index_count += lateral_ocelli_vertex_count;
			ocelli_group.material = lateral_ocelli_model->get_groups().front().material;
		}
		if (phenome.ocelli->median_ocellus_present)
		{
			ocelli_group.index_count += median_ocellus_vertex_count;
			ocelli_group.material = median_ocellus_model->get_groups().front().material;
		}
		
		index_offset += ocelli_group.index_count;
	}
	
	if (phenome.wings->present)
	{
		// Construct forewings model group
		render::model_group& forewings_group = model->get_groups()[++model_group_index];
		forewings_group.id = "forewings";
		forewings_group.material = forewings_model->get_groups().front().material;
		forewings_group.drawing_mode = gl::drawing_mode::triangles;
		forewings_group.start_index = index_offset;
		forewings_group.index_count = forewings_vertex_count;
		
		index_offset += forewings_group.index_count;
		
		// Construct hindwings model group
		render::model_group& hindwings_group = model->get_groups()[++model_group_index];
		hindwings_group.id = "hindwings";
		hindwings_group.material = hindwings_model->get_groups().front().material;
		hindwings_group.drawing_mode = gl::drawing_mode::triangles;
		hindwings_group.start_index = index_offset;
		hindwings_group.index_count = hindwings_vertex_count;
		
		index_offset += hindwings_group.index_count;
	}
	
	// Calculate model bounding box
	model->get_bounds() = calculate_bounds(vertex_buffer_data.data(), index_offset, *position_attribute);
	
	return model;
}
