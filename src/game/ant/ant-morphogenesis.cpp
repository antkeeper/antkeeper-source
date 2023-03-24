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
#include <engine/math/quaternion.hpp>
#include <engine/debug/log.hpp>
#include <engine/geom/primitives/box.hpp>
#include <engine/animation/bone.hpp>
#include <unordered_set>
#include <optional>

namespace {

/// Set of pointers to all possible ant skeleton bones.
struct ant_bone_set
{
	std::optional<bone_index_type> mesosoma;
	std::optional<bone_index_type> procoxa_l;
	std::optional<bone_index_type> profemur_l;
	std::optional<bone_index_type> protibia_l;
	std::optional<bone_index_type> protarsus_l;
	std::optional<bone_index_type> procoxa_r;
	std::optional<bone_index_type> profemur_r;
	std::optional<bone_index_type> protibia_r;
	std::optional<bone_index_type> protarsus_r;
	std::optional<bone_index_type> mesocoxa_l;
	std::optional<bone_index_type> mesofemur_l;
	std::optional<bone_index_type> mesotibia_l;
	std::optional<bone_index_type> mesotarsus_l;
	std::optional<bone_index_type> mesocoxa_r;
	std::optional<bone_index_type> mesofemur_r;
	std::optional<bone_index_type> mesotibia_r;
	std::optional<bone_index_type> mesotarsus_r;
	std::optional<bone_index_type> metacoxa_l;
	std::optional<bone_index_type> metafemur_l;
	std::optional<bone_index_type> metatibia_l;
	std::optional<bone_index_type> metatarsus_l;
	std::optional<bone_index_type> metacoxa_r;
	std::optional<bone_index_type> metafemur_r;
	std::optional<bone_index_type> metatibia_r;
	std::optional<bone_index_type> metatarsus_r;
	std::optional<bone_index_type> head;
	std::optional<bone_index_type> mandible_l;
	std::optional<bone_index_type> mandible_r;
	std::optional<bone_index_type> antennomere1_l;
	std::optional<bone_index_type> antennomere2_l;
	std::optional<bone_index_type> antennomere1_r;
	std::optional<bone_index_type> antennomere2_r;
	std::optional<bone_index_type> petiole;
	std::optional<bone_index_type> postpetiole;
	std::optional<bone_index_type> gaster;
	std::optional<bone_index_type> sting;
	std::optional<bone_index_type> forewing_l;
	std::optional<bone_index_type> forewing_r;
	std::optional<bone_index_type> hindwing_l;
	std::optional<bone_index_type> hindwing_r;
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
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>>& reskin_map
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
		auto entry = reskin_map.find(static_cast<bone_index_type>(bone_index));
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
[[nodiscard]] geom::box<float> calculate_bounds
(
	const std::byte* vertex_data,
	std::size_t vertex_count,
	const gl::vertex_attribute& position_attribute
)
{
	const std::byte* position_data = vertex_data + position_attribute.offset;
	
	geom::box<float> bounds
	{
		{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
		{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()}
	};
	
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		const float3& position = reinterpret_cast<const float3&>(*(position_data + position_attribute.stride * i));	
		bounds.extend(position);
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
[[nodiscard]] std::size_t count_ant_skeleton_bones(const ant_phenome& phenome) noexcept
{
	constexpr std::size_t minimum_bone_count = 33;
	
	std::size_t bone_count = minimum_bone_count;
	
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
	skeleton.add_bones(count_ant_skeleton_bones(phenome));
	
	// Construct ant bone set
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
	
	// Assign bone parents
	skeleton.set_bone_parent(*bones.mesosoma,       *bones.mesosoma);
	skeleton.set_bone_parent(*bones.procoxa_l,      *bones.mesosoma);
	skeleton.set_bone_parent(*bones.profemur_l,     *bones.procoxa_l);
	skeleton.set_bone_parent(*bones.protibia_l,     *bones.profemur_l);
	skeleton.set_bone_parent(*bones.protarsus_l,    *bones.protibia_l);
	skeleton.set_bone_parent(*bones.procoxa_r,      *bones.mesosoma);
	skeleton.set_bone_parent(*bones.profemur_r,     *bones.procoxa_r);
	skeleton.set_bone_parent(*bones.protibia_r,     *bones.profemur_r);
	skeleton.set_bone_parent(*bones.protarsus_r,    *bones.protibia_r);
	skeleton.set_bone_parent(*bones.mesocoxa_l,     *bones.mesosoma);
	skeleton.set_bone_parent(*bones.mesofemur_l,    *bones.mesocoxa_l);
	skeleton.set_bone_parent(*bones.mesotibia_l,    *bones.mesofemur_l);
	skeleton.set_bone_parent(*bones.mesotarsus_l,   *bones.mesotibia_l);
	skeleton.set_bone_parent(*bones.mesocoxa_r,     *bones.mesosoma);
	skeleton.set_bone_parent(*bones.mesofemur_r,    *bones.mesocoxa_r);
	skeleton.set_bone_parent(*bones.mesotibia_r,    *bones.mesofemur_r);
	skeleton.set_bone_parent(*bones.mesotarsus_r,   *bones.mesotibia_r);
	skeleton.set_bone_parent(*bones.metacoxa_l,     *bones.mesosoma);
	skeleton.set_bone_parent(*bones.metafemur_l,    *bones.metacoxa_l);
	skeleton.set_bone_parent(*bones.metatibia_l,    *bones.metafemur_l);
	skeleton.set_bone_parent(*bones.metatarsus_l,   *bones.metatibia_l);
	skeleton.set_bone_parent(*bones.metacoxa_r,     *bones.mesosoma);
	skeleton.set_bone_parent(*bones.metafemur_r,    *bones.metacoxa_r);
	skeleton.set_bone_parent(*bones.metatibia_r,    *bones.metafemur_r);
	skeleton.set_bone_parent(*bones.metatarsus_r,   *bones.metatibia_r);
	skeleton.set_bone_parent(*bones.head,           *bones.mesosoma);
	skeleton.set_bone_parent(*bones.mandible_l,     *bones.head);
	skeleton.set_bone_parent(*bones.mandible_r,     *bones.head);
	skeleton.set_bone_parent(*bones.antennomere1_l, *bones.head);
	skeleton.set_bone_parent(*bones.antennomere2_l, *bones.antennomere1_l);
	skeleton.set_bone_parent(*bones.antennomere1_r, *bones.head);
	skeleton.set_bone_parent(*bones.antennomere2_r, *bones.antennomere1_r);
	
	if (bones.petiole)
	{
		skeleton.set_bone_parent(*bones.petiole, *bones.mesosoma);
	}
	if (bones.postpetiole)
	{
		if (bones.petiole)
		{
			skeleton.set_bone_parent(*bones.postpetiole, *bones.petiole);
		}
		else
		{
			skeleton.set_bone_parent(*bones.postpetiole, *bones.mesosoma);
		}
	}
	
	if (bones.postpetiole)
	{
		skeleton.set_bone_parent(*bones.gaster, *bones.postpetiole);
	}
	else
	{
		if (bones.petiole)
		{
			skeleton.set_bone_parent(*bones.gaster, *bones.petiole);
		}
		else
		{
			skeleton.set_bone_parent(*bones.gaster, *bones.mesosoma);
		}
	}
	
	if (bones.sting)
	{
		skeleton.set_bone_parent(*bones.sting, *bones.gaster);
	}
	
	if (bones.forewing_l)
	{
		skeleton.set_bone_parent(*bones.forewing_l, *bones.mesosoma);
		skeleton.set_bone_parent(*bones.forewing_r, *bones.mesosoma);
		skeleton.set_bone_parent(*bones.hindwing_l, *bones.mesosoma);
		skeleton.set_bone_parent(*bones.hindwing_r, *bones.mesosoma);
	}
	
	// Map bone names to bones
	skeleton.set_bone_name(*bones.mesosoma, "mesosoma");
	skeleton.set_bone_name(*bones.procoxa_l, "procoxa_l");
	skeleton.set_bone_name(*bones.profemur_l, "profemur_l");
	skeleton.set_bone_name(*bones.protibia_l, "protibia_l");
	skeleton.set_bone_name(*bones.protarsus_l, "protarsus_l");
	skeleton.set_bone_name(*bones.procoxa_r, "procoxa_r");
	skeleton.set_bone_name(*bones.profemur_r, "profemur_r");
	skeleton.set_bone_name(*bones.protibia_r, "protibia_r");
	skeleton.set_bone_name(*bones.protarsus_r, "protarsus_r");
	skeleton.set_bone_name(*bones.mesocoxa_l, "mesocoxa_l");
	skeleton.set_bone_name(*bones.mesofemur_l, "mesofemur_l");
	skeleton.set_bone_name(*bones.mesotibia_l, "mesotibia_l");
	skeleton.set_bone_name(*bones.mesotarsus_l, "mesotarsus_l");
	skeleton.set_bone_name(*bones.mesocoxa_r, "mesocoxa_r");
	skeleton.set_bone_name(*bones.mesofemur_r, "mesofemur_r");
	skeleton.set_bone_name(*bones.mesotibia_r, "mesotibia_r");
	skeleton.set_bone_name(*bones.mesotarsus_r, "mesotarsus_r");
	skeleton.set_bone_name(*bones.metacoxa_l, "metacoxa_l");
	skeleton.set_bone_name(*bones.metafemur_l, "metafemur_l");
	skeleton.set_bone_name(*bones.metatibia_l, "metatibia_l");
	skeleton.set_bone_name(*bones.metatarsus_l, "metatarsus_l");
	skeleton.set_bone_name(*bones.metacoxa_r, "metacoxa_r");
	skeleton.set_bone_name(*bones.metafemur_r, "metafemur_r");
	skeleton.set_bone_name(*bones.metatibia_r, "metatibia_r");
	skeleton.set_bone_name(*bones.metatarsus_r, "metatarsus_r");
	skeleton.set_bone_name(*bones.head, "head");
	skeleton.set_bone_name(*bones.mandible_l, "mandible_l");
	skeleton.set_bone_name(*bones.mandible_r, "mandible_r");
	skeleton.set_bone_name(*bones.antennomere1_l, "antennomere1_l");
	skeleton.set_bone_name(*bones.antennomere2_l, "antennomere2_l");
	skeleton.set_bone_name(*bones.antennomere1_r, "antennomere1_r");
	skeleton.set_bone_name(*bones.antennomere2_r, "antennomere2_r");
	if (bones.petiole)
	{
		skeleton.set_bone_name(*bones.petiole, "petiole");
	}
	if (bones.postpetiole)
	{
		skeleton.set_bone_name(*bones.postpetiole, "postpetiole");
	}
	skeleton.set_bone_name(*bones.gaster, "gaster");
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
}

/**
 * Builds the bind pose of an ant skeleton.
 *
 * @param[in] phenome Ant phenome.
 * @param[in] bones Set of ant skeleton bones.
 * @param[in,out] skeleton Ant skeleton.
 */
void build_ant_rest_pose(const ant_phenome& phenome, const ant_bone_set& bones, ::skeleton& skeleton)
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
	skeleton.set_bone_transform(*bones.mesosoma, mesosoma_transform);
	
	skeleton.set_bone_transform(*bones.procoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "procoxa_l"));
	skeleton.set_bone_transform(*bones.profemur_l, get_bone_transform(legs_skeleton, "profemur_l"));
	skeleton.set_bone_transform(*bones.protibia_l, get_bone_transform(legs_skeleton, "protibia_l"));
	skeleton.set_bone_transform(*bones.protarsus_l, get_bone_transform(legs_skeleton, "protarsus1_l"));
	
	skeleton.set_bone_transform(*bones.procoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "procoxa_r"));
	skeleton.set_bone_transform(*bones.profemur_r, get_bone_transform(legs_skeleton, "profemur_r"));
	skeleton.set_bone_transform(*bones.protibia_r, get_bone_transform(legs_skeleton, "protibia_r"));
	skeleton.set_bone_transform(*bones.protarsus_r, get_bone_transform(legs_skeleton, "protarsus1_r"));
	
	skeleton.set_bone_transform(*bones.mesocoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "mesocoxa_l"));
	skeleton.set_bone_transform(*bones.mesofemur_l, get_bone_transform(legs_skeleton, "mesofemur_l"));
	skeleton.set_bone_transform(*bones.mesotibia_l, get_bone_transform(legs_skeleton, "mesotibia_l"));
	skeleton.set_bone_transform(*bones.mesotarsus_l, get_bone_transform(legs_skeleton, "mesotarsus1_l"));
	
	skeleton.set_bone_transform(*bones.mesocoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "mesocoxa_r"));
	skeleton.set_bone_transform(*bones.mesofemur_r, get_bone_transform(legs_skeleton, "mesofemur_r"));
	skeleton.set_bone_transform(*bones.mesotibia_r, get_bone_transform(legs_skeleton, "mesotibia_r"));
	skeleton.set_bone_transform(*bones.mesotarsus_r, get_bone_transform(legs_skeleton, "mesotarsus1_r"));
	
	skeleton.set_bone_transform(*bones.metacoxa_l, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "metacoxa_l"));
	skeleton.set_bone_transform(*bones.metafemur_l, get_bone_transform(legs_skeleton, "metafemur_l"));
	skeleton.set_bone_transform(*bones.metatibia_l, get_bone_transform(legs_skeleton, "metatibia_l"));
	skeleton.set_bone_transform(*bones.metatarsus_l, get_bone_transform(legs_skeleton, "metatarsus1_l"));
	
	skeleton.set_bone_transform(*bones.metacoxa_r, inverse_mesosoma_transform * get_bone_transform(legs_skeleton, "metacoxa_r"));
	skeleton.set_bone_transform(*bones.metafemur_r, get_bone_transform(legs_skeleton, "metafemur_r"));
	skeleton.set_bone_transform(*bones.metatibia_r, get_bone_transform(legs_skeleton, "metatibia_r"));
	skeleton.set_bone_transform(*bones.metatarsus_r, get_bone_transform(legs_skeleton, "metatarsus1_r"));
	
	skeleton.set_bone_transform(*bones.head, head_socket_transform * head_transform);
	skeleton.set_bone_transform(*bones.mandible_l, get_bone_transform(head_skeleton, "mandible_l") * get_bone_transform(mandibles_skeleton, "mandible_l"));
	skeleton.set_bone_transform(*bones.mandible_r, get_bone_transform(head_skeleton, "mandible_r") * get_bone_transform(mandibles_skeleton, "mandible_r"));
	skeleton.set_bone_transform(*bones.antennomere1_l, get_bone_transform(head_skeleton, "antenna_l") * get_bone_transform(antennae_skeleton, "antennomere1_l"));
	skeleton.set_bone_transform(*bones.antennomere2_l, get_bone_transform(antennae_skeleton, "antennomere2_l"));
	skeleton.set_bone_transform(*bones.antennomere1_r, get_bone_transform(head_skeleton, "antenna_r") * get_bone_transform(antennae_skeleton, "antennomere1_r"));
	skeleton.set_bone_transform(*bones.antennomere2_r, get_bone_transform(antennae_skeleton, "antennomere2_r"));
	
	if (bones.petiole)
	{
		skeleton.set_bone_transform(*bones.petiole, get_bone_transform(mesosoma_skeleton, "petiole") * get_bone_transform(waist_skeleton, "petiole"));
	}
	
	if (bones.postpetiole)
	{
		skeleton.set_bone_transform(*bones.postpetiole, get_bone_transform(waist_skeleton, "postpetiole"));
	}
	
	skeleton.set_bone_transform(*bones.gaster, get_bone_transform(waist_skeleton, "gaster") * get_bone_transform(gaster_skeleton, "gaster"));
	
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
	
	// Build ant rest pose
	build_ant_rest_pose(phenome, bones, skeleton);
	const auto& rest_pose = skeleton.get_rest_pose();
	
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
	
	auto get_bone_transform = [](const ::skeleton& skeleton, hash::fnv1a32_t bone_name)
	{
		return skeleton.get_rest_pose().get_relative_transform(*skeleton.get_bone_index(bone_name));
	};
	
	// Calculate transformations from part space to body space
	const math::transform<float> legs_to_body = math::transform<float>::identity();
	const math::transform<float> head_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "head");
	const math::transform<float> mandible_l_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "mandible_l");
	const math::transform<float> mandible_r_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "mandible_r");
	const math::transform<float> antenna_l_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "antenna_l");
	const math::transform<float> antenna_r_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "antenna_r");
	const math::transform<float> waist_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "petiole");
	
	math::transform<float> gaster_to_body;
	if (phenome.waist->postpetiole_present)
	{
		gaster_to_body = rest_pose.get_absolute_transform(*bones.postpetiole) * get_bone_transform(waist_skeleton, "gaster");
	}
	else if (phenome.waist->petiole_present)
	{
		gaster_to_body = rest_pose.get_absolute_transform(*bones.petiole) * get_bone_transform(waist_skeleton, "gaster");
	}
	else
	{
		gaster_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(waist_skeleton, "gaster");
	}
	
	math::transform<float> sting_to_body;
	if (phenome.sting->present)
	{
		sting_to_body = gaster_to_body * get_bone_transform(gaster_skeleton, "sting");
	}
	
	math::transform<float> eye_l_to_body;
	math::transform<float> eye_r_to_body;
	if (phenome.eyes->present)
	{
		eye_l_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "eye_l");
		eye_r_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "eye_r");
	}
	
	math::transform<float> ocellus_l_to_body;
	math::transform<float> ocellus_r_to_body;
	math::transform<float> ocellus_m_to_body;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		ocellus_l_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "ocellus_l");
		ocellus_r_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "ocellus_r");
	}
	if (phenome.ocelli->median_ocellus_present)
	{
		ocellus_m_to_body = rest_pose.get_absolute_transform(*bones.head) * get_bone_transform(head_skeleton, "ocellus_m");
	}
	
	math::transform<float> forewing_l_to_body;
	math::transform<float> forewing_r_to_body;
	math::transform<float> hindwing_l_to_body;
	math::transform<float> hindwing_r_to_body;
	if (phenome.wings->present)
	{
		forewing_l_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "forewing_l");
		forewing_r_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "forewing_r");
		hindwing_l_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "hindwing_l");
		hindwing_r_to_body = rest_pose.get_absolute_transform(*bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "hindwing_r");
	}
	
	// Build legs vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> legs_reskin_map
	{
		{*legs_skeleton.get_bone_index("procoxa_l"),     {*bones.procoxa_l,    &legs_to_body}},
		{*legs_skeleton.get_bone_index("profemur_l"),    {*bones.profemur_l,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("protibia_l"),    {*bones.protibia_l,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus1_l"),  {*bones.protarsus_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus2_l"),  {*bones.protarsus_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus3_l"),  {*bones.protarsus_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus4_l"),  {*bones.protarsus_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus5_l"),  {*bones.protarsus_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("procoxa_r"),     {*bones.procoxa_r,    &legs_to_body}},
		{*legs_skeleton.get_bone_index("profemur_r"),    {*bones.profemur_r,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("protibia_r"),    {*bones.protibia_r,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus1_r"),  {*bones.protarsus_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus2_r"),  {*bones.protarsus_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus3_r"),  {*bones.protarsus_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus4_r"),  {*bones.protarsus_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("protarsus5_r"),  {*bones.protarsus_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesocoxa_l"),    {*bones.mesocoxa_l,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesofemur_l"),   {*bones.mesofemur_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotibia_l"),   {*bones.mesotibia_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus1_l"), {*bones.mesotarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus2_l"), {*bones.mesotarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus3_l"), {*bones.mesotarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus4_l"), {*bones.mesotarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus5_l"), {*bones.mesotarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesocoxa_r"),    {*bones.mesocoxa_r,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesofemur_r"),   {*bones.mesofemur_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotibia_r"),   {*bones.mesotibia_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus1_r"), {*bones.mesotarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus2_r"), {*bones.mesotarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus3_r"), {*bones.mesotarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus4_r"), {*bones.mesotarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsus5_r"), {*bones.mesotarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metacoxa_l"),    {*bones.metacoxa_l,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("metafemur_l"),   {*bones.metafemur_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatibia_l"),   {*bones.metatibia_l,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus1_l"), {*bones.metatarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus2_l"), {*bones.metatarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus3_l"), {*bones.metatarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus4_l"), {*bones.metatarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus5_l"), {*bones.metatarsus_l, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metacoxa_r"),    {*bones.metacoxa_r,   &legs_to_body}},
		{*legs_skeleton.get_bone_index("metafemur_r"),   {*bones.metafemur_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatibia_r"),   {*bones.metatibia_r,  &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus1_r"), {*bones.metatarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus2_r"), {*bones.metatarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus3_r"), {*bones.metatarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus4_r"), {*bones.metatarsus_r, &legs_to_body}},
		{*legs_skeleton.get_bone_index("metatarsus5_r"), {*bones.metatarsus_r, &legs_to_body}}
	};
	
	// Build head vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> head_reskin_map
	{
		{*head_skeleton.get_bone_index("head"), {*bones.head, &head_to_body}}
	};
	
	// Build mandibles vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> mandibles_reskin_map
	{
		{*mandibles_skeleton.get_bone_index("mandible_l"), {*bones.mandible_l, &mandible_l_to_body}},
		{*mandibles_skeleton.get_bone_index("mandible_r"), {*bones.mandible_r, &mandible_r_to_body}}
	};
	
	// Build antennae vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> antennae_reskin_map
	{
		{*antennae_skeleton.get_bone_index("antennomere1_l"), {*bones.antennomere1_l, &antenna_l_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere2_l"), {*bones.antennomere2_l, &antenna_l_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere1_r"), {*bones.antennomere1_r, &antenna_r_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere2_r"), {*bones.antennomere2_r, &antenna_r_to_body}}
	};
	for (std::uint8_t i = 3; i <= phenome.antennae->total_antennomere_count; ++i)
	{
		const std::string antennomere_l_name = std::format("antennomere{}_l", i);
		const std::string antennomere_r_name = std::format("antennomere{}_r", i);
		
		const hash::fnv1a32_t antennomere_l_key = hash::fnv1a32<char>(antennomere_l_name);
		const hash::fnv1a32_t antennomere_r_key = hash::fnv1a32<char>(antennomere_r_name);
		
		antennae_reskin_map.emplace(*antennae_skeleton.get_bone_index(antennomere_l_key), std::tuple(*bones.antennomere2_l, &antenna_l_to_body));
		antennae_reskin_map.emplace(*antennae_skeleton.get_bone_index(antennomere_r_key), std::tuple(*bones.antennomere2_r, &antenna_r_to_body));
	}
	
	// Build waist vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> waist_reskin_map;
	if (phenome.waist->petiole_present)
	{
		waist_reskin_map.emplace(*waist_skeleton.get_bone_index("petiole"), std::tuple(*bones.petiole, &waist_to_body));
	}
	if (phenome.waist->postpetiole_present)
	{
		waist_reskin_map.emplace(*waist_skeleton.get_bone_index("postpetiole"), std::tuple(*bones.postpetiole, &waist_to_body));
	}
	
	// Build gaster vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> gaster_reskin_map
	{
		{*gaster_skeleton.get_bone_index("gaster"), {*bones.gaster, &gaster_to_body}}
	};
	
	// Build sting vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> sting_reskin_map;
	if (phenome.sting->present)
	{
		sting_reskin_map.emplace(*sting_skeleton->get_bone_index("sting"), std::tuple(*bones.sting, &sting_to_body));
	}
	
	// Build eyes vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> eyes_reskin_map;
	if (phenome.eyes->present)
	{
		eyes_reskin_map.emplace(*eyes_skeleton->get_bone_index("eye_l"), std::tuple(*bones.head, &eye_l_to_body));
		eyes_reskin_map.emplace(*eyes_skeleton->get_bone_index("eye_r"), std::tuple(*bones.head, &eye_r_to_body));
	}
	
	// Build lateral ocelli vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> lateral_ocelli_reskin_map;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		lateral_ocelli_reskin_map.emplace(*lateral_ocelli_skeleton->get_bone_index("ocellus_l"), std::tuple(*bones.head, &ocellus_l_to_body));
		lateral_ocelli_reskin_map.emplace(*lateral_ocelli_skeleton->get_bone_index("ocellus_r"), std::tuple(*bones.head, &ocellus_r_to_body));
	}
	
	// Build median ocellus vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> median_ocellus_reskin_map;
	if (phenome.ocelli->median_ocellus_present)
	{
		median_ocellus_reskin_map.emplace(*median_ocellus_skeleton->get_bone_index("ocellus_m"), std::tuple(*bones.head, &ocellus_m_to_body));
	}
	
	// Build wings vertex reskin maps
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> forewings_reskin_map;
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> hindwings_reskin_map;
	if (phenome.wings->present)
	{
		forewings_reskin_map.emplace(*forewings_skeleton->get_bone_index("forewing_l"), std::tuple(*bones.forewing_l, &forewing_l_to_body));
		forewings_reskin_map.emplace(*forewings_skeleton->get_bone_index("forewing_r"), std::tuple(*bones.forewing_r, &forewing_r_to_body));
		hindwings_reskin_map.emplace(*hindwings_skeleton->get_bone_index("hindwing_l"), std::tuple(*bones.hindwing_l, &hindwing_l_to_body));
		hindwings_reskin_map.emplace(*hindwings_skeleton->get_bone_index("hindwing_r"), std::tuple(*bones.hindwing_r, &hindwing_r_to_body));
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
