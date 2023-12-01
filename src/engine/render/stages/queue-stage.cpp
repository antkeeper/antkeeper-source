// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/stages/queue-stage.hpp>
#include <engine/scene/object.hpp>
#include <algorithm>
#include <execution>

namespace render {

void queue_stage::execute(render::context& ctx)
{
	// For each visible object in the render context
	std::for_each
	(
		std::execution::seq,
		std::begin(ctx.objects),
		std::end(ctx.objects),
		[&ctx](scene::object_base* object)
		{
			object->render(ctx);
		}
	);
}

} // namespace render
