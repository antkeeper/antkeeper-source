// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_MARGIN_CONTAINER_HPP
#define ANTKEEPER_UI_MARGIN_CONTAINER_HPP

#include <engine/ui/container.hpp>

namespace ui {

/// Creates a margin around its children.
class margin_container: public container
{
public:
	/// Destructs a margin container.
	~margin_container() override = default;

	[[nodiscard]] inline constexpr element_type get_type() const noexcept override
	{
		return element_type::margin_container;
	}
	
private:
	void redistribute_children() override;
};

} // namespace ui

#endif // ANTKEEPER_UI_MARGIN_CONTAINER_HPP
