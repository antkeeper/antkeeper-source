// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
