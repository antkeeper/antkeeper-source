// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.margin_container;
export import engine.ui.container;

export namespace engine::ui
{
	/// Creates a margin around its children.
	class margin_container: public container
	{
	public:
		/// Destructs a margin container.
		~margin_container() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::margin_container;
		}

	private:
		void redistribute_children() override;
	};
}
