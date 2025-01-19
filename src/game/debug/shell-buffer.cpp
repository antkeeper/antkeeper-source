// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/debug/shell-buffer.hpp"
#include <engine/debug/log.hpp>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <regex>

void shell_buffer::set_text_object(std::shared_ptr<scene::text> object)
{
	m_text_object = object;
}

int shell_buffer::sync()
{
	if (m_text_object)
	{
		auto string_view = this->view();
		const std::size_t max_line_count = 100;
		
		std::size_t line_count = 0;
		std::size_t pos = string_view.length();
		
		while (pos > 0 && line_count < max_line_count + 1)
		{
			--pos;
			if (string_view[pos] == '\n')
			{
				++line_count;
			}
		}
		
		string_view = string_view.substr(pos);
		
		m_text_object->set_content(string_view);
		
		this->str(std::string{string_view});
		this->pubseekoff(0, std::ios_base::end);
		
		const auto& font_metrics =  m_text_object->get_font()->get_metrics();
		auto translation = m_text_object->get_translation();
		translation.x() = std::round(font_metrics.linespace);
		translation.y() = std::round((line_count + 1) * font_metrics.linespace - font_metrics.descent);
		m_text_object->set_translation(translation);
	}
	
	return 0;
}
