// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RESOURCES_DESERIALIZE_ERROR_HPP
#define ANTKEEPER_RESOURCES_DESERIALIZE_ERROR_HPP

#include <stdexcept>

/// An exception of this type is thrown when an error occurs during deserialization.
class deserialize_error: public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

#endif // ANTKEEPER_RESOURCES_DESERIALIZE_ERROR_HPP
