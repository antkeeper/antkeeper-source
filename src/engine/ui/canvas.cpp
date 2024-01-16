// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/canvas.hpp>
#include <engine/debug/log.hpp>

namespace ui {

void canvas::descendant_added(element& descendant)
{
	// Add descendant to the scene
	descendant.add_to_scene(m_scene);
	
	// Add all sub-descendants to the scene
	descendant.visit_descendants
	(
		[&](element& e)
		{
			e.add_to_scene(m_scene);
		}
	);
}

void canvas::descendant_removed(element& descendant)
{
	// Remove descendant from the scene
	descendant.remove_from_scene(m_scene);
	
	// Remove all sub-descendants from the scene
	descendant.visit_descendants
	(
		[&](element& e)
		{
			e.remove_from_scene(m_scene);
		}
	);
}

} // namespace ui
