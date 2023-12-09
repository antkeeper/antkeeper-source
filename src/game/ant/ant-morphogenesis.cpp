// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/ant-morphogenesis.hpp"
#include "game/ant/ant-bone-set.hpp"
#include "game/ant/ant-skeleton.hpp"
#include <engine/render/material.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/debug/log.hpp>
#include <engine/geom/primitives/box.hpp>
#include <engine/animation/bone.hpp>
#include <unordered_set>
#include <optional>

namespace {

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
	const gl::vertex_input_attribute& position_attribute,
	const gl::vertex_input_attribute& normal_attribute,
	const gl::vertex_input_attribute& tangent_attribute,
	const gl::vertex_input_attribute& bone_index_attribute,
	std::size_t vertex_stride,
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
		std::uint16_t& bone_index = reinterpret_cast<std::uint16_t&>(*(bone_index_data + vertex_stride * i));
		
		// Ignore vertices with unmapped bone indices
		auto entry = reskin_map.find(static_cast<bone_index_type>(bone_index));
		if (entry == reskin_map.end())
		{
			continue;
		}
		
		const auto& [new_bone_index, transform] = entry->second;
		
		// Update bone index
		bone_index = static_cast<std::uint16_t>(new_bone_index);
		
		// Get vertex attributes
		float* px = reinterpret_cast<float*>(position_data + vertex_stride * i);
		float* py = px + 1;
		float* pz = py + 1;
		float* nx = reinterpret_cast<float*>(normal_data + vertex_stride * i);
		float* ny = nx + 1;
		float* nz = ny + 1;
		float* tx = reinterpret_cast<float*>(tangent_data + vertex_stride * i);
		float* ty = tx + 1;
		float* tz = ty + 1;
		
		// Transform vertex attributes
		const math::fvec3 position = (*transform) * math::fvec3{*px, *py, *pz};
		const math::fvec3 normal = math::normalize(transform->rotation * math::fvec3{*nx, *ny, *nz});
		const math::fvec3 tangent = math::normalize(transform->rotation * math::fvec3{*tx, *ty, *tz});
		
		// Update vertex attributes
		*px = position.x();
		*py = position.y();
		*pz = position.z();
		*nx = normal.x();
		*ny = normal.y();
		*nz = normal.z();
		*tx = tangent.x();
		*ty = tangent.y();
		*tz = tangent.z();
	}
}

/**
 * Tags the vertices of a body part by storing a value in the fourth bone index.
 *
 * @param vertex_data Vertex buffer data.
 * @param bone_index_attribute Vertex bone index attribute.
 * @param reskin_map Map of old bone index to a tuple containing the new bone index and a vertex transformation.
 */
void tag_vertices
(
	std::span<std::byte> vertex_data,
	const gl::vertex_input_attribute& bone_index_attribute,
	std::size_t vertex_stride,
	std::uint16_t vertex_tag
)
{
	std::byte* bone_index_data = vertex_data.data() + bone_index_attribute.offset;
	
	for (std::size_t i = 0; i < vertex_data.size(); ++i)
	{
		// Get bone indices of current vertex
		std::uint16_t* bone_indices = reinterpret_cast<std::uint16_t*>(bone_index_data + vertex_stride * i);
		
		// Tag fourth bone index
		bone_indices[3] = vertex_tag;
	}
}

/**
 * Calculates the total area of UV coordinates.
 *
 * @param vertex_data Vertex buffer data.
 * @param uv_attribute Vertex UV attribute.
 *
 * @return Total UV area.
 */
float calculate_uv_area
(
	std::span<std::byte> vertex_data,
	const gl::vertex_input_attribute& uv_attribute,
	std::size_t vertex_stride
)
{
	std::byte* uv_data = vertex_data.data() + uv_attribute.offset;
	
	float sum_area = 0.0f;
	
	for (std::size_t i = 0; i + 2 < vertex_data.size(); i += 3)
	{
		const float* uv_data_a = reinterpret_cast<const float*>(uv_data + vertex_stride * i);
		const float* uv_data_b = reinterpret_cast<const float*>(uv_data + vertex_stride * (i + 1));
		const float* uv_data_c = reinterpret_cast<const float*>(uv_data + vertex_stride * (i + 2));
		
		const math::fvec3 uva = {uv_data_a[0], uv_data_a[1], 0.0f};
		const math::fvec3 uvb = {uv_data_b[0], uv_data_b[1], 0.0f};
		const math::fvec3 uvc = {uv_data_c[0], uv_data_c[1], 0.0f};
		
		const math::fvec3 uvab = uvb - uva;
		const math::fvec3 uvac = uvc - uva;
		
		sum_area += math::length(math::cross(uvab, uvac)) * 0.5f;
	}
	
	return sum_area;
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
	const gl::vertex_input_attribute& position_attribute,
	std::size_t vertex_stride
)
{
	const std::byte* position_data = vertex_data + position_attribute.offset;
	
	geom::box<float> bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		const float* px = reinterpret_cast<const float*>(position_data + vertex_stride * i);
		const float* py = px + 1;
		const float* pz = py + 1;
		
		bounds.extend(math::fvec3{*px, *py, *pz});
	}
	
	return bounds;
}

/**
 * Calculates a scale factor which will give ant eyes the desired number of ommatidia.
 *
 * @param eye_uv_area Total UV area of a single eye.
 * @param ommatidia_count Desired number of ommatidia.
 *
 * @return Ommatidia scale factor.
 */
[[nodiscard]] float calculate_ommatidia_scale(float eye_uv_area, float ommatidia_count)
{
	// Side length of hexagon tiles generated by the eye shader
	constexpr float source_side_length = 1.0f / math::sqrt_3<float>;
	
	// Side length of hexagon tiles that will fill UV area
	const float target_side_length = std::sqrt((eye_uv_area * 2.0f) / (3.0f * math::sqrt_3<float> * ommatidia_count));
	
	return source_side_length / target_side_length;
}

/**
 * Generates an ant exoskeleton material.
 *
 * @param phenome Ant phenome.
 * @param eye_uv_area Total UV area of a single eye.
 *
 * @return Generated ant exoskeleton material.
 */
[[nodiscard]] std::unique_ptr<render::material> generate_ant_exoskeleton_material
(
	const ant_phenome& phenome,
	float eye_uv_area
)
{
	// Allocate copy of pigmentation material
	std::unique_ptr<render::material> exoskeleton_material = std::make_unique<render::material>(*phenome.pigmentation->material);
	
	// Set roughness variable
	exoskeleton_material->set_variable("exoskeleton_roughness", std::make_shared<render::matvar_float>(1, phenome.sculpturing->roughness));
	
	// Set normal map variable
	exoskeleton_material->set_variable("exoskeleton_normal_map", std::make_shared<render::matvar_texture_2d>(1, phenome.sculpturing->normal_map));
	
	if (phenome.eyes->present)
	{
		// Set ommatidia scale variable
		const float ommatidia_scale = calculate_ommatidia_scale(eye_uv_area, static_cast<float>(phenome.eyes->ommatidia_count));
		exoskeleton_material->set_variable("ommatidia_scale", std::make_shared<render::matvar_float>(1, ommatidia_scale));
	}
	
	return exoskeleton_material;
}

} // namespace

std::unique_ptr<render::model> ant_morphogenesis(const ant_phenome& phenome)
{
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
	const render::model* ocelli_model = phenome.ocelli->model.get();
	const render::model* wings_model = phenome.wings->model.get();
	
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
	if ((phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present) && !ocelli_model)
	{
		throw std::runtime_error("Ant phenome missing ocelli model");
	}
	if (phenome.wings->present && !wings_model)
	{
		throw std::runtime_error("Ant phenome missing wings model");
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
	const gl::vertex_buffer* ocelli_vbo = (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present) ? ocelli_model->get_vertex_buffer().get() : nullptr;
	const gl::vertex_buffer* wings_vbo = (phenome.wings->present) ? wings_model->get_vertex_buffer().get() : nullptr;
	
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
	const std::size_t ocelli_vbo_offset = vertex_buffer_size;
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		vertex_buffer_size += ocelli_vbo->size();
	}
	std::size_t wings_vbo_offset = vertex_buffer_size;
	if (phenome.wings->present)
	{
		vertex_buffer_size += wings_vbo->size();
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
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		ocelli_vbo->read({vertex_buffer_data.data() + ocelli_vbo_offset, ocelli_vbo->size()});
	}
	if (phenome.wings->present)
	{
		wings_vbo->read({vertex_buffer_data.data() + wings_vbo_offset, wings_vbo->size()});
	}
	
	// Allocate model
	std::unique_ptr<render::model> model = std::make_unique<render::model>();
	
	// Construct model VAO (clone mesosoma model VAO)
	auto& model_vao = model->get_vertex_array();
	model_vao = std::make_unique<gl::vertex_array>(mesosoma_model->get_vertex_array()->attributes());
	
	// Get vertex attributes
	const gl::vertex_input_attribute* position_attribute = nullptr;
	const gl::vertex_input_attribute* uv_attribute = nullptr;
	const gl::vertex_input_attribute* normal_attribute = nullptr;
	const gl::vertex_input_attribute* tangent_attribute = nullptr;
	const gl::vertex_input_attribute* bone_index_attribute = nullptr;
	for (const auto& attribute: model_vao->attributes())
	{
		switch (attribute.location)
		{
			case render::vertex_attribute_location::position:
				position_attribute = &attribute;
				break;
			case render::vertex_attribute_location::uv:
				uv_attribute = &attribute;
				break;
			case render::vertex_attribute_location::normal:
				normal_attribute = &attribute;
				break;
			case render::vertex_attribute_location::tangent:
				tangent_attribute = &attribute;
				break;
			case render::vertex_attribute_location::bone_index:
				bone_index_attribute = &attribute;
				break;
			default:
				break;
		}
	}
	
	// Init model vertex binding
	model->set_vertex_offset(0);
	model->set_vertex_stride(mesosoma_model->get_vertex_stride());
	
	// Generate ant skeleton
	::skeleton& skeleton = model->get_skeleton();
	ant_bone_set bones;
	generate_ant_skeleton(skeleton, bones, phenome);
	const auto& rest_pose = skeleton.get_rest_pose();
	
	// Get number of vertices for each body part
	const std::uint32_t mesosoma_vertex_count = (mesosoma_model->get_groups()).front().vertex_count;
	const std::uint32_t legs_vertex_count = (legs_model->get_groups()).front().vertex_count;
	const std::uint32_t head_vertex_count = (head_model->get_groups()).front().vertex_count;
	const std::uint32_t mandibles_vertex_count = (mandibles_model->get_groups()).front().vertex_count;
	const std::uint32_t antennae_vertex_count = (antennae_model->get_groups()).front().vertex_count;
	const std::uint32_t waist_vertex_count = (waist_model->get_groups()).front().vertex_count;
	const std::uint32_t gaster_vertex_count = (gaster_model->get_groups()).front().vertex_count;
	const std::uint32_t sting_vertex_count = (phenome.sting->present) ? (sting_model->get_groups()).front().vertex_count : 0;
	const std::uint32_t eyes_vertex_count = (phenome.eyes->present) ? (eyes_model->get_groups()).front().vertex_count : 0;
	const std::uint32_t ocelli_vertex_count = (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present) ? (ocelli_model->get_groups()).front().vertex_count : 0;
	const std::uint32_t wings_vertex_count = (phenome.wings->present) ? wings_model->get_groups().front().vertex_count : 0;
	
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
	const ::skeleton* ocelli_skeleton = (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present) ? &phenome.ocelli->model->get_skeleton() : nullptr;
	const ::skeleton* wings_skeleton = (phenome.wings->present) ? &phenome.wings->model->get_skeleton() : nullptr;
	
	auto get_bone_transform = [](const ::skeleton& skeleton, hash::fnv1a32_t bone_name)
	{
		return skeleton.get_rest_pose().get_relative_transform(*skeleton.get_bone_index(bone_name));
	};
	
	// Calculate transformations from part space to body space
	const math::transform<float> procoxa_l_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "procoxa_socket_l");
	const math::transform<float> procoxa_r_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "procoxa_socket_r");
	const math::transform<float> mesocoxa_l_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_l");
	const math::transform<float> mesocoxa_r_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "mesocoxa_socket_r");
	const math::transform<float> metacoxa_l_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "metacoxa_socket_l");
	const math::transform<float> metacoxa_r_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "metacoxa_socket_r");
	const math::transform<float> head_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "head_socket");
	const math::transform<float> mandible_l_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "mandible_socket_l");
	const math::transform<float> mandible_r_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "mandible_socket_r");
	const math::transform<float> antenna_l_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "antenna_socket_l");
	const math::transform<float> antenna_r_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "antenna_socket_r");
	const math::transform<float> waist_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "petiole_socket");
	
	math::transform<float> gaster_to_body;
	if (phenome.waist->present)
	{
		if (phenome.waist->postpetiole_present)
		{
			gaster_to_body = rest_pose.get_absolute_transform(*bones.postpetiole) * get_bone_transform(waist_skeleton, "gaster_socket");
		}
		else
		{
			gaster_to_body = rest_pose.get_absolute_transform(*bones.petiole) * get_bone_transform(waist_skeleton, "gaster_socket");
		}
	}
	else
	{
		gaster_to_body = waist_to_body;
	}
	
	math::transform<float> sting_to_body;
	if (phenome.sting->present)
	{
		sting_to_body = gaster_to_body * get_bone_transform(gaster_skeleton, "sting_socket");
	}
	
	math::transform<float> eye_l_to_body;
	math::transform<float> eye_r_to_body;
	if (phenome.eyes->present)
	{
		eye_l_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "eye_socket_l");
		eye_r_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "eye_socket_r");
	}
	
	math::transform<float> ocellus_l_to_body;
	math::transform<float> ocellus_r_to_body;
	math::transform<float> ocellus_m_to_body;
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		ocellus_l_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "ocellus_socket_l");
		ocellus_r_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "ocellus_socket_r");
		ocellus_m_to_body = rest_pose.get_absolute_transform(bones.head) * get_bone_transform(head_skeleton, "ocellus_socket_m");
	}
	
	// Build legs vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> legs_reskin_map
	{
		{*legs_skeleton.get_bone_index("procoxa_l"),        {bones.procoxa_l,    &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("profemur_l"),       {bones.profemur_l,   &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protibia_l"),       {bones.protibia_l,   &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere1_l"),  {bones.protarsomere1_l,  &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere2_l"),  {bones.protarsomere1_l,  &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere3_l"),  {bones.protarsomere1_l,  &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere4_l"),  {bones.protarsomere1_l,  &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere5_l"),  {bones.protarsomere1_l,  &procoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("procoxa_r"),        {bones.procoxa_r,    &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("profemur_r"),       {bones.profemur_r,   &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protibia_r"),       {bones.protibia_r,   &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere1_r"),  {bones.protarsomere1_r,  &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere2_r"),  {bones.protarsomere1_r,  &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere3_r"),  {bones.protarsomere1_r,  &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere4_r"),  {bones.protarsomere1_r,  &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("protarsomere5_r"),  {bones.protarsomere1_r,  &procoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesocoxa_l"),       {bones.mesocoxa_l,   &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesofemur_l"),      {bones.mesofemur_l,  &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotibia_l"),      {bones.mesotibia_l,  &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere1_l"), {bones.mesotarsomere1_l, &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere2_l"), {bones.mesotarsomere1_l, &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere3_l"), {bones.mesotarsomere1_l, &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere4_l"), {bones.mesotarsomere1_l, &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere5_l"), {bones.mesotarsomere1_l, &mesocoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("mesocoxa_r"),       {bones.mesocoxa_r,   &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesofemur_r"),      {bones.mesofemur_r,  &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotibia_r"),      {bones.mesotibia_r,  &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere1_r"), {bones.mesotarsomere1_r, &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere2_r"), {bones.mesotarsomere1_r, &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere3_r"), {bones.mesotarsomere1_r, &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere4_r"), {bones.mesotarsomere1_r, &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("mesotarsomere5_r"), {bones.mesotarsomere1_r, &mesocoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metacoxa_l"),       {bones.metacoxa_l,   &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metafemur_l"),      {bones.metafemur_l,  &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatibia_l"),      {bones.metatibia_l,  &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere1_l"), {bones.metatarsomere1_l, &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere2_l"), {bones.metatarsomere1_l, &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere3_l"), {bones.metatarsomere1_l, &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere4_l"), {bones.metatarsomere1_l, &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere5_l"), {bones.metatarsomere1_l, &metacoxa_l_to_body}},
		{*legs_skeleton.get_bone_index("metacoxa_r"),       {bones.metacoxa_r,   &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metafemur_r"),      {bones.metafemur_r,  &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatibia_r"),      {bones.metatibia_r,  &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere1_r"), {bones.metatarsomere1_r, &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere2_r"), {bones.metatarsomere1_r, &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere3_r"), {bones.metatarsomere1_r, &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere4_r"), {bones.metatarsomere1_r, &metacoxa_r_to_body}},
		{*legs_skeleton.get_bone_index("metatarsomere5_r"), {bones.metatarsomere1_r, &metacoxa_r_to_body}}
	};
	
	// Build head vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> head_reskin_map
	{
		{*head_skeleton.get_bone_index("head"), {bones.head, &head_to_body}}
	};
	
	// Build mandibles vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> mandibles_reskin_map
	{
		{*mandibles_skeleton.get_bone_index("mandible_l"), {bones.mandible_l, &mandible_l_to_body}},
		{*mandibles_skeleton.get_bone_index("mandible_r"), {bones.mandible_r, &mandible_r_to_body}}
	};
	
	// Build antennae vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> antennae_reskin_map
	{
		{*antennae_skeleton.get_bone_index("antennomere1_l"), {bones.antennomere1_l, &antenna_l_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere2_l"), {bones.antennomere2_l, &antenna_l_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere1_r"), {bones.antennomere1_r, &antenna_r_to_body}},
		{*antennae_skeleton.get_bone_index("antennomere2_r"), {bones.antennomere2_r, &antenna_r_to_body}}
	};
	for (std::uint8_t i = 3; i <= phenome.antennae->total_antennomere_count; ++i)
	{
		const std::string antennomere_l_name = std::format("antennomere{}_l", i);
		const std::string antennomere_r_name = std::format("antennomere{}_r", i);
		
		const hash::fnv1a32_t antennomere_l_key = hash::fnv1a32<char>(antennomere_l_name);
		const hash::fnv1a32_t antennomere_r_key = hash::fnv1a32<char>(antennomere_r_name);
		
		antennae_reskin_map.emplace(*antennae_skeleton.get_bone_index(antennomere_l_key), std::tuple(bones.antennomere2_l, &antenna_l_to_body));
		antennae_reskin_map.emplace(*antennae_skeleton.get_bone_index(antennomere_r_key), std::tuple(bones.antennomere2_r, &antenna_r_to_body));
	}
	
	// Build waist vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> waist_reskin_map;
	if (phenome.waist->present)
	{
		waist_reskin_map.emplace(*waist_skeleton.get_bone_index("petiole"), std::tuple(*bones.petiole, &waist_to_body));
		
		if (phenome.waist->postpetiole_present)
		{
			waist_reskin_map.emplace(*waist_skeleton.get_bone_index("postpetiole"), std::tuple(*bones.postpetiole, &waist_to_body));
		}
	}
	
	// Build gaster vertex reskin map
	const std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> gaster_reskin_map
	{
		{*gaster_skeleton.get_bone_index("gaster"), {bones.gaster, &gaster_to_body}}
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
		eyes_reskin_map.emplace(*eyes_skeleton->get_bone_index("eye_l"), std::tuple(bones.head, &eye_l_to_body));
		eyes_reskin_map.emplace(*eyes_skeleton->get_bone_index("eye_r"), std::tuple(bones.head, &eye_r_to_body));
	}
	
	// Build ocelli vertex reskin map
	std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> ocelli_reskin_map;
	if (phenome.ocelli->lateral_ocelli_present)
	{
		ocelli_reskin_map.emplace(*ocelli_skeleton->get_bone_index("ocellus_l"), std::tuple(bones.head, &ocellus_l_to_body));
		ocelli_reskin_map.emplace(*ocelli_skeleton->get_bone_index("ocellus_r"), std::tuple(bones.head, &ocellus_r_to_body));
		ocelli_reskin_map.emplace(*ocelli_skeleton->get_bone_index("ocellus_m"), std::tuple(bones.head, &ocellus_m_to_body));
	}
	
	// Reskin legs vertices
	reskin_vertices(vertex_buffer_data.data() + legs_vbo_offset, legs_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), legs_reskin_map);
	
	// Reskin head vertices
	reskin_vertices(vertex_buffer_data.data() + head_vbo_offset, head_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), head_reskin_map);
	
	// Reskin mandibles vertices
	reskin_vertices(vertex_buffer_data.data() + mandibles_vbo_offset, mandibles_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), mandibles_reskin_map);
	
	// Reskin antennae vertices
	reskin_vertices(vertex_buffer_data.data() + antennae_vbo_offset, antennae_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), antennae_reskin_map);
	
	// Reskin waist vertices
	if (phenome.waist->present)
	{
		reskin_vertices(vertex_buffer_data.data() + waist_vbo_offset, waist_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), waist_reskin_map);
	}
	
	// Reskin gaster vertices
	reskin_vertices(vertex_buffer_data.data() + gaster_vbo_offset, gaster_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), gaster_reskin_map);
	
	// Reskin sting vertices
	if (phenome.sting->present)
	{
		reskin_vertices(vertex_buffer_data.data() + sting_vbo_offset, sting_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), sting_reskin_map);
	}
	
	// Reskin eyes vertices
	if (phenome.eyes->present)
	{
		reskin_vertices(vertex_buffer_data.data() + eyes_vbo_offset, eyes_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), eyes_reskin_map);
	}
	
	// Reskin ocelli vertices
	if (phenome.ocelli->lateral_ocelli_present || phenome.ocelli->median_ocellus_present)
	{
		reskin_vertices(vertex_buffer_data.data() + ocelli_vbo_offset, ocelli_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), ocelli_reskin_map);
	}
	
	// Reskin wings vertices
	if (phenome.wings->present)
	{
		const auto forewing_l_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "forewing_socket_l");
		const auto forewing_r_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "forewing_socket_r");
		const auto hindwing_l_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "hindwing_socket_l");
		const auto hindwing_r_to_body = rest_pose.get_absolute_transform(bones.mesosoma) * get_bone_transform(mesosoma_skeleton, "hindwing_socket_r");
		
		std::unordered_map<bone_index_type, std::tuple<bone_index_type, const math::transform<float>*>> wings_reskin_map;
		wings_reskin_map.emplace(*wings_skeleton->get_bone_index("forewing_l"), std::tuple(*bones.forewing_l, &forewing_l_to_body));
		wings_reskin_map.emplace(*wings_skeleton->get_bone_index("forewing_r"), std::tuple(*bones.forewing_r, &forewing_r_to_body));
		wings_reskin_map.emplace(*wings_skeleton->get_bone_index("hindwing_l"), std::tuple(*bones.hindwing_l, &hindwing_l_to_body));
		wings_reskin_map.emplace(*wings_skeleton->get_bone_index("hindwing_r"), std::tuple(*bones.hindwing_r, &hindwing_r_to_body));
		
		reskin_vertices(vertex_buffer_data.data() + wings_vbo_offset, wings_vertex_count, *position_attribute, *normal_attribute, *tangent_attribute, *bone_index_attribute, model->get_vertex_stride(), wings_reskin_map);
	}
	
	// Tag eye vertices
	if (phenome.eyes->present)
	{
		tag_vertices({vertex_buffer_data.data() + eyes_vbo_offset, vertex_buffer_data.data() + eyes_vbo_offset + eyes_vertex_count}, *bone_index_attribute, model->get_vertex_stride(), 1);
	}
	
	// Construct model VBO
	auto& model_vbo = model->get_vertex_buffer();
	model_vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_buffer_data);
	
	// Allocate model groups
	model->get_groups().resize(phenome.wings->present ? 2 : 1);
	
	// Calculate UV area of a single eye
	float eye_uv_area = 0.0f;
	if (phenome.eyes->present)
	{
		eye_uv_area = calculate_uv_area({vertex_buffer_data.data() + eyes_vbo_offset, vertex_buffer_data.data() + eyes_vbo_offset + eyes_vertex_count / 2}, *uv_attribute, model->get_vertex_stride());
	}
	
	// Generate exoskeleton material
	std::shared_ptr<render::material> exoskeleton_material = generate_ant_exoskeleton_material(phenome, eye_uv_area);
	
	// Construct model group
	render::model_group& model_group = model->get_groups()[0];
	model_group.id = "exoskeleton";
	model_group.material = exoskeleton_material;
	model_group.primitive_topology = gl::primitive_topology::triangle_list;
	model_group.first_vertex = 0;
	model_group.vertex_count = mesosoma_vertex_count +
		legs_vertex_count +
		head_vertex_count +
		mandibles_vertex_count +
		antennae_vertex_count +
		waist_vertex_count +
		gaster_vertex_count +
		sting_vertex_count +
		eyes_vertex_count +
		ocelli_vertex_count;
	
	if (phenome.wings->present)
	{
		// Construct wings model group
		render::model_group& wings_group = model->get_groups()[1];
		wings_group.id = "wings";
		wings_group.material = wings_model->get_groups().front().material;
		wings_group.primitive_topology = gl::primitive_topology::triangle_list;
		wings_group.first_vertex = model_group.vertex_count;
		wings_group.vertex_count = wings_vertex_count;
	}
	
	// Calculate model bounding box
	model->get_bounds() = calculate_bounds(vertex_buffer_data.data(), model_group.vertex_count, *position_attribute, model->get_vertex_stride());
	
	return model;
}
