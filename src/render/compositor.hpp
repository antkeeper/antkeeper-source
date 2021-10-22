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

#ifndef ANTKEEPER_RENDER_COMPOSITOR_HPP
#define ANTKEEPER_RENDER_COMPOSITOR_HPP

#include "render/context.hpp"
#include "render/queue.hpp"
#include <list>

namespace render {

class pass;

/**
 * 
 */
class compositor
{
public:
	void add_pass(pass* pass);
	void remove_pass(pass* pass);
	void remove_passes();

	void composite(const render::context& ctx, render::queue& queue) const;

	const std::list<pass*>* get_passes() const;

private:
	std::list<pass*> passes;
};

inline const std::list<pass*>* compositor::get_passes() const
{
	return &passes;
}

} // namespace render

#endif // ANTKEEPER_RENDER_COMPOSITOR_HPP
