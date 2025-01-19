// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/stages/queue-stage.hpp>
#include <engine/scene/object.hpp>

namespace render {

void queue_stage::execute(render::context& ctx)
{
	// For each visible object in the render context
	for (const auto& object: ctx.objects)
	{
		object->render(ctx);
	}
}

} // namespace render
