// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_CANVAS_HPP
#define ANTKEEPER_UI_CANVAS_HPP

#include <engine/ui/element.hpp>
#include <engine/scene/collection.hpp>

namespace ui {

/**
 * UI canvas.
 */
class canvas: public element
{
public:
	/** Destructs a canvas. */
	~canvas() override = default;	
	
	/// @{
	/** Returns the canvas's scene. */
	[[nodiscard]] inline const auto& get_scene() const noexcept
	{
		return m_scene;
	}
	[[nodiscard]] inline auto& get_scene() noexcept
	{
		return m_scene;
	}
	/// @}
	
private:
	void descendant_added(element& descendant) override;
	void descendant_removed(element& descendant) override;
	
	scene::collection m_scene;
};

} // namespace ui

#endif // ANTKEEPER_UI_CANVAS_HPP
