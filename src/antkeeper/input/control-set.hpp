/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_CONTROL_SET_HPP
#define ANTKEEPER_CONTROL_SET_HPP

#include <list>

class control;

/**
 * A set of controls which can be managed simultaneously.
 *
 * @ingroup input
 */
class control_set
{
public:
	/**
	 * Adds a control to the control set.
	 *
	 * @param control Pointer to the control to add.
	 */
	void add_control(control* control);

	/**
	 * Removes a control from the control set.
	 *
	 * @param control Pointer to the control to remove.
	 */
	void remove_control(control* control);

	/**
	 * Removes all controls from the control set.
	 */
	void remove_controls();
	
	/**
	 * Calls control::update() on each control in this control set.
	 */
	void update();

	/**
	 * Enables or disables callbacks for all controls in the control set.
	 *
	 * @param enabled Whether to enable or disable callbacks for all controls in the control set.
	 */
	void set_callbacks_enabled(bool enabled);

	/**
	 * Returns the list of controls in the control set.
	 */
	const std::list<control*>* get_controls() const;
	
private:
	std::list<control*> controls;
};

inline const std::list<control*>* control_set::get_controls() const
{
	return &controls;
}

#endif // ANTKEEPER_CONTROL_SET_HPP

