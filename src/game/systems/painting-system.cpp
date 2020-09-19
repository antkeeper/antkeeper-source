/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "painting-system.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/brush-component.hpp"
#include "game/components/tool-component.hpp"
#include "event/event-dispatcher.hpp"
#include "resources/resource-manager.hpp"
#include "scene/scene.hpp"
#include "scene/model-instance.hpp"
#include "math/math.hpp"
#include "renderer/material.hpp"
#include "renderer/model.hpp"
#include "utility/fundamental-types.hpp"
#include "game/entity-commands.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/transform-component.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "renderer/vertex-attributes.hpp"
#include "geometry/mesh-functions.hpp"
#include <limits>

using namespace ecs;

painting_system::painting_system(entt::registry& registry, ::event_dispatcher* event_dispatcher, ::resource_manager* resource_manager):
	entity_system(registry),
	event_dispatcher(event_dispatcher),
	resource_manager(resource_manager),
	scene(nullptr),
	painting(false)
{
	event_dispatcher->subscribe<tool_pressed_event>(this);
	event_dispatcher->subscribe<tool_released_event>(this);
	
	max_miter_angle = math::radians(135.0f);
	decal_offset = 0.01f;
	stroke_width = 1.5f;
	min_stroke_length = 1.0f;
	min_stroke_length_squared = min_stroke_length * min_stroke_length;
	max_stroke_segments = 4096;
	current_stroke_segment = 0;
	vertex_size = 13;
	vertex_stride = sizeof(float) * vertex_size;
	vertex_count = max_stroke_segments * 6;
	
	// Create stroke model
	stroke_model = new model();
	stroke_model_group = stroke_model->add_group();
	stroke_model_group->set_material(resource_manager->load<material>("brushstroke.mtl"));
	
	// Setup stroke vbo and vao
	stroke_vbo = stroke_model->get_vertex_buffer();
	stroke_vbo->repurpose(sizeof(float) * vertex_size * vertex_count, nullptr, buffer_usage::dynamic_draw);
	stroke_model->get_vertex_array()->bind_attribute(VERTEX_POSITION_LOCATION, *stroke_vbo, 4, vertex_attribute_type::float_32, vertex_stride, 0);
	stroke_model->get_vertex_array()->bind_attribute(VERTEX_NORMAL_LOCATION, *stroke_vbo, 3, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * 4);
	stroke_model->get_vertex_array()->bind_attribute(VERTEX_TEXCOORD_LOCATION, *stroke_vbo, 2, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * 7);
	stroke_model->get_vertex_array()->bind_attribute(VERTEX_TANGENT_LOCATION, *stroke_vbo, 4, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * 9);
	
	// Create stroke model instance
	stroke_model_instance = new model_instance();
	stroke_model_instance->set_model(stroke_model);
	stroke_model_instance->update_tweens();
	
	stroke_bounds_min.x = std::numeric_limits<float>::infinity();
	stroke_bounds_min.y = std::numeric_limits<float>::infinity();
	stroke_bounds_min.z = std::numeric_limits<float>::infinity();
	stroke_bounds_max.x = -std::numeric_limits<float>::infinity();
	stroke_bounds_max.y = -std::numeric_limits<float>::infinity();
	stroke_bounds_max.z = -std::numeric_limits<float>::infinity();
	
	midstroke = false;
}

painting_system::~painting_system()
{
	event_dispatcher->unsubscribe<tool_pressed_event>(this);
	event_dispatcher->unsubscribe<tool_released_event>(this);
}

void painting_system::update(double t, double dt)
{
	if (painting)
	{
		const tool_component& tool = registry.get<tool_component>(brush_entity);
		
		auto cast_result = cast_ray(tool.cursor);
		if (cast_result.has_value())
		{
			stroke_end = std::get<0>(cast_result.value());
			float3 surface_normal = std::get<1>(cast_result.value());
			
			float3 segment_difference = stroke_end - stroke_start;
			float segment_length_squared = math::dot(segment_difference, segment_difference);
			if (segment_length_squared >= min_stroke_length_squared)
			{
				float segment_length = std::sqrt(segment_length_squared);
				
				float3 segment_forward = segment_difference / segment_length;
				float3 segment_right = math::normalize(math::cross(segment_forward, surface_normal));
				float3 segment_up = math::cross(segment_right, segment_forward);
				
				float angle = std::acos(math::dot(segment_forward, float3{0, 0, -1}));
				float3 cross = math::cross(segment_forward, float3{0, 0, -1});
				if (math::dot(surface_normal, cross) < 0.0f)
					angle = -angle;
				
				math::quaternion<float> tangent_rotation = math::normalize(math::angle_axis(-angle, surface_normal));
				
				float3 p1 = stroke_start;
				float3 p2 = stroke_end;
				
				// Find miter
				float3 tangent = math::normalize(math::normalize(p2 - p1) + math::normalize(p1 - p0));
				float2 miter = float2{-tangent.z, tangent.x};
				float2 normal = float2{segment_right.x, segment_right.z};
				float miter_length = stroke_width / math::dot(miter, normal);
				
				float3 a = p0a;
				float3 b = p0b;
				float3 c = p1 - segment_right * stroke_width * 0.5f + segment_up * decal_offset;
				float3 d = p1 + segment_right * stroke_width * 0.5f + segment_up * decal_offset;
				float3 e = p2 - segment_right * stroke_width * 0.5f + segment_up * decal_offset;
				float3 f = p2 + segment_right * stroke_width * 0.5f + segment_up * decal_offset;
				
				// Adjust c and d
				bool mitered = false;
				if (midstroke)
				{
					float angle = std::acos(math::dot(math::normalize(p2 - p1), math::normalize(p1 - p0)));
					if (angle < max_miter_angle)
					{
						mitered = true;
						c = p1 - float3{miter.x, 0.0f, miter.y} * miter_length * 0.5f + segment_up * decal_offset;
						d = p1 + float3{miter.x, 0.0f, miter.y} * miter_length * 0.5f + segment_up * decal_offset;
					}
				}
				
				const float3 positions[] =
				{
					a, b, c,
					c, b, d,
					c, d, e,
					e, d, f
				};
				const float w = static_cast<float>(t);
				
				float2 texcoords[] =
				{
					{0, 0}, {1, 0}, {0, 1},
					{0, 1}, {1, 0}, {1, 1},
					{0, 0}, {1, 0}, {0, 1},
					{0, 1}, {1, 0}, {1, 1},
				};
				
				float3 tangent_positions[] =
				{
					{0, 0, 0}, {1, 0, 0}, {0, 0, 1},
					{0, 0, 1}, {1, 0, 0}, {1, 0, 1},
					{0, 0, 0}, {1, 0, 0}, {0, 0, 1},
					{0, 0, 1}, {1, 0, 0}, {1, 0, 1}
				};
				
				/// @TODO: smooth normals in middle of segment
				
				float3 tangents[12];
				float3 bitangents[12];
				for (int i = 0; i < 4; ++i)
				{
					const float3& a = tangent_positions[i * 3];
					const float3& b = tangent_positions[i * 3 + 1];
					const float3& c = tangent_positions[i * 3 + 2];
					const float2& uva = texcoords[i * 3];
					const float2& uvb = texcoords[i * 3 + 1];
					const float2& uvc = texcoords[i * 3 + 2];
					
					float3 ba = b - a;
					float3 ca = c - a;
					float2 uvba = uvb - uva;
					float2 uvca = uvc - uva;
					
					float f = 1.0f / (uvba.x * uvca.y - uvca.x * uvba.y);
					float3 tangent = math::normalize((ba * uvca.y - ca * uvba.y) * f);
					float3 bitangent = math::normalize((ba * -uvca.x + ca * uvba.x) * f);
					
					// Rotate tangent and bitangent according to segment rotation
					tangent = math::normalize(tangent_rotation * tangent);
					bitangent = math::normalize(tangent_rotation * bitangent);
					
					tangents[i * 3] = tangent;
					tangents[i * 3 + 1] = tangent;
					tangents[i * 3 + 2] = tangent;
					
					bitangents[i * 3] = bitangent;
					bitangents[i * 3 + 1] = bitangent;
					bitangents[i * 3 + 2] = bitangent;
				}
				
				float vertex_data[13 * 12];
				float* v = &vertex_data[0];
				for (int i = 0; i < 12; ++i)
				{
					*(v++) = positions[i].x;
					*(v++) = positions[i].y;
					*(v++) = positions[i].z;
					*(v++) = w;

					*(v++) = surface_normal.x;
					*(v++) = surface_normal.y;
					*(v++) = surface_normal.z;
					
					*(v++) = texcoords[i].x;
					*(v++) = texcoords[i].y;
					
					*(v++) = tangents[i].x;
					*(v++) = tangents[i].y;
					*(v++) = tangents[i].z;
					*(v++) = 0.0f;
				}
				
				std::size_t segment_size = sizeof(float) * vertex_size * 6;
				if (mitered)
				{
					stroke_vbo->update((current_stroke_segment - 1) * segment_size, segment_size * 2, &vertex_data[0]);
				}
				else
				{
					stroke_vbo->update(current_stroke_segment * segment_size, segment_size, &vertex_data[vertex_size * 6]);
				}
				
				++current_stroke_segment;
				
				stroke_model_group->set_index_count(current_stroke_segment * 6);
				
				// Update stroke bounds
				stroke_bounds_min.x = std::min<float>(stroke_bounds_min.x, std::min<float>(c.x, std::min<float>(d.x, std::min<float>(e.x, f.x))));
				stroke_bounds_min.y = std::min<float>(stroke_bounds_min.y, std::min<float>(c.y, std::min<float>(d.y, std::min<float>(e.y, f.y))));
				stroke_bounds_min.z = std::min<float>(stroke_bounds_min.z, std::min<float>(c.z, std::min<float>(d.z, std::min<float>(e.z, f.z))));
				stroke_bounds_max.x = std::max<float>(stroke_bounds_max.x, std::max<float>(c.x, std::max<float>(d.x, std::max<float>(e.x, f.x))));
				stroke_bounds_max.y = std::max<float>(stroke_bounds_max.y, std::max<float>(c.y, std::max<float>(d.y, std::max<float>(e.y, f.y))));
				stroke_bounds_max.z = std::max<float>(stroke_bounds_max.z, std::max<float>(c.z, std::max<float>(d.z, std::max<float>(e.z, f.z))));
				stroke_model->set_bounds(aabb<float>{stroke_bounds_min, stroke_bounds_max});
				stroke_model_instance->update_bounds();
				
				p0 = stroke_start;
				p0a = c;
				p0b = d;
				stroke_start = stroke_end;
				midstroke = true;
			}
		}
	}
}

void painting_system::set_scene(::scene* scene)
{
	this->scene = scene;
	scene->add_object(stroke_model_instance);
}

void painting_system::handle_event(const tool_pressed_event& event)
{
	if (registry.has<brush_component>(event.entity))
	{
		auto cast_result = cast_ray(event.position);
		
		if (cast_result.has_value())
		{
			brush_entity = event.entity;
			painting = true;
			stroke_start = std::get<0>(cast_result.value());
			stroke_end = stroke_start;
			p0 = stroke_start;
			p0a = p0;
			p0b = p0;
			midstroke = false;
		}
	}
}

void painting_system::handle_event(const tool_released_event& event)
{
	if (registry.has<brush_component>(event.entity))
	{
		auto cast_result = cast_ray(ec::get_world_transform(registry, event.entity).translation);
		
		if (cast_result.has_value())
		{
			stroke_end = std::get<0>(cast_result.value());
		}
		
		brush_entity = entt::null;
		painting = false;
	}
}

std::optional<std::tuple<float3, float3>> painting_system::cast_ray(const float3& position) const
{
	std::optional<std::tuple<float3, float3>> result;
	
	float3 intersection;
	float3 surface_normal;
	mesh::face* face = nullptr;
	
	ray<float> untransformed_ray = {position + float3{0.0f, 10000.0f, 0.0f}, {0, -1, 0}};
	float min_distance = std::numeric_limits<float>::infinity();

	registry.view<transform_component, collision_component>().each(
		[&](auto entity, auto& collision_transform, auto& collision)
		{
			// Transform ray into local space of collision component
			math::transform<float> inverse_transform = math::inverse(collision_transform.local);
			float3 origin = inverse_transform * untransformed_ray.origin;
			float3 direction = math::normalize(math::conjugate(collision_transform.local.rotation) * untransformed_ray.direction);
			ray<float> transformed_ray = {origin, direction};

			// Broad phase AABB test
			auto aabb_result = ray_aabb_intersection(transformed_ray, collision.bounds);
			if (!std::get<0>(aabb_result))
			{
				return;
			}

			// Narrow phase mesh test
			auto mesh_result = collision.mesh_accelerator.query_nearest(transformed_ray);
			if (mesh_result)
			{
				if (mesh_result->t < min_distance)
				{
					min_distance = mesh_result->t;
					intersection = untransformed_ray.extrapolate(min_distance);
					face = mesh_result->face;
				}
			}
		});
	
	if (face != nullptr)
	{
		surface_normal = calculate_face_normal(*face);
		result = std::make_tuple(intersection, surface_normal);
	}
	
	return result;
}
