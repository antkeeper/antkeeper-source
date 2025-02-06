// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/ui/container.hpp>

namespace ui {

void container::descendant_added(element&)
{
	redistribute_children();
}

void container::descendant_removed(element&)
{
	redistribute_children();
}

void container::child_min_size_changed(element&)
{
	redistribute_children();
}

} // namespace ui
