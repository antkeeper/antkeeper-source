// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/ui/element.hpp>

namespace engine::ui
{
	/// Manages the layout of child elements.
	class container: public element
	{
	public:
		/// Destructs a container.
		~container() override = default;

	private:
		void descendant_added(element& descendant) override;
		void descendant_removed(element& descendant) override;
		void child_min_size_changed(element& descendant) override;
		virtual void redistribute_children() = 0;
	};
}
