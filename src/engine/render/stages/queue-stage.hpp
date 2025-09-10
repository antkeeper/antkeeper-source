// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/stage.hpp>

namespace engine::render
{
	/// Builds render queues.
	class queue_stage: public stage
	{
	public:
		/// Destructs a queue stage.
		~queue_stage() override = default;
	
		void execute(render::context& ctx) override;
	};
}
