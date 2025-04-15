// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.ui.label;
import engine.scene.text;
import engine.math.functions;

namespace engine::ui
{
	label::label()
	{
		m_text_object = std::make_unique<scene::text>();
	}

	void label::set_text(std::string_view text)
	{
		m_text_object->set_content(std::move(text));
		recalculate_min_size();
	}

	void label::set_font(std::shared_ptr<type::font> font)
	{
		m_text_object->set_font(std::move(font));
		recalculate_min_size();
	}

	std::shared_ptr<type::font> label::get_font() const noexcept
	{
		return m_text_object->get_font();
	}

	void label::set_material(std::shared_ptr<render::material> material)
	{
		m_text_object->set_material(std::move(material));
	}

	void label::set_color(const math::fvec4& color)
	{
		if (m_color != color)
		{
			m_color[0] = color[0];
			m_color[1] = color[1];
			m_color[2] = color[2];

			if (get_opacity() != color[3])
			{
				// The call to effective_opacity_changed() should update the text color
				set_opacity(color[3]);
			}
			else
			{
				m_text_object->set_color({m_color[0], m_color[1], m_color[2], get_effective_opacity()});
			}
		}
	}

	void label::set_color(const math::fvec3& color)
	{
		set_color({color[0], color[1], color[2], get_opacity()});
	}

	const std::string& label::get_text() const noexcept
	{
		return m_text_object->get_content();
	}

	void label::set_text_refresher(std::function<std::string(const label&)> refresher)
	{
		m_text_refresher = std::move(refresher);
	}

	void label::refresh_text()
	{
		if (m_text_refresher)
		{
			set_text(m_text_refresher(*this));
		}
	}

	void label::add_to_scene(scene::collection& scene)
	{
		scene.add_object(*m_text_object);
	}

	void label::remove_from_scene(scene::collection& scene)
	{
		scene.remove_object(*m_text_object);
	}

	void label::bounds_recalculated()
	{
		reposition_text();
	}

	void label::effective_opacity_changed()
	{
		m_color[3] = get_opacity();
		m_text_object->set_color({m_color[0], m_color[1], m_color[2], get_effective_opacity()});
	}

	void label::reposition_text()
	{
		auto translation = get_bounds().min;

		translation.x() -= m_text_object->get_local_bounds().min.x();

		if (auto font = m_text_object->get_font())
		{
			const auto& metrics = font->get_metrics();
			translation.y() -= metrics.descent;
		}
		else
		{
			translation.y() -= m_text_object->get_local_bounds().min.y();
		}

		m_text_object->set_translation({math::round(translation.x()), math::round(translation.y()), m_text_object->get_translation().z()});
	}

	void label::recalculate_min_size()
	{
		if (auto font = m_text_object->get_font())
		{
			const auto& metrics = font->get_metrics();

			// Without internal leading
			set_min_size({m_text_object->get_bounds().size().x(), math::round(metrics.em_size)});

			// With internal leading
			//set_min_size({m_text_object->get_bounds().size().x(), math::round(metrics.ascent - metrics.descent)});
		}
		else
		{
			set_min_size(math::fvec2(m_text_object->get_bounds().size()));
		}
	}
}
