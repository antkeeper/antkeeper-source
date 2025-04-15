// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.render.queue_stage;
import engine.scene.object;

namespace engine::render
{
	void queue_stage::execute(render::context& ctx)
	{
		// For each visible object in the render context
		for (const auto& object: ctx.objects)
		{
			object->render(ctx);
		}
	}
}
