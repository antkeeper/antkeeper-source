// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.culling_stage;
export import engine.render.stage;

export namespace engine::render
{
	/// Builds a set of scene objects visible to the current camera and stores it in the render context.
	class culling_stage: public stage
	{
	public:
		/// Destructs a culling stage.
		~culling_stage() override = default;

		void execute(render::context& ctx) override;
	};
}
