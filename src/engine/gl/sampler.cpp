// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/sampler.hpp>
#include <glad/gl.h>

namespace gl {

namespace
{
	static constexpr GLenum mag_filter_lut[] =
	{
		GL_NEAREST, // sampler_filter::nearest
		GL_LINEAR   // sampler_filter::linear
	};
	
	static constexpr GLenum min_filter_lut[][2] =
	{
		{
			GL_NEAREST_MIPMAP_NEAREST, // sampler_filter::nearest, sampler_mipmap_mode::nearest
			GL_NEAREST_MIPMAP_LINEAR   // sampler_filter::nearest, sampler_mipmap_mode::linear
		},
		{
			GL_LINEAR_MIPMAP_NEAREST, // sampler_filter::linear, sampler_mipmap_mode::nearest
			GL_LINEAR_MIPMAP_LINEAR   // sampler_filter::linear, sampler_mipmap_mode::linear
		},
	};
	
	static constexpr GLenum wrap_lut[] =
	{
		GL_REPEAT,              // sampler_address_mode::repeat
		GL_MIRRORED_REPEAT,     // sampler_address_mode::mirrored_repeat
		GL_CLAMP_TO_EDGE,       // sampler_address_mode::clamp_to_edge
		GL_CLAMP_TO_BORDER,     // sampler_address_mode::clamp_to_border
		GL_MIRROR_CLAMP_TO_EDGE // sampler_address_mode::mirror_clamp_to_edge
	};
	
	static constexpr GLenum compare_func_lut[] =
	{
		GL_NEVER,    // compare_op::never
		GL_LESS,     // compare_op::less
		GL_EQUAL,    // compare_op::equal
		GL_LEQUAL,   // compare_op::less_or_equal
		GL_GREATER,  // compare_op::greater
		GL_NOTEQUAL, // compare_op::not_equal
		GL_GEQUAL,   // compare_op::greater_or_equal
		GL_ALWAYS    // compare_op::always
	};
}

sampler::sampler
(
	sampler_filter mag_filter,
	sampler_filter min_filter,
	sampler_mipmap_mode mipmap_mode,
	sampler_address_mode address_mode_u,
	sampler_address_mode address_mode_v,
	sampler_address_mode address_mode_w,
	float mip_lod_bias,
	float max_anisotropy,
	bool compare_enabled,
	gl::compare_op compare_op,
	float min_lod,
	float max_lod,
	const std::array<float, 4>& border_color
)
{
	glCreateSamplers(1, &m_gl_named_sampler);
	
	set_mag_filter(mag_filter);
	set_min_filter(min_filter);
	set_mipmap_mode(mipmap_mode);
	set_address_mode_u(address_mode_u);
	set_address_mode_v(address_mode_v);
	set_address_mode_w(address_mode_w);
	set_mip_lod_bias(mip_lod_bias);
	set_max_anisotropy(max_anisotropy);
	set_compare_enabled(compare_enabled);
	set_compare_op(compare_op);
	set_min_lod(min_lod);
	set_max_lod(max_lod);
	set_border_color(border_color);
}

sampler::~sampler()
{
	glDeleteSamplers(1, &m_gl_named_sampler);
}

void sampler::set_mag_filter(sampler_filter filter)
{
	if (m_mag_filter != filter)
	{
		m_mag_filter = filter;
		const auto gl_mag_filter = mag_filter_lut[std::to_underlying(m_mag_filter)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
	}
}

void sampler::set_min_filter(sampler_filter filter)
{
	if (m_min_filter != filter)
	{
		m_min_filter = filter;
		const auto gl_min_filter = min_filter_lut[std::to_underlying(m_min_filter)][std::to_underlying(m_mipmap_mode)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_MIN_FILTER, gl_min_filter);
	}
}

void sampler::set_mipmap_mode(sampler_mipmap_mode mode)
{
	if (m_mipmap_mode != mode)
	{
		m_mipmap_mode = mode;
		const auto gl_min_filter = min_filter_lut[std::to_underlying(m_min_filter)][std::to_underlying(m_mipmap_mode)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_MIN_FILTER, gl_min_filter);
	}
}

void sampler::set_address_mode_u(sampler_address_mode mode)
{
	if (m_address_mode_u != mode)
	{
		m_address_mode_u = mode;
		const auto gl_wrap_s = wrap_lut[std::to_underlying(m_address_mode_u)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_WRAP_S, gl_wrap_s);
	}
}

void sampler::set_address_mode_v(sampler_address_mode mode)
{
	if (m_address_mode_v != mode)
	{
		m_address_mode_v = mode;
		const auto gl_wrap_t = wrap_lut[std::to_underlying(m_address_mode_v)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_WRAP_T, gl_wrap_t);
	}
}

void sampler::set_address_mode_w(sampler_address_mode mode)
{
	if (m_address_mode_w != mode)
	{
		m_address_mode_w = mode;
		const auto gl_wrap_r = wrap_lut[std::to_underlying(m_address_mode_w)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_WRAP_R, gl_wrap_r);
	}
}

void sampler::set_mip_lod_bias(float bias)
{
	if (m_mip_lod_bias != bias)
	{
		m_mip_lod_bias = bias;
		glSamplerParameterf(m_gl_named_sampler, GL_TEXTURE_LOD_BIAS, m_mip_lod_bias);
	}
}

void sampler::set_max_anisotropy(float anisotropy)
{
	if (m_max_anisotropy != anisotropy)
	{
		m_max_anisotropy = anisotropy;
		glSamplerParameterf(m_gl_named_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_max_anisotropy);
	}
}

void sampler::set_compare_enabled(bool enabled)
{
	if (m_compare_enabled != enabled)
	{
		m_compare_enabled = enabled;
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_COMPARE_MODE, (m_compare_enabled) ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
	}
}

void sampler::set_compare_op(gl::compare_op op)
{
	if (m_compare_op != op)
	{
		m_compare_op = op;
		const auto gl_compare_func = compare_func_lut[std::to_underlying(m_compare_op)];
		glSamplerParameteri(m_gl_named_sampler, GL_TEXTURE_COMPARE_FUNC, gl_compare_func);
	}
}

void sampler::set_min_lod(float lod)
{
	if (m_min_lod != lod)
	{
		m_min_lod = lod;
		glSamplerParameterf(m_gl_named_sampler, GL_TEXTURE_MIN_LOD, m_min_lod);
	}
}

void sampler::set_max_lod(float lod)
{
	if (m_max_lod != lod)
	{
		m_max_lod = lod;
		glSamplerParameterf(m_gl_named_sampler, GL_TEXTURE_MAX_LOD, m_max_lod);
	}
}

void sampler::set_border_color(const std::array<float, 4>& color)
{
	if (m_border_color != color)
	{
		m_border_color = color;
		glSamplerParameterfv(m_gl_named_sampler, GL_TEXTURE_BORDER_COLOR, m_border_color.data());
	}
}

} // namespace gl
