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

#ifndef ANTKEEPER_RENDER_PASS_HPP
#define ANTKEEPER_RENDER_PASS_HPP

class rasterizer;
class framebuffer;
struct render_context;

/**
 *
 */
class render_pass
{
public:
	render_pass(rasterizer* rasterizer, const framebuffer* framebuffer);
	virtual ~render_pass();

	virtual void render(render_context* context) const = 0;

	void set_enabled(bool enabled);
	bool is_enabled() const;

protected:
	rasterizer* rasterizer;
	const ::framebuffer* framebuffer;

private:
	bool enabled;
};

inline bool render_pass::is_enabled() const
{
	return enabled;
}

#endif // ANTKEEPER_RENDER_PASS_HPP

