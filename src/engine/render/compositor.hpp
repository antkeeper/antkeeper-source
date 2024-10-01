// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_COMPOSITOR_HPP
#define ANTKEEPER_RENDER_COMPOSITOR_HPP

#include <engine/render/context.hpp>
#include <list>

namespace render {

class pass;

/// Composites a scene.
class compositor
{
public:
	void add_pass(pass* pass);
	void remove_pass(pass* pass);
	void remove_passes();

	void composite(render::context& ctx);

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
