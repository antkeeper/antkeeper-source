// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/font.hpp>

namespace type {

font::font(const font_metrics& metrics):
	metrics(metrics)
{}

font::font()
{}

font::~font()
{}

void font::kern(char32_t first, char32_t second, const math::fvec2& offset)
{
	kerning_table[first][second] = offset;
}

void font::set_font_metrics(const font_metrics& metrics)
{
	this->metrics = metrics;
}

const math::fvec2& font::get_kerning(char32_t first, char32_t second) const
{
	if (auto it_first = kerning_table.find(first); it_first != kerning_table.end())
		if (auto it_second = it_first->second.find(second); it_second != it_first->second.end())
			return it_second->second;
	
	static const math::fvec2 no_kerning = {0.0f, 0.0f};
	return no_kerning;
}

} // namespace type
