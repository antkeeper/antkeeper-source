// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/image.hpp>

namespace ui {

image::image()
{
	m_billboard = std::make_unique<scene::billboard>();
}

void image::set_material(std::shared_ptr<render::material> material)
{
	m_billboard->set_material(std::move(material));
}

void image::add_to_scene(::scene::collection& scene)
{
	scene.add_object(*m_billboard);
}

void image::remove_from_scene(::scene::collection& scene)
{
	scene.remove_object(*m_billboard);
}

} // namespace ui
