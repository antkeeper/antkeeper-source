// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.center_container;
export import engine.ui.container;

export namespace engine::ui
{
	/// Centers child elements.
	class center_container: public container
	{
	public:
		/// Destructs a center container.
		~center_container() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::center_container;
		}

	private:
		void redistribute_children() override;
	};
}
