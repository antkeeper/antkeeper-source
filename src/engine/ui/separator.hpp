// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_SEPARATOR_HPP
#define ANTKEEPER_UI_SEPARATOR_HPP

#include <engine/ui/element.hpp>

namespace ui {

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

} // namespace ui

#endif // ANTKEEPER_UI_SEPARATOR_HPP
