/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GL_FRAMEBUFFER_ATTACHMENT_HPP
#define ANTKEEPER_GL_FRAMEBUFFER_ATTACHMENT_HPP

#include <engine/gl/image-view.hpp>
#include <cstdint>
#include <memory>

namespace gl {

/// Framebuffer attachment.
struct framebuffer_attachment
{
	/// Attachment usage bit mask.
	std::uint8_t usage_mask{};
	
	/// Attached image view.
	std::shared_ptr<gl::image_view> image_view;
	
	/// Mip level of attached image view.
	std::uint32_t level{};
};

} // namespace gl

#endif // ANTKEEPER_GL_FRAMEBUFFER_ATTACHMENT_HPP
