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

#include "renderer/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "resources/string-table.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute-type.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/render-context.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"
#include "color/color.hpp"
#include "astro/illuminance.hpp"
#include "math/interpolation.hpp"
#include "geom/cartesian.hpp"
#include "geom/spherical.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/light/photometry.hpp"
#include <cmath>
#include <stdexcept>
#include <glad/glad.h>
#include <iostream>

sky_pass::sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	mouse_position({0.0f, 0.0f}),
	sky_model(nullptr),
	sky_material(nullptr),
	sky_model_vao(nullptr),
	sky_shader_program(nullptr),
	moon_model(nullptr),
	moon_material(nullptr),
	moon_model_vao(nullptr),
	moon_shader_program(nullptr),
	time_tween(nullptr),
	observer_altitude_tween(0.0f, math::lerp<float, float>),
	sun_position_tween(float3{1.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	sun_color_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	topocentric_frame_translation({0, 0, 0}, math::lerp<float3, float>),
	topocentric_frame_rotation(math::quaternion<float>::identity(), math::nlerp<float>)
{
	// Load star catalog
	string_table* star_catalog = resource_manager->load<string_table>("stars.csv");
	
	// Allocate star catalog vertex data
	star_count = 0;
	if (star_catalog->size() > 0)
		star_count = star_catalog->size() - 1;
	std::size_t star_vertex_size = 6;
	std::size_t star_vertex_stride = star_vertex_size * sizeof(float);
	float* star_vertex_data = new float[star_count * star_vertex_size];
	float* star_vertex = star_vertex_data;
	
	// Build star catalog vertex data
	for (std::size_t i = 1; i < star_catalog->size(); ++i)
	{
		const string_table_row& catalog_row = (*star_catalog)[i];
		
		double ra = 0.0;
		double dec = 0.0;
		double vmag = 0.0;
		double bv_color = 0.0;
		
		// Parse star catalog entry
		try
		{
			ra = std::stod(catalog_row[1]);
			dec = std::stod(catalog_row[2]);
			vmag = std::stod(catalog_row[3]);
			bv_color = std::stod(catalog_row[4]);
		}
		catch (const std::exception& e)
		{
			continue;
		}
		
		// Convert right ascension and declination from degrees to radians
		ra = math::wrap_radians(math::radians(ra));
		dec = math::wrap_radians(math::radians(dec));
		
		// Transform spherical equatorial coordinates to rectangular equatorial coordinates
		double3 position_bci = geom::spherical::to_cartesian(double3{1.0, dec, ra});
		
		// Transform coordinates from equatorial space to inertial space
		physics::frame<double> bci_to_inertial = physics::orbit::inertial::to_bci({0, 0, 0}, 0.0, math::radians(23.4393)).inverse();
		double3 position_inertial = bci_to_inertial * position_bci;
		
		// Convert color index to color temperature
		double cct = color::index::bv_to_cct(bv_color);
		
		// Calculate XYZ color from color temperature
		double3 color_xyz = color::cct::to_xyz(cct);
		
		// Transform XYZ color to ACEScg colorspace
		double3 color_acescg = color::xyz::to_acescg(color_xyz);
		
		// Convert apparent magnitude to irradiance W/m2
		double vmag_lux = astro::vmag_to_lux(vmag);
		
		// Convert irradiance to illuminance (using luminous efficiency of sun)
		double illuminance = physics::light::watts_to_lumens<double>(vmag_lux, 0.13);
		
		// Scale color by illuminance
		double3 scaled_color = color_acescg * illuminance;
		
		// Build vertex
		*(star_vertex++) = static_cast<float>(position_inertial.x);
		*(star_vertex++) = static_cast<float>(position_inertial.y);
		*(star_vertex++) = static_cast<float>(position_inertial.z);
		*(star_vertex++) = static_cast<float>(scaled_color.x);
		*(star_vertex++) = static_cast<float>(scaled_color.y);
		*(star_vertex++) = static_cast<float>(scaled_color.z);
	}
	
	// Unload star catalog
	resource_manager->unload("stars.csv");
	
	// Create star catalog VBO
	star_catalog_vbo = new gl::vertex_buffer(star_count * star_vertex_stride, star_vertex_data);
	
	// Create star catalog VAO
	star_catalog_vao = new gl::vertex_array();
	
	// Bind star catalog vertex attributes
	std::size_t vao_offset = 0;
	star_catalog_vao->bind_attribute(VERTEX_POSITION_LOCATION, *star_catalog_vbo, 3, gl::vertex_attribute_type::float_32, star_vertex_stride, 0);
	vao_offset += 3;
	star_catalog_vao->bind_attribute(VERTEX_COLOR_LOCATION, *star_catalog_vbo, 3, gl::vertex_attribute_type::float_32, star_vertex_stride, sizeof(float) * vao_offset);
	
	// Free star catalog vertex data
	delete[] star_vertex_data;
	
	// Load star shader
	star_shader_program = resource_manager->load<gl::shader_program>("star.glsl");
	star_model_view_input = star_shader_program->get_input("model_view");
	star_projection_input = star_shader_program->get_input("projection");
	star_distance_input = star_shader_program->get_input("star_distance");
	star_exposure_input = star_shader_program->get_input("camera.exposure");
}

sky_pass::~sky_pass()
{}

void sky_pass::render(render_context* context) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float time = (*time_tween)[context->alpha];
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const scene::camera& camera = *context->camera;
	float clip_near = camera.get_clip_near_tween().interpolate(context->alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(context->alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::identity4x4<float>, model_scale);
	float4x4 view = math::resize<4, 4>(math::resize<3, 3>(camera.get_view_tween().interpolate(context->alpha)));
	float4x4 model_view = view * model;
	float4x4 projection = camera.get_projection_tween().interpolate(context->alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection = projection * model_view;
	float exposure = std::exp2(camera.get_exposure_tween().interpolate(context->alpha));
	
	// Interpolate observer altitude
	float observer_altitude = observer_altitude_tween.interpolate(context->alpha);
	
	// Construct tweened inertial to topocentric frame
	physics::frame<float> topocentric_frame =
	{
		topocentric_frame_translation.interpolate(context->alpha),
		topocentric_frame_rotation.interpolate(context->alpha)
	};
	
	// Get topocentric space direction to sun
	float3 sun_position = sun_position_tween.interpolate(context->alpha);
	float3 sun_direction = math::normalize(sun_position);
	
	// Interpolate sun color
	float3 sun_color = sun_color_tween.interpolate(context->alpha);
	
	// Draw sky model
	{
		rasterizer->use_program(*sky_shader_program);

		// Upload shader parameters
		if (model_view_projection_input)
			model_view_projection_input->upload(model_view_projection);
		if (mouse_input)
			mouse_input->upload(mouse_position);
		if (resolution_input)
			resolution_input->upload(resolution);
		if (time_input)
			time_input->upload(time);
		if (exposure_input)
			exposure_input->upload(exposure);
		
		if (observer_altitude_input)
			observer_altitude_input->upload(observer_altitude);
		if (sun_direction_input)
			sun_direction_input->upload(sun_direction);
		if (cos_sun_angular_radius_input)
			cos_sun_angular_radius_input->upload(cos_sun_angular_radius);
		if (sun_color_input)
			sun_color_input->upload(sun_color);
		if (scale_height_rm_input)
			scale_height_rm_input->upload(scale_height_rm);
		if (rayleigh_scattering_input)
			rayleigh_scattering_input->upload(rayleigh_scattering);
		if (mie_scattering_input)
			mie_scattering_input->upload(mie_scattering);
		if (mie_asymmetry_input)
			mie_asymmetry_input->upload(mie_asymmetry);
		if (atmosphere_radii_input)
			atmosphere_radii_input->upload(atmosphere_radii);
		
		sky_material->upload(context->alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_ONE, GL_ONE);
	
	// Draw moon model
	/*
	float3 moon_position = {0, 0, 0};
	if (moon_position.y >= -moon_angular_radius)
	{

		
		float moon_distance = (clip_near + clip_far) * 0.5f;		
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = moon_position * -moon_distance;
		moon_transform.rotation = math::quaternion<float>::identity();
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = math::matrix_cast(moon_transform);		
		model_view = view * model;
		model_view_projection = projection * model_view;
		float3x3 normal_model = math::transpose(math::inverse(math::resize<3, 3>(model)));
		
		rasterizer->use_program(*moon_shader_program);
		if (moon_model_view_projection_input)
			moon_model_view_projection_input->upload(model_view_projection);
		if (moon_normal_model_input)
			moon_normal_model_input->upload(normal_model);
		if (moon_moon_position_input)
			moon_moon_position_input->upload(moon_position);
		if (moon_sun_position_input)
			moon_sun_position_input->upload(sun_position);
		moon_material->upload(context->alpha);
		rasterizer->draw_arrays(*moon_model_vao, moon_model_drawing_mode, moon_model_start_index, moon_model_index_count);
	}
	*/
	
	// Draw stars
	{
		float star_distance = (clip_near + clip_far) * 0.5f;
		
		model = math::resize<4, 4>(math::matrix_cast<float>(topocentric_frame.rotation));
		model = math::scale(model, {star_distance, star_distance, star_distance});
		
		model_view = view * model;
		
		rasterizer->use_program(*star_shader_program);
		if (star_model_view_input)
			star_model_view_input->upload(model_view);
		if (star_projection_input)
			star_projection_input->upload(projection);
		if (star_distance_input)
			star_distance_input->upload(star_distance);
		if (star_exposure_input)
			star_exposure_input->upload(exposure);
		
		rasterizer->draw_arrays(*star_catalog_vao, gl::drawing_mode::points, 0, star_count);
	}
}

void sky_pass::set_sky_model(const model* model)
{
	sky_model = model;
	
	if (sky_model)
	{
		sky_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			sky_material = group->get_material();
			sky_model_drawing_mode = group->get_drawing_mode();
			sky_model_start_index = group->get_start_index();
			sky_model_index_count = group->get_index_count();
		}
		
		if (sky_material)
		{
			sky_shader_program = sky_material->get_shader_program();
			
			if (sky_shader_program)
			{
				model_view_projection_input = sky_shader_program->get_input("model_view_projection");
				mouse_input = sky_shader_program->get_input("mouse");
				resolution_input = sky_shader_program->get_input("resolution");
				time_input = sky_shader_program->get_input("time");
				exposure_input = sky_shader_program->get_input("camera.exposure");

				observer_altitude_input = sky_shader_program->get_input("observer_altitude");
				sun_direction_input = sky_shader_program->get_input("sun_direction");
				sun_color_input = sky_shader_program->get_input("sun_color");
				cos_sun_angular_radius_input = sky_shader_program->get_input("cos_sun_angular_radius");
				scale_height_rm_input = sky_shader_program->get_input("scale_height_rm");
				rayleigh_scattering_input = sky_shader_program->get_input("rayleigh_scattering");
				mie_scattering_input = sky_shader_program->get_input("mie_scattering");
				mie_asymmetry_input = sky_shader_program->get_input("mie_asymmetry");
				atmosphere_radii_input = sky_shader_program->get_input("atmosphere_radii");
			}
		}
	}
	else
	{
		sky_model_vao = nullptr;
	}
}

void sky_pass::set_moon_model(const model* model)
{
	moon_model = model;
	
	if (moon_model)
	{
		moon_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			moon_material = group->get_material();
			moon_model_drawing_mode = group->get_drawing_mode();
			moon_model_start_index = group->get_start_index();
			moon_model_index_count = group->get_index_count();
		}
		
		if (moon_material)
		{
			moon_shader_program = moon_material->get_shader_program();
			
			if (moon_shader_program)
			{
				moon_model_view_projection_input = moon_shader_program->get_input("model_view_projection");
				moon_normal_model_input = moon_shader_program->get_input("normal_model");
				moon_moon_position_input = moon_shader_program->get_input("moon_position");
				moon_sun_position_input = moon_shader_program->get_input("sun_position");
			}
		}
	}
	else
	{
		moon_model = nullptr;
	}
}

void sky_pass::update_tweens()
{
	observer_altitude_tween.update();
	sun_position_tween.update();
	sun_color_tween.update();
	topocentric_frame_translation.update();
	topocentric_frame_rotation.update();
}

void sky_pass::set_time_tween(const tween<double>* time)
{
	this->time_tween = time;
}

void sky_pass::set_topocentric_frame(const physics::frame<float>& frame)
{
	topocentric_frame_translation[1] = frame.translation;
	topocentric_frame_rotation[1] = frame.rotation;
}

void sky_pass::set_sun_position(const float3& position)
{
	sun_position_tween[1] = position;
}

void sky_pass::set_sun_color(const float3& color)
{
	sun_color_tween[1] = color;
}

void sky_pass::set_sun_angular_radius(float radius)
{
	cos_sun_angular_radius = std::cos(radius);
}

void sky_pass::set_observer_altitude(float altitude)
{
	observer_altitude_tween[1] = altitude;
}

void sky_pass::set_scale_heights(float rayleigh, float mie)
{
	scale_height_rm = {rayleigh, mie};
}

void sky_pass::set_scattering_coefficients(const float3& r, const float3& m)
{
	rayleigh_scattering = r;
	mie_scattering = m;
}

void sky_pass::set_mie_asymmetry(float g)
{
	mie_asymmetry = {g, g * g};
}

void sky_pass::set_atmosphere_radii(float inner, float outer)
{
	atmosphere_radii.x = inner;
	atmosphere_radii.y = outer;
	atmosphere_radii.z = outer * outer;
}

void sky_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}
