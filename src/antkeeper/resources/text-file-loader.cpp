/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "resources/resource-loader.hpp"
#include "resources/text-file.hpp"

template <>
text_file* resource_loader<text_file>::load(resource_manager* resource_manager, std::istream* is)
{
	text_file* file = new text_file();
	std::string line;

	while (!is->eof())
	{
		std::getline(*is, line);
		if (is->bad() || is->fail())
		{
			break;
		}

		file->push_back(line);
	}

	return file;
}

