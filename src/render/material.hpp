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

#ifndef ANTKEEPER_RENDER_MATERIAL_HPP
#define ANTKEEPER_RENDER_MATERIAL_HPP

#include "render/blend-mode.hpp"
#include "render/material-property.hpp"
#include "render/shadow-mode.hpp"
#include "gl/shader-program.hpp"
#include <cstdint>
#include <cstddef>
#include <list>
#include <unordered_map>
#include <string>

namespace render {

/**
 * A material is associated with exactly one shader program and contains a set of material properties which can be uploaded to that shader program via shader inputs.
 */
class material
{
public:
	/**
	 * Creates a material.
	 *
	 * @param program Shader program with which to associate this material.
	 */
	explicit material(gl::shader_program* program);

	/**
	 * Creates a material.
	 */
	material();

	/**
	 * Creates a copy of another material.
	 *
	 * @param other Material to copy.
	 */
	material(const material& other);

	/**
	 * Destroys a material.
	 */
	~material();

	/**
	 * Makes this material a copy of aother material.
	 *
	 * @param other Material to copy.
	 * @return Reference to this material.
	 */
	material& operator=(const material& other);
	
	/**
	 * Sets state 0 = state 1 for each material property tween.
	 */
	void update_tweens();

	/**
	 * Uploads each material property to the material's shader program.
	 *
	 * @param a Interpolation factor. Should be on `[0.0, 1.0]`.
	 * @return Number of material property uploads which failed.
	 */
	std::size_t upload(double a) const;

	/**
	 * Sets the material's shader program and reconnects all shader properties to their corresponding shader inputs.
	 *
	 * @param program Shader program with which to associate the material.
	 */
	void set_shader_program(gl::shader_program* program);

	/**
	 * Sets the material flags.
	 *
	 * @param flags Material flags.
	 */
	void set_flags(std::uint32_t flags) noexcept;
	
	/**
	 * Sets the material blend mode.
	 *
	 * @param mode Blend mode.
	 */
	void set_blend_mode(blend_mode mode) noexcept;
	
	/**
	 * Sets the opacity mask threshold value for masked blend mode.
	 *
	 * @param threshold Opacity mask threshold value, above which the surface is considered opaque.
	 *
	 * @see render::blend_mode::masked
	 */
	void set_opacity_threshold(float threshold) noexcept;
	
	/**
	 * Enables or disables back-face culling of the material surface.
	 *
	 * @param two_sided `true` to disable back-face culling, or `false` to enable it.
	 */
	void set_two_sided(bool two_sided) noexcept;
	
	/**
	 * Sets the material shadow mode.
	 *
	 * @param mode Shadow mode.
	 */
	void set_shadow_mode(shadow_mode mode) noexcept;

	/**
	 * Adds a material array property to the material.
	 *
	 * @param name Name of the material array property.
	 * @param element_count Number of elements in the array.
	 * @return Pointer to the added material property.
	 */
	template <typename T>
	material_property<T>* add_property(const std::string& name, std::size_t element_count = 1);

	/**
	 * Returns the shader program with which this material is associated.
	 */
	gl::shader_program* get_shader_program() const;

	/// Returns the material flags.
	std::uint32_t get_flags() const noexcept;
	
	/// Returns the material blend mode.
	blend_mode get_blend_mode() const noexcept;
	
	/// Returns the opacity mask threshold value.
	float get_opacity_threshold() const noexcept;
	
	/// Returns `true` if the material surface is two-sided, and `false` otherwise.
	bool is_two_sided() const noexcept;
	
	/// Returns the material shadow mode.
	shadow_mode get_shadow_mode() const noexcept;

	/**
	 * Returns the material property with the specified name, or `nullptr` if the material could not be found.
	 */
	material_property_base* get_property(const std::string& name) const;

	/**
	 * Returns a list of all material properties in the material.
	 */
	const std::list<material_property_base*>* get_properties() const;

private:
	/**
	 * Attempts to reconnect all material properties to their corresponding shader inputs.
	 *
	 * @return Number of disconnected properties.
	 */
	std::size_t reconnect_properties();

	gl::shader_program* program;
	std::uint32_t flags;
	blend_mode blend_mode;
	float opacity_threshold;
	bool two_sided;
	shadow_mode shadow_mode;
	std::list<material_property_base*> properties;
	std::unordered_map<std::string, material_property_base*> property_map;
};

template <typename T>
material_property<T>* material::add_property(const std::string& name, std::size_t element_count)
{
	// Allocate property
	material_property<T>* property = new material_property<T>(element_count);

	// Add to property list and map
	properties.push_back(property);
	property_map[name] = property;

	// Attempt to connect property to its corresponding shader input
	if (program)
	{
		property->connect(program->get_input(name));
	}

	return property;
}

inline gl::shader_program* material::get_shader_program() const
{
	return program;
}

inline std::uint32_t material::get_flags() const noexcept
{
	return flags;
}

inline blend_mode material::get_blend_mode() const noexcept
{
	return blend_mode;
}

inline float material::get_opacity_threshold() const noexcept
{
	return opacity_threshold;
}

inline bool material::is_two_sided() const noexcept
{
	return two_sided;
}

inline shadow_mode material::get_shadow_mode() const noexcept
{
	return shadow_mode;
}

inline material_property_base* material::get_property(const std::string& name) const
{
	if (auto it = property_map.find(name); it != property_map.end())
	{
		return it->second;
	}

	return nullptr;
}

inline const std::list<material_property_base*>* material::get_properties() const
{
	return &properties;
}

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_HPP
