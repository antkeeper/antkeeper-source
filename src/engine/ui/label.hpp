// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_LABEL_HPP
#define ANTKEEPER_UI_LABEL_HPP

#include <engine/ui/element.hpp>
#include <engine/scene/text.hpp>
#include <engine/type/font.hpp>
#include <memory>

namespace ui {

/**
 * UI label.
 */
class label: public element
{
public:
	/** Constructs an label. */
	label();
	
	/** Destructs an label. */
	~label() override = default;	
	
	/**
	 * Sets the label text.
	 *
	 * @param text UTF-8 string of text.
	 */
	void set_text(std::string_view text);
	
	/**
	 * Sets the label font.
	 *
	 * @param font Label font.
	 */
	void set_font(std::shared_ptr<type::font> font);
	
	/**
	 * Sets the label material.
	 *
	 * @param material Label material.
	 */
	void set_material(std::shared_ptr<render::material> material);
	
	/**
	 * Sets the label color and opacity.
	 *
	 * @param color Label color and opacity.
	 */
	void set_color(const math::fvec4& color);
	
private:
	void add_to_scene(::scene::collection& scene) override;
	void remove_from_scene(::scene::collection& scene) override;
	void bounds_recalculated() override;
	void reposition_text();

	std::unique_ptr<scene::text> m_text_object;
};

} // namespace ui

#endif // ANTKEEPER_UI_LABEL_HPP
