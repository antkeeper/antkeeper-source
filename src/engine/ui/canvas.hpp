// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/ui/element.hpp>
#include <engine/scene/collection.hpp>

namespace engine::ui
{
	/// UI canvas.
	class canvas: public element
	{
	public:
		/// Destructs a canvas.
		~canvas() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::canvas;
		}

		/// Returns the canvas's scene.
		[[nodiscard]] inline auto& get_scene() noexcept
		{
			return m_scene;
		}

		/// @copydoc get_scene()
		[[nodiscard]] inline const auto& get_scene() const noexcept
		{
			return m_scene;
		}

	private:
		void descendant_added(element& descendant) override;
		void descendant_removed(element& descendant) override;

		scene::collection m_scene;
	};
}
