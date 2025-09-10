// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/material-variable.hpp>

namespace engine::render
{
	template <>
	material_variable_type matvar_bool::type() const noexcept
	{
		return material_variable_type::bvec1;
	}

	template <>
	material_variable_type matvar_bvec2::type() const noexcept
	{
		return material_variable_type::bvec2;
	}

	template <>
	material_variable_type matvar_bvec3::type() const noexcept
	{
		return material_variable_type::bvec3;
	}

	template <>
	material_variable_type matvar_bvec4::type() const noexcept
	{
		return material_variable_type::bvec4;
	}

	template <>
	material_variable_type matvar_int::type() const noexcept
	{
		return material_variable_type::ivec1;
	}

	template <>
	material_variable_type matvar_ivec2::type() const noexcept
	{
		return material_variable_type::ivec2;
	}

	template <>
	material_variable_type matvar_ivec3::type() const noexcept
	{
		return material_variable_type::ivec3;
	}

	template <>
	material_variable_type matvar_ivec4::type() const noexcept
	{
		return material_variable_type::ivec4;
	}

	template <>
	material_variable_type matvar_uint::type() const noexcept
	{
		return material_variable_type::uvec1;
	}

	template <>
	material_variable_type matvar_uvec2::type() const noexcept
	{
		return material_variable_type::uvec2;
	}

	template <>
	material_variable_type matvar_uvec3::type() const noexcept
	{
		return material_variable_type::uvec3;
	}

	template <>
	material_variable_type matvar_uvec4::type() const noexcept
	{
		return material_variable_type::uvec4;
	}

	template <>
	material_variable_type matvar_float::type() const noexcept
	{
		return material_variable_type::fvec1;
	}

	template <>
	material_variable_type matvar_fvec2::type() const noexcept
	{
		return material_variable_type::fvec2;
	}

	template <>
	material_variable_type matvar_fvec3::type() const noexcept
	{
		return material_variable_type::fvec3;
	}

	template <>
	material_variable_type matvar_fvec4::type() const noexcept
	{
		return material_variable_type::fvec4;
	}

	template <>
	material_variable_type matvar_fmat2::type() const noexcept
	{
		return material_variable_type::fmat2;
	}

	template <>
	material_variable_type matvar_fmat3::type() const noexcept
	{
		return material_variable_type::fmat3;
	}

	template <>
	material_variable_type matvar_fmat4::type() const noexcept
	{
		return material_variable_type::fmat4;
	}

	template <>
	material_variable_type matvar_texture_1d::type() const noexcept
	{
		return material_variable_type::texture_1d;
	}

	template <>
	material_variable_type matvar_texture_2d::type() const noexcept
	{
		return material_variable_type::texture_2d;
	}

	template <>
	material_variable_type matvar_texture_3d::type() const noexcept
	{
		return material_variable_type::texture_3d;
	}

	template <>
	material_variable_type matvar_texture_cube::type() const noexcept
	{
		return material_variable_type::texture_cube;
	}
}
