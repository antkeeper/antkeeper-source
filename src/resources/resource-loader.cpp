/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "resource-loader.hpp"
#include <physfs.h>

void physfs_getline(PHYSFS_File* file, std::string& line)
{
	PHYSFS_sint64 bytes;
	char c;
	
	line.clear();
	
	do
	{
		bytes = PHYSFS_readBytes(file, &c, 1);
		
		if (bytes != 1 || c == '\n')
			break;
		
		if (c == '\r')
			continue;
		
		line.append(1, c);
	}
	while (!PHYSFS_eof(file));
}
