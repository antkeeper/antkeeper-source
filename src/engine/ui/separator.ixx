// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.separator;
export import engine.ui.element;

export namespace engine::ui
{
	/// Separates elements in a container.
	class separator: public element
	{
	public:
		/// Destructs a separator.
		~separator() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::separator;
		}
	};
}
