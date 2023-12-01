// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/uuid.hpp>
#include <cstdint>

std::string uuid::string() const
{
	static const char* hex = "0123456789abcdef";
	
	std::string str(32, '0');
	
	char* c = str.data();
	for (std::byte byte: data)
	{
		*(c++) = hex[static_cast<std::uint8_t>(byte) >> 4];
        *(c++) = hex[static_cast<std::uint8_t>(byte) & 15];
	}
	
	return str;
}

std::ostream& operator<<(std::ostream& os, const uuid& id)
{
	os << id.string();
	return os;
}
