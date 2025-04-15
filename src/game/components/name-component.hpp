// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_NAME_COMPONENT_HPP
#define ANTKEEPER_NAME_COMPONENT_HPP

import <string>;

/// Supplies a human-readable name.
struct name_component
{
	/// UTF-8 encoded name.
	std::string name;
};

#endif // ANTKEEPER_NAME_COMPONENT_HPP
