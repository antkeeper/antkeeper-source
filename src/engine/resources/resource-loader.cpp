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

#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <physfs.h>

void physfs_getline(PHYSFS_File* file, std::string& line)
{
	line.clear();
	
	for (;;)
	{
		char c;
		const PHYSFS_sint64 status = PHYSFS_readBytes(file, &c, 1);
		
		if (status == 1)
		{
			if (c == '\r')
			{
				continue;
			}
			else if (c == '\n')
			{
				break;
			}
			else
			{
				line.append(1, c);
			}
			
		}
		else
		{
			if (PHYSFS_eof(file))
			{
				break;
			}
			else
			{
				throw deserialize_error(PHYSFS_getLastError());
			}
		}
	}
}
