// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_TAG_COMPONENT_HPP
#define ANTKEEPER_TAG_COMPONENT_HPP

#include <engine/hash/fnv.hpp>

/// Tags an entity with a 32-bit FNV-1a hash value.
/// @tparam Tag 32-bit FNV-1a hash value.
template <hash::fnv32_t Tag>
struct tag_component {};

#endif // ANTKEEPER_TAG_COMPONENT_HPP
