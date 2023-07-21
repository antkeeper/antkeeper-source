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

#ifndef ANTKEEPER_RENDER_SKY_PASS_HPP
#define ANTKEEPER_RENDER_SKY_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/math/vector.hpp>
#include <engine/animation/tween.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/math/se3.hpp>
#include <engine/scene/object.hpp>
#include <engine/scene/light-probe.hpp>

class resource_manager;

namespace render {

class material;
class model;

/**
 *
 *
 * @see Hillaire, Sébastien. "A scalable and production ready sky and atmosphere rendering technique." Computer Graphics Forum. Vol. 39. No. 4. 2020.
 */
class sky_pass: public pass
{
public:
	sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~sky_pass() = default;
	void render(render::context& ctx) override;
	
	/// @name Transmittance LUT
	/// @{
	
	/**
	 * Sets the number of transmittance integration samples.
	 *
	 * @param count Integration sample count.
	 *
	 * @note Triggers rebuilding of transmittance LUT shader.
	 * @note Triggers rendering of transmittance LUT.
	 */
	void set_transmittance_lut_sample_count(std::uint16_t count);
	
	/**
	 * Sets the resolution of the transmittance LUT.
	 *
	 * @param resolution Resolution of the transmittance LUT texture, in pixels.
	 *
	 * @note Triggers rendering of transmittance LUT.
	 */
	void set_transmittance_lut_resolution(const math::vec2<std::uint16_t>& resolution);
	
	/// Returns the number of transmittance integration samples.
	[[nodiscard]] inline std::uint16_t get_transmittance_lut_sample_count() const noexcept
	{
		return m_transmittance_lut_sample_count;
	}
	
	/// Returns the resolution of the transmittance LUT texture, in pixels.
	[[nodiscard]] inline const math::vec2<std::uint16_t>& get_transmittance_lut_resolution() const noexcept
	{
		return m_transmittance_lut_resolution;
	}
	
	/// @}
	/// @name Multiscattering LUT
	/// @{
	
	/**
	 * Sets the number of multiscattering directions to sample.
	 *
	 * @param count Multiscattering direction sample count.
	 *
	 * @note Triggers rebuilding of multiscattering LUT shader.
	 * @note Triggers rendering of multiscattering LUT.
	 */
	void set_multiscattering_lut_direction_sample_count(std::uint16_t count);
	
	/**
	 * Sets the number of multiscattering scatter events to sample per sample direction.
	 *
	 * @param count Multiscattering scatter sample count.
	 *
	 * @note Triggers rebuilding of multiscattering LUT shader.
	 * @note Triggers rendering of multiscattering LUT.
	 */
	void set_multiscattering_lut_scatter_sample_count(std::uint16_t count);
	
	/**
	 * Sets the resolution of the multiscattering LUT.
	 *
	 * @param resolution Resolution of the multiscattering LUT texture, in pixels.
	 *
	 * @note Triggers rendering of multiscattering LUT.
	 */
	void set_multiscattering_lut_resolution(const math::vec2<std::uint16_t>& resolution);
	
	/// Returns the number of multiscattering direction samples.
	[[nodiscard]] inline std::uint16_t get_multiscattering_lut_direction_sample_count() const noexcept
	{
		return m_multiscattering_lut_direction_sample_count;
	}
	
	/// Returns the number of multiscattering scatter samples per direction.
	[[nodiscard]] inline std::uint16_t get_multiscattering_lut_scatter_sample_count() const noexcept
	{
		return m_multiscattering_lut_scatter_sample_count;
	}
	
	/// Returns the resolution of the multiscattering LUT texture, in pixels.
	[[nodiscard]] inline const math::vec2<std::uint16_t>& get_multiscattering_lut_resolution() const noexcept
	{
		return m_multiscattering_lut_resolution;
	}
	
	/// @}
	/// @name Luminance LUT
	/// @{
	
	/**
	 * Sets the number of luminance integration samples.
	 *
	 * @param count Integration sample count.
	 *
	 * @note Triggers rebuilding of luminance LUT shader.
	 * @note Triggers rendering of luminance LUT.
	 */
	void set_luminance_lut_sample_count(std::uint16_t count);
	
	/**
	 * Sets the resolution of the luminance LUT.
	 *
	 * @param resolution Resolution of the luminance LUT texture, in pixels.
	 *
	 * @note Triggers rendering of luminance LUT.
	 */
	void set_luminance_lut_resolution(const math::vec2<std::uint16_t>& resolution);
	
	/// Returns the number of luminance integration samples.
	[[nodiscard]] inline std::uint16_t get_luminance_lut_sample_count() const noexcept
	{
		return m_luminance_lut_sample_count;
	}
	
	/// Returns the resolution of the luminance LUT texture, in pixels.
	[[nodiscard]] inline const math::vec2<std::uint16_t>& get_luminance_lut_resolution() const noexcept
	{
		return m_luminance_lut_resolution;
	}
	
	/// @}
	
	void update_tweens();
	
	void set_magnification(float scale);
	
	void set_sky_model(std::shared_ptr<render::model> model);
	void set_moon_model(std::shared_ptr<render::model> model);
	void set_stars_model(std::shared_ptr<render::model> model);
	
	void set_icrf_to_eus(const math::transformation::se3<float>& transformation);
	
	void set_sun_position(const math::fvec3& position);
	void set_sun_luminance(const math::fvec3& luminance);
	void set_sun_illuminance(const math::fvec3& illuminance, const math::fvec3& transmitted_illuminance);
	void set_sun_angular_radius(float radius);
	void set_planet_radius(float radius);
	void set_atmosphere_upper_limit(float limit);
	void set_observer_elevation(float elevation);
	void set_rayleigh_parameters(float scale_height, const math::fvec3& scattering);
	void set_mie_parameters(float scale_height, float scattering, float extinction, float anisotropy);
	void set_ozone_parameters(float lower_limit, float upper_limit, float mode, const math::fvec3& absorption);
	void set_airglow_luminance(const math::fvec3& luminance);
	void set_ground_albedo(const math::fvec3& albedo);
	
	void set_moon_position(const math::fvec3& position);
	void set_moon_rotation(const math::fquat& rotation);
	void set_moon_angular_radius(float angular_radius);
	void set_moon_sunlight_direction(const math::fvec3& direction);
	void set_moon_sunlight_illuminance(const math::fvec3& illuminance);
	void set_moon_planetlight_direction(const math::fvec3& direction);
	void set_moon_planetlight_illuminance(const math::fvec3& illuminance);
	void set_moon_illuminance(const math::fvec3& illuminance, const math::fvec3& transmitted_illuminance);
	

	
	void set_sky_probe(std::shared_ptr<scene::light_probe> probe);

private:
	void rebuild_transmittance_lut_shader_program();
	void rebuild_transmittance_lut_command_buffer();
	void rebuild_multiscattering_lut_shader_program();
	void rebuild_multiscattering_lut_command_buffer();
	void rebuild_luminance_lut_shader_program();
	void rebuild_luminance_lut_command_buffer();
	
	void rebuild_sky_lut_command_buffer();
	void rebuild_sky_probe_command_buffer();
	
	std::unique_ptr<gl::vertex_buffer> quad_vbo;
	std::unique_ptr<gl::vertex_array> quad_vao;
	
	// Transmittance
	std::uint16_t m_transmittance_lut_sample_count{40};
	math::vec2<std::uint16_t> m_transmittance_lut_resolution{256, 64};
	std::unique_ptr<gl::texture_2d> m_transmittance_lut_texture;
	std::unique_ptr<gl::framebuffer> m_transmittance_lut_framebuffer;
	std::shared_ptr<gl::shader_template> m_transmittance_lut_shader_template;
	std::unique_ptr<gl::shader_program> m_transmittance_lut_shader_program;
	std::vector<std::function<void()>> m_transmittance_lut_command_buffer;
	bool m_render_transmittance_lut{false};
	
	// Multiscattering
	std::uint16_t m_multiscattering_lut_direction_sample_count{64};
	std::uint16_t m_multiscattering_lut_scatter_sample_count{20};
	math::vec2<std::uint16_t> m_multiscattering_lut_resolution{32, 32};
	std::unique_ptr<gl::texture_2d> m_multiscattering_lut_texture;
	std::unique_ptr<gl::framebuffer> m_multiscattering_lut_framebuffer;
	std::shared_ptr<gl::shader_template> m_multiscattering_lut_shader_template;
	std::unique_ptr<gl::shader_program> m_multiscattering_lut_shader_program;
	std::vector<std::function<void()>> m_multiscattering_lut_command_buffer;
	bool m_render_multiscattering_lut{false};
	
	// Luminance
	std::uint16_t m_luminance_lut_sample_count{30};
	math::vec2<std::uint16_t> m_luminance_lut_resolution{200, 100};
	std::unique_ptr<gl::texture_2d> m_luminance_lut_texture;
	std::unique_ptr<gl::framebuffer> m_luminance_lut_framebuffer;
	std::shared_ptr<gl::shader_template> m_luminance_lut_shader_template;
	std::unique_ptr<gl::shader_program> m_luminance_lut_shader_program;
	std::vector<std::function<void()>> m_luminance_lut_command_buffer;
	bool m_render_luminance_lut{false};
	
	// Sky probe
	std::shared_ptr<scene::light_probe> m_sky_probe;
	std::vector<std::unique_ptr<gl::framebuffer>> m_sky_probe_framebuffers;
	std::shared_ptr<gl::shader_template> m_sky_probe_shader_template;
	std::unique_ptr<gl::shader_program> m_sky_probe_shader_program;
	std::vector<std::function<void()>> m_sky_probe_command_buffer;
	
	math::fvec3 dominant_light_direction;
	math::fvec3 dominant_light_illuminance;
	math::fvec3 observer_position;
	float camera_exposure;

	std::shared_ptr<gl::shader_program> sky_shader_program;
	const gl::shader_variable* model_view_projection_var;
	const gl::shader_variable* mouse_var;
	const gl::shader_variable* resolution_var;
	const gl::shader_variable* light_direction_var;
	const gl::shader_variable* sun_luminance_var;
	const gl::shader_variable* sun_angular_radius_var;
	const gl::shader_variable* atmosphere_radii_var;
	const gl::shader_variable* observer_position_var;
	const gl::shader_variable* sky_transmittance_lut_var;
	const gl::shader_variable* sky_transmittance_lut_resolution_var;
	const gl::shader_variable* sky_luminance_lut_var;
	const gl::shader_variable* sky_luminance_lut_resolution_var;
	
	std::shared_ptr<gl::shader_program> moon_shader_program;
	const gl::shader_variable* moon_model_var;
	const gl::shader_variable* moon_view_projection_var;
	const gl::shader_variable* moon_normal_model_var;
	const gl::shader_variable* moon_camera_position_var;
	const gl::shader_variable* moon_sunlight_direction_var;
	const gl::shader_variable* moon_sunlight_illuminance_var;
	const gl::shader_variable* moon_planetlight_direction_var;
	const gl::shader_variable* moon_planetlight_illuminance_var;
	const gl::shader_variable* moon_albedo_map_var;
	const gl::shader_variable* moon_normal_map_var;
	const gl::shader_variable* moon_observer_position_var;
	const gl::shader_variable* moon_sky_transmittance_lut_var;
	const gl::shader_variable* moon_atmosphere_radii_var;
	
	std::shared_ptr<render::model> sky_model;
	const material* sky_material;
	const gl::vertex_array* sky_model_vao;
	gl::drawing_mode sky_model_drawing_mode;
	std::size_t sky_model_start_index;
	std::size_t sky_model_index_count;
	
	std::shared_ptr<render::model> moon_model;
	const material* moon_material;
	const gl::vertex_array* moon_model_vao;
	gl::drawing_mode moon_model_drawing_mode;
	std::size_t moon_model_start_index;
	std::size_t moon_model_index_count;
	std::shared_ptr<gl::texture_2d> m_moon_albedo_map;
	std::shared_ptr<gl::texture_2d> m_moon_normal_map;

	
	std::shared_ptr<render::model> stars_model;
	const material* star_material;
	const gl::vertex_array* stars_model_vao;
	gl::drawing_mode stars_model_drawing_mode;
	std::size_t stars_model_start_index;
	std::size_t stars_model_index_count;
	std::unique_ptr<gl::shader_program> star_shader_program;
	const gl::shader_variable* star_model_view_projection_var;
	const gl::shader_variable* star_exposure_var;
	const gl::shader_variable* star_inv_resolution_var;

	math::fvec2 mouse_position;
	
	tween<math::fvec3> sun_position_tween;
	tween<math::fvec3> sun_luminance_tween;
	tween<math::fvec3> sun_illuminance_tween;
	math::fvec3 sun_transmitted_illuminance;
	tween<math::fvec3> icrf_to_eus_translation;
	tween<math::fquat> icrf_to_eus_rotation;
	
	tween<math::fvec3> moon_position_tween;
	tween<math::fquat> moon_rotation_tween;
	tween<float> moon_angular_radius_tween;
	tween<math::fvec3> moon_sunlight_direction_tween;
	tween<math::fvec3> moon_sunlight_illuminance_tween;
	tween<math::fvec3> moon_planetlight_direction_tween;
	tween<math::fvec3> moon_planetlight_illuminance_tween;
	tween<math::fvec3> moon_illuminance_tween;
	math::fvec3 moon_transmitted_illuminance;
	
	float sun_angular_radius;
	float atmosphere_upper_limit;
	math::fvec4 atmosphere_radii;
	float observer_elevation;
	tween<math::fvec3> observer_position_tween;
	math::fvec4 rayleigh_parameters;
	math::fvec4 mie_parameters;
	math::fvec3 ozone_distribution;
	math::fvec3 ozone_absorption;
	math::fvec3 airglow_luminance;
	math::fvec3 m_ground_albedo{};
	
	float magnification;
};

} // namespace render

#endif // ANTKEEPER_RENDER_SKY_PASS_HPP
