// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/rectangle-light.hpp>
#include <engine/math/constants.hpp>

namespace engine::scene
{
	rectangle_light::rectangle_light()
	{
		transformed();
	}

	void rectangle_light::set_size(const math::fvec2& size)
	{
		set_scale({size.x(), size.y(), 1.0f});
	}

	void rectangle_light::transformed()
	{
		const auto& transform = get_transform();

		// Update corner positions
		// m_corners[0] = transform * math::fvec3{-0.5f, 0.0f, -0.5f};
		// m_corners[1] = transform * math::fvec3{ 0.5f, 0.0f, -0.5f};
		// m_corners[2] = transform * math::fvec3{ 0.5f, 0.0f,  0.5f};
		// m_corners[3] = transform * math::fvec3{-0.5f, 0.0f,  0.5f};
		m_corners[0] = transform * math::fvec3{-0.5f, -0.5f, 0.0f};
		m_corners[1] = transform * math::fvec3{-0.5f,  0.5f, 0.0f};
		m_corners[2] = transform * math::fvec3{0.5f,  0.5f, 0.0f};
		m_corners[3] = transform * math::fvec3{0.5f, -0.5f, 0.0f};

		// Update area
		m_area = get_scale().x() * get_scale().y();
		area_updated();
	}

	void rectangle_light::color_updated()
	{
		m_colored_luminous_flux = get_color() * m_luminous_flux;
		m_colored_luminance = get_color() * m_luminance;
	}

	void rectangle_light::area_updated() noexcept
	{
		// Calculate luminance from luminous flux
		m_luminance = m_luminous_flux / (m_area * math::pi<float>);
		m_colored_luminance = get_color() * m_luminance;
	}

	void rectangle_light::luminous_flux_updated() noexcept
	{
		m_colored_luminous_flux = get_color() * m_luminous_flux;

		// Calculate luminance from luminous flux
		m_luminance = m_luminous_flux / (m_area * math::pi<float>);
		m_colored_luminance = get_color() * m_luminance;
	}

	void rectangle_light::luminance_updated() noexcept
	{
		m_colored_luminance = get_color() * m_luminance;

		// Calculate luminous flux from luminance
		m_luminous_flux = m_luminance * (m_area * math::pi<float>);
		m_colored_luminous_flux = get_color() * m_luminous_flux;
	}
}
