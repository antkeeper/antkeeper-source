// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/label.hpp>
#include <engine/debug/log.hpp>

namespace ui {

label::label()
{
	m_text_object = std::make_unique<scene::text>();
}

void label::set_text(std::string_view text)
{
	m_text_object->set_content(std::move(text));
}

void label::set_font(std::shared_ptr<type::font> font)
{
	m_text_object->set_font(std::move(font));
}

void label::set_material(std::shared_ptr<render::material> material)
{
	m_text_object->set_material(std::move(material));
}

void label::set_color(const math::fvec4& color)
{
	m_text_object->set_color(color);
}

void label::add_to_scene(::scene::collection& scene)
{
	scene.add_object(*m_text_object);
}

void label::remove_from_scene(::scene::collection& scene)
{
	scene.remove_object(*m_text_object);
}

void label::bounds_recalculated()
{
	reposition_text();
}

void label::reposition_text()
{
	const auto& bounds = get_bounds();
	m_text_object->set_translation({std::round(bounds.min.x()), std::round(bounds.min.y()), 0.0f});
}

} // namespace ui
