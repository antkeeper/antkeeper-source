// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.row_container;
export import engine.ui.container;
export import engine.ui.alignment;

export namespace engine::ui
{
	/// Arranges child elements as columns in a row.
	class row_container: public container
	{
	public:
		/// Destructs a row container.
		~row_container() override = default;

		[[nodiscard]] inline constexpr element_type get_type() const noexcept override
		{
			return element_type::row_container;
		}

		/// Sets the alignment of all child elements in the row.
		/// @param alignment Alignment of all child elements in the row.
		void set_alignment(ui::alignment alignment);

		/// Sets the spacing between child elements.
		/// @param spacing Spacing between child elements.
		void set_spacing(float spacing);

	private:
		void redistribute_children() override;

		alignment m_alignment{alignment::center};
		float m_spacing{0.0f};
	};
}
