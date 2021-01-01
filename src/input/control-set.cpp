/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "control-set.hpp"
#include "control.hpp"

void control_set::add_control(control* control)
{
	controls.push_back(control);
}

void control_set::remove_control(control* control)
{
	controls.remove(control);
}

void control_set::remove_controls()
{
	controls.clear();
}

void control_set::update()
{
	for (control* control: controls)
	{
		control->update();
	}
}

void control_set::set_callbacks_enabled(bool enabled)
{
	for (control* control: controls)
	{
		control->set_callbacks_enabled(enabled);
	}
}

