// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/unicode/convert.hpp>
#include <codecvt>

namespace type {
namespace unicode {

std::u32string u32(std::string_view u8_view)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.from_bytes(u8_view.data());
}

std::string u8(std::u32string_view u32_view)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.to_bytes(u32_view.data());
}

} // namespace unicode
} // namespace type
