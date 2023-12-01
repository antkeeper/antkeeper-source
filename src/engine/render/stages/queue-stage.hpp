// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_QUEUE_STAGE_HPP
#define ANTKEEPER_RENDER_QUEUE_STAGE_HPP

#include <engine/render/stage.hpp>

namespace render {

/**
 * Builds render queues.
 */
class queue_stage: public stage
{
public:
	void execute(render::context& ctx) override;
};

} // namespace render

#endif // ANTKEEPER_RENDER_QUEUE_STAGE_HPP
