// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/pass.hpp>
#include <engine/render/material.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/texture.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/sized-types.hpp>
#include <functional>
#include <span>
#include <unordered_map>

namespace engine::render
{
	/// Renders scene objects using their material-specified shaders and properties.
	class material_pass: public pass
	{
	public:
		/// Constructs a material pass.
		material_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resources::resource_manager* resource_manager);
	
		/// Destructs a material pass.
		~material_pass() override = default;
	
		void render(render::context& ctx) override;
	
		/// Sets the material to be used when a render operation is missing a material. If no fallback material is specified, render operations without materials will not be processed.
		void set_fallback_material(std::shared_ptr<render::material> fallback);
	
		inline void set_mouse_position(const math::fvec2& position)
		{
			mouse_position = position;
		}
	
	private:
		struct shader_cache_entry
		{
			std::unique_ptr<gl::shader_program> shader_program;
		
			/// Command buffer which enables the shader and updates render state-related shader variables.
			std::vector<std::function<void()>> shader_command_buffer;
		
			/// Command buffer which updates geometry-related shader variables.
			std::vector<std::function<void()>> geometry_command_buffer;
		
			/// Map of materials to command buffers which update corresponding material shader variables.
			std::unordered_map<const material*, std::vector<std::function<void()>>> material_command_buffers;
		};
	
		/// Map of state hashes to shader cache entries.
		std::unordered_map<usize, shader_cache_entry> shader_cache;
	
		/// Evaluates the active camera and stores camera information in local variables to be passed to shaders.
		void evaluate_camera(const render::context& ctx);
	
		/// Evaluates scene lights and stores lighting information in local variables to be passed to shaders.
		void evaluate_lighting(const render::context& ctx, u32 layer_mask);
	
		void evaluate_misc(const render::context& ctx);
	
		[[nodiscard]] std::unique_ptr<gl::shader_program> generate_shader_program(const gl::shader_template& shader_template, material_blend_mode blend_mode) const;
	
		void build_shader_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const;
		void build_geometry_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const;
		void build_material_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program, const material& material) const;
	
		// Camera
		const math::fmat4* view{};
		const math::fmat4* inv_view{};
		const math::fmat4* projection{};
		const math::fmat4* view_projection{};
		math::fvec4 view_translation{};
		math::fmat4 view_rotation{};
		math::fmat4 model_view{};
		const math::fvec3* camera_position{};
		float camera_exposure{};
	
		// Light probes
		const gl::texture_cube* light_probe_luminance_texture{};
		const gl::texture_1d* light_probe_illuminance_texture{};
		usize light_probe_count{};
	
		// Point lights
		std::vector<math::fvec3> point_light_colors;
		std::vector<math::fvec3> point_light_positions;
		usize point_light_count{};
	
		// Directional lights
		std::vector<math::fvec3> directional_light_colors;
		std::vector<math::fvec3> directional_light_directions;
		usize directional_light_count{};
	
		// Directional shadows
		std::vector<const gl::texture_2d*> directional_shadow_maps;
		std::vector<math::fvec4> directional_shadow_splits;
		std::vector<float> directional_shadow_fade_ranges;
		std::vector<std::span<const math::fmat4>> directional_shadow_matrices;
		usize directional_shadow_count{};
	
		// Spot lights
		std::vector<math::fvec3> spot_light_colors;
		std::vector<math::fvec3> spot_light_positions;
		std::vector<math::fvec3> spot_light_directions;
		std::vector<math::fvec2> spot_light_cutoffs;
		usize spot_light_count{};
	
		// Rectangle lights
		std::vector<math::fvec3> rectangle_light_colors;
		std::vector<math::fvec3> rectangle_light_corners;
		usize rectangle_light_count{};
	
		// LTC
		std::shared_ptr<gl::texture_2d> ltc_lut_1;
		std::shared_ptr<gl::texture_2d> ltc_lut_2;
	
		// IBL
		std::shared_ptr<gl::texture_2d> brdf_lut;
	
		// Misc
		float time{};
		float timestep{};
		unsigned int frame{0};
		float subframe{};
		math::fvec2 resolution{};
		math::fvec2 mouse_position{};
	
		// Geometry
		const math::fmat4* model{};
		std::span<const math::fmat4> skinning_matrices;
	
		/// Hash of the lighting state.
		usize lighting_state_hash{};
	
		std::shared_ptr<render::material> fallback_material;
	};
}
