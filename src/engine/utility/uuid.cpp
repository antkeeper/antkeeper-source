/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

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
