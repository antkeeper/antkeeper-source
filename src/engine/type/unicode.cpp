// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/unicode.hpp>
#include <stdexcept>

namespace type {

std::u32string to_utf32(std::string_view utf8)
{
	std::u32string utf32;

	for (std::size_t i = 0; i < utf8.size();)
	{
		const char32_t byte = static_cast<char32_t>(utf8[i]);
		char32_t codepoint = 0;

		if ((byte & 0x80) == 0)
		{
			// 1 byte character
			codepoint = byte;
			i += 1;
		}
		else if ((byte & 0xe0) == 0xc0)
		{
			if (i + 1 >= utf8.size())
			{
				throw std::runtime_error("Invalid UTF-8 sequence");
			}

			// 2 byte character
			codepoint = ((byte & 0x1f) << 6)
						| (utf8[i + 1] & 0x3f);
			i += 2;
		}
		else if ((byte & 0xf0) == 0xe0)
		{
			if (i + 2 >= utf8.size())
			{
				throw std::runtime_error("Invalid UTF-8 sequence");
			}

			// 3 byte character
			codepoint = ((byte & 0x0f) << 12)
						| (static_cast<char32_t>(utf8[i + 1] & 0x3f) << 6)
						| static_cast<char32_t>(utf8[i + 2] & 0x3f);
			i += 3;
		}
		else if ((byte & 0xf8) == 0xf0)
		{
			if (i + 3 >= utf8.size())
			{
				throw std::runtime_error("Invalid UTF-8 sequence");
			}

			// 4 byte character
			codepoint = ((byte & 0x07) << 18)
						| (static_cast<char32_t>(utf8[i + 1] & 0x3f) << 12)
						| (static_cast<char32_t>(utf8[i + 2] & 0x3f) << 6)
						| (utf8[i + 3] & 0x3f);
			i += 4;
		}
		else
		{
			throw std::runtime_error("Invalid UTF-8 byte");
		}

		utf32.push_back(codepoint);
	}

	return utf32;
}

std::string to_utf8(std::u32string_view utf32)
{
	std::string utf8;

	for (char32_t codepoint : utf32)
	{
		if (codepoint <= 0x7f)
		{
			// 1 byte character
			utf8.push_back(static_cast<char>(codepoint));
		}
		else if (codepoint <= 0x7ff)
		{
			// 2 byte character
			utf8.push_back(static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
			utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else if (codepoint <= 0xffff)
		{
			// 3 byte character
			utf8.push_back(static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
			utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else if (codepoint <= 0x10ffff)
		{
			// 4 byte character
			utf8.push_back(static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
			utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			utf8.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else
		{
			throw std::runtime_error("Invalid UTF-32 codepoint");
		}
	}

	return utf8;
}

} // namespace type
