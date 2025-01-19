// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-loop-container.hpp>

namespace geom {

brep_loop* brep_loop_container::emplace_back()
{
	brep_loop* loop = brep_element_container<brep_loop>::emplace_back();
	loop->m_index = size() - 1;
	
	return loop;
};

} // namespace geom
