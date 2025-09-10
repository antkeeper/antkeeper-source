// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/image-view.hpp>
#include <engine/utility/sized-types.hpp>
#include <array>
#include <span>
#include <vector>

namespace engine::gl
{
	/// Framebuffer attachment.
	struct framebuffer_attachment
	{
		/// Attachment usage bit mask.
		u8 usage_mask{};

		/// Attached image view.
		std::shared_ptr<gl::image_view> image_view;

		/// Mip level of attached image view.
		u32 level{};
	};

	/// Framebuffer attachment usage bits.
	enum: u8
	{
		/// Framebuffer color attachment.
		color_attachment_bit = 0b001,

		/// Framebuffer depth attachment.
		depth_attachment_bit = 0b010,

		/// Framebuffer stencil attachment.
		stencil_attachment_bit = 0b100,

		/// Framebuffer depth/stencil attachment.
		depth_stencil_attachment_bits = 0b110,
	};

	/// Framebuffer object.
	class framebuffer
	{
	public:
		/// Constructs a framebuffer.
		/// @param attachments Framebuffer attachments.
		/// @param width Width of the framebuffer.
		/// @param height Height of the framebuffer.
		framebuffer(std::span<const framebuffer_attachment> attachments, u32 width, u32 height);

		/// Destroys a framebuffer.
		~framebuffer();

		/// Resizes the framebuffer.
		/// @param width New width of the framebuffer.
		/// @param height New height of the framebuffer.
		/// @warning Does not resize framebuffer attachments.
		void resize(u32 width, u32 height);

		/// Returns the framebuffer attachments.
		[[nodiscard]] inline constexpr const std::vector<framebuffer_attachment>& attachments() const noexcept
		{
			return m_attachments;
		}

		/// Returns the dimensions of the framebuffer.
		[[nodiscard]] inline constexpr const std::array<u32, 2>& dimensions() const noexcept
		{
			return m_dimensions;
		}

		/// Returns the width of the framebuffer.
		[[nodiscard]] inline constexpr u32 width() const noexcept
		{
			return m_dimensions[0];
		}

		/// Returns the height of the framebuffer.
		[[nodiscard]] inline constexpr u32 height() const noexcept
		{
			return m_dimensions[1];
		}

	private:
		friend class pipeline;

		std::vector<framebuffer_attachment> m_attachments;
		std::array<u32, 2> m_dimensions{0, 0};
		unsigned int m_gl_named_framebuffer{0};
	};
}
