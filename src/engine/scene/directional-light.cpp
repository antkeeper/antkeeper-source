// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/directional-light.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/debug/log.hpp>

namespace scene {

directional_light::directional_light()
{
	set_shadow_bias(m_shadow_bias);
	update_shadow_cascade_distances();
}

void directional_light::set_direction(const math::fvec3& direction)
{
	set_rotation(math::rotation(math::fvec3{0.0f, 0.0f, -1.0f}, direction));
}

void directional_light::set_shadow_caster(bool caster) noexcept
{
	m_shadow_caster = caster;
}

void directional_light::set_shadow_framebuffer(std::shared_ptr<gl::framebuffer> framebuffer) noexcept
{
	m_shadow_framebuffer = std::move(framebuffer);
	if (m_shadow_framebuffer)
	{
		if (!m_shadow_texture)
		{
			m_shadow_texture = std::make_shared<gl::texture_2d>
			(
				std::static_pointer_cast<gl::image_view_2d>(m_shadow_framebuffer->attachments().front().image_view),
				std::make_shared<gl::sampler>
				(
					gl::sampler_filter::linear,
					gl::sampler_filter::linear,
					gl::sampler_mipmap_mode::linear,
					gl::sampler_address_mode::clamp_to_border,
					gl::sampler_address_mode::clamp_to_border,
					gl::sampler_address_mode::clamp_to_border,
					0.0f,
					0.0f,
					true,
					gl::compare_op::greater
				)
			);
		}
		else
		{
			m_shadow_texture->set_image_view(std::static_pointer_cast<gl::image_view_2d>(m_shadow_framebuffer->attachments().front().image_view));
		}
	}
	else
	{
		m_shadow_texture = nullptr;
	}
}

void directional_light::set_shadow_bias(float bias) noexcept
{
	m_shadow_bias = bias;
	update_shadow_scale_bias_matrices();
}

void directional_light::set_shadow_cascade_count(unsigned int count) noexcept
{
	m_shadow_cascade_count = std::min(std::max(count, 1u), 4u);
	update_shadow_scale_bias_matrices();
	update_shadow_cascade_distances();
}

void directional_light::set_shadow_max_distance(float distance) noexcept
{
	m_shadow_max_distance = distance;
	update_shadow_cascade_distances();
}

void directional_light::set_shadow_fade_range(float range) noexcept
{
	m_shadow_fade_range = range;
}

void directional_light::set_shadow_cascade_distribution(float weight) noexcept
{
	m_shadow_cascade_distribution = weight;
	update_shadow_cascade_distances();
}

void directional_light::transformed()
{
	m_direction = get_rotation() * math::fvec3{0.0f, 0.0f, -1.0f};
}

void directional_light::color_updated()
{
	m_colored_illuminance = m_color * m_illuminance;
}

void directional_light::illuminance_updated()
{
	m_colored_illuminance = m_color * m_illuminance;
}

void directional_light::update_shadow_scale_bias_matrices()
{
	// Transform coordinate range from `[-1, 1]` to `[0, 1]` and apply shadow bias
	auto m = math::translate(math::fvec3{0.5f, 0.5f, m_shadow_bias}) * math::scale(math::fvec3{0.5f, 0.5f, 1.0f});
	
	// Apply cascade scale
	m = math::scale(math::fvec3{0.5f, 0.5f, 1.0f}) * m;
	
	for (unsigned int i = 0; i < m_shadow_cascade_count; ++i)
	{
		// Apply cascade bias
		m_shadow_scale_bias_matrices[i] = math::translate(math::fvec3{static_cast<float>(i % 2) * 0.5f, static_cast<float>(i / 2) * 0.5f, 0.0f}) * m;
	}
}

void directional_light::update_shadow_cascade_distances()
{
	if (!m_shadow_cascade_count)
	{
		return;
	}
	
	m_shadow_cascade_distances[m_shadow_cascade_count - 1] = m_shadow_max_distance;
	for (unsigned int i = 0; i < m_shadow_cascade_count - 1; ++i)
	{
		const auto weight = static_cast<float>(i + 1) / static_cast<float>(m_shadow_cascade_count);
		
		// Calculate linear and logarithmic distribution distances
		const auto linear_distance = m_shadow_max_distance * weight;
		// const auto log_distance = math::log_lerp(0.0f, m_shadow_max_distance, weight);
		
		// Interpolate between linear and logarithmic distribution distances
		// cascade_distances[i] = math::lerp(linear_distance, log_distance, light.get_shadow_cascade_distribution());
		
		m_shadow_cascade_distances[i] = linear_distance;
	}
}

} // namespace scene
