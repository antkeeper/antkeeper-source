/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "string-table.hpp"
#include <sstream>
#include <stdexcept>

#include <emergent/emergent.hpp>
using namespace Emergent;

template <>
ParameterDict* ResourceLoader<ParameterDict>::load(ResourceManager* resourceManager, std::istream* is)
{
	// Load string table from input stream
	StringTable* table = ResourceLoader<StringTable>::load(resourceManager, is);

	// Ensure table is not empty.
	if (!table || table->empty())
	{
		delete table;
		return nullptr;
	}

	// Create new parameter dict
	ParameterDict* dict = new ParameterDict();

	// Load parameters from table rows
	for (const StringTableRow& row: *table)
	{
		// Skip comments, blank parameter names, and rows that don't have exactly two columns
		if (row.size() != 2 || row[0].empty() || row[0][0] == '#')
		{
			continue;
		}

		// Add parameter to dict
		dict->set(row[0], row[1]);
	}
	
	return dict;
}

template <>
void ResourceLoader<ParameterDict>::save(ResourceManager* resourceManager, std::ostream* os, const ParameterDict* dict)
{
	// Build string table
	StringTable table;
	const std::map<std::string, std::string>* parameters = dict->getParameters();
	for (auto it = parameters->begin(); it != parameters->end(); ++it)
	{
		table.push_back({it->first, it->second});
	}

	// Save string table
	ResourceLoader<StringTable>::save(resourceManager, os, &table);
}

