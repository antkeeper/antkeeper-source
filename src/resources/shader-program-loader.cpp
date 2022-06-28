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

#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/text-file.hpp"
#include "render/shader-template.hpp"
#include "gl/shader-object.hpp"
#include "gl/shader-program.hpp"
#include <sstream>

/**
 * Handles `#pragma include` directives by loading the specified text files and inserting them in place.
 */
static void handle_includes(text_file* source, resource_manager* resource_manager)
{
	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		std::string token;
		std::istringstream line_stream((*source)[i]);
		
		// If line contains a `#pragma include` directive
		if (line_stream >> token && token == "#pragma" &&
			line_stream >> token && token == "include")
		{
			// If third token is enclosed in quotes or angled brackets
			if (line_stream >> token && token.size() > 2 &&
				((token.front() == '\"' && token.back() == '\"') ||
				(token.front() == '<' && token.back() == '>')))
			{
				// Extract include path
				std::string path = token.substr(1, token.length() - 2);
				
				// Load include file
				if (!resource_manager->load<text_file>(path))
				{
					(*source)[i] = "#error file not found (" + path + ")";
				}
				else
				{
					// Create copy of include file
					text_file include_file = *(resource_manager->load<text_file>(path));
					
					// Handle `#pragma include` directives inside include file
					handle_includes(&include_file, resource_manager);
					
					// Replace #pragma include directive with include file contents
					source->erase(source->begin() + i);
					source->insert(source->begin() + i, include_file.begin(), include_file.end());
					i += include_file.size() - 1;
				}
			}
			else
			{
				(*source)[i] = "#error malformed include directive (" + (*source)[i] + ")";
			}
		}
	}
}

template <>
gl::shader_program* resource_loader<gl::shader_program>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load shader template source
	text_file source_lines = *resource_loader<text_file>::load(resource_manager, file, path);
	
	// Handle `#pragma include` directives
	handle_includes(&source_lines, resource_manager);
	
	// Join vector of source lines into single string
	std::ostringstream stream;
	std::copy(source_lines.begin(), source_lines.end(), std::ostream_iterator<std::string>(stream, "\n"));
	
	// Create shader template
	render::shader_template* shader = new render::shader_template(stream.str());
	
	// Build shader program
	gl::shader_program* program = shader->build(render::shader_template::dictionary_type());
	
	// Check if shader program was linked successfully
	if (!program->was_linked())
	{
		throw std::runtime_error("Shader program linking failed: " + program->get_info_log());
	}
	
	// Destroy shader template
	delete shader;

	return program;
}
