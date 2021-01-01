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

#ifndef ANTKEEPER_COMPOSITOR_HPP
#define ANTKEEPER_COMPOSITOR_HPP

#include <list>

class render_pass;
struct render_context;

/**
 * 
 */
class compositor
{
public:
	void add_pass(render_pass* pass);
	void remove_pass(render_pass* pass);
	void remove_passes();

	void composite(render_context* context) const;

	const std::list<render_pass*>* get_passes() const;

private:
	std::list<render_pass*> passes;
};

inline const std::list<render_pass*>* compositor::get_passes() const
{
	return &passes;
}

#endif // ANTKEEPER_COMPOSITOR_HPP

