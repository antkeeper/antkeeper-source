// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/ui/element.hpp>

namespace engine::ui
{
	/// Separates elements in a container.
	class separator: public element
	{
	public:
		/// Destructs a separator.
		~separator() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::separator;
		}
	};
}
