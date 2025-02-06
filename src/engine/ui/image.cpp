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

void image::bounds_recalculated()
{
	const auto& bounds = get_bounds();
	const auto center = bounds.center();
	const auto size = bounds.size();

	auto transform = m_billboard->get_transform();
	transform.translation.x() = center.x();
	transform.translation.y() = center.y();
	transform.translation.z() = static_cast<float>(get_depth());
	transform.scale.x() = size.x() * 0.5f;
	transform.scale.y() = size.y() * 0.5f;

	m_billboard->set_transform(transform);
}

void image::add_to_scene(::scene::collection& scene)
{
	scene.add_object(*m_billboard);
}

void image::remove_from_scene(::scene::collection& scene)
{
	scene.remove_object(*m_billboard);
}

void image::depth_changed()
{
	auto translation = m_billboard->get_translation();
	translation.z() = static_cast<float>(get_depth());
	m_billboard->set_translation(translation);
}

} // namespace ui
