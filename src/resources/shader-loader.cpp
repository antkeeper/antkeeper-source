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
#include "text-file.hpp"
#include <sstream>

#include <emergent/emergent.hpp>
using namespace Emergent;

static void preprocess(ResourceManager* resourceManager, TextFile* source)
{
	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		// Tokenize line
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream((*source)[i]);
		while (linestream >> token)
			tokens.push_back(token);

		// Inject DEBUG and NDEBUG macros
		if (!tokens.empty() && tokens[0] == "#version")
		{
			#if defined(NDEBUG)
				source->insert(source->begin() + i + 1, "#define NDEBUG");
			#else
				source->insert(source->begin() + i + 1, "#define DEBUG");
			#endif

			++i;
		}
		// Handle #pragma include directives
		else if (tokens.size() == 3 && tokens[0] == "#pragma" && tokens[1] == "include")
		{
			// Get path to include file
			std::string path = tokens[2].substr(1, tokens[2].length() - 2);

			// Load include file
			TextFile* includeFile = resourceManager->load<TextFile>(path);

			// Preprocess include file
			preprocess(resourceManager, includeFile);
			
			// Replace #pragma include directive with include file contents
			source->erase(source->begin() + i);
			source->insert(source->begin() + i, includeFile->begin(), includeFile->end());
			i += includeFile->size() - 1;
		}
	}
}

template <>
Shader* ResourceLoader<Shader>::load(ResourceManager* resourceManager, std::istream* is)
{
	// Allocate shader source
	TextFile* source = new TextFile();

	while (!is->eof())
	{
		// For each line in input stream
		std::string line;
		std::getline(*is, line);
		if (is->bad() || is->fail())
		{
			break;
		}

		// Add line to the source
		source->push_back(line);
	}

	// Preprocess source
	preprocess(resourceManager, source);

	// Create shader
	Shader* shader = new Shader();
	shader->setSource(*source);

	// Free the shader source
	delete source;

	return shader;
}

