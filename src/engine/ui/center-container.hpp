// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_CENTER_CONTAINER_HPP
#define ANTKEEPER_UI_CENTER_CONTAINER_HPP

#include <engine/ui/container.hpp>

namespace ui {

/// Centers child elements.
class center_container: public container
{
public:
	/// Destructs a center container.
	~center_container() override = default;

	[[nodiscard]] inline constexpr element_type get_type() const noexcept override
	{
		return element_type::center_container;
	}
	
private:
	void redistribute_children() override;
};

} // namespace ui

#endif // ANTKEEPER_UI_CENTER_CONTAINER_HPP
