// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.gl.blend_op;
import engine.utility.sized_types;

export namespace engine::gl
{
    /// Framebuffer blending operations.
    enum class blend_op: u8
    {
        add,
        subtract,
        reverse_subtract,
        min,
        max,
    };
}
