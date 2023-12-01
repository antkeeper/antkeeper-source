// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/unicode/convert.hpp>
#include <codecvt>

namespace type {
namespace unicode {

std::u32string u32(const std::string& u8)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.from_bytes(u8);
}

std::string u8(const std::u32string& u32)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.to_bytes(u32);
}

} // namespace unicode
} // namespace type
