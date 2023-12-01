// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_BLEND_OP_HPP
#define ANTKEEPER_GL_BLEND_OP_HPP

#include <cstdint>

namespace gl {

/// Framebuffer blending operations.
enum class blend_op: std::uint8_t
{
    add,
    subtract,
    reverse_subtract,
    min,
    max,
};

} // namespace gl

#endif // ANTKEEPER_GL_BLEND_OP_HPP
