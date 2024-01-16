// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_IMAGE_HPP
#define ANTKEEPER_UI_IMAGE_HPP

#include <engine/ui/element.hpp>
#include <engine/scene/billboard.hpp>
#include <memory>

namespace ui {

/**
 * UI image.
 */
class image: public element
{
public:
	/** Constructs an image. */
	image();
	
	/** Destructs an image. */
	~image() override = default;	
	
	/**
	 * Sets the image material.
	 *
	 * @param material Image material.
	 */
	void set_material(std::shared_ptr<render::material> material);
	
private:
	void add_to_scene(::scene::collection& scene) override;
	void remove_from_scene(::scene::collection& scene) override;

	std::unique_ptr<scene::billboard> m_billboard;
};

} // namespace ui

#endif // ANTKEEPER_UI_IMAGE_HPP
