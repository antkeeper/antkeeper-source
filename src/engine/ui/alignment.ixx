// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.alignment;

export namespace engine::ui
{
	/// Alignments of elements within a container.
	enum class alignment
	{
		/// Aligns elements to the start of the container.
		start,

		/// Aligns elements to the center of the container.
		center,

		/// Aligns elements to the end of the container.
		end
	};
}
