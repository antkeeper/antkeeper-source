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

#include "resources/resource-loader.hpp"
#include "resources/resource-manager.hpp"
#include "resources/text-file.hpp"
#include "render/shader-template.hpp"
#include "gl/shader-object.hpp"
#include "gl/shader-program.hpp"
#include <sstream>
#include <unordered_set>

/**
 * Scans a text file for the presence of a `#pragma once` directive.
 *
 * @param source Text file to scan.
 *
 * @return `true` if the file contains a `#pragma once` directive, `false` otherwise.
 */
static bool has_pragma_once(const text_file& source)
{
	for (const auto& line: source)
	{
		std::istringstream line_stream(line);
		std::string token;
		
		// If line contains a `#pragma once` directive
		if (line_stream >> token && token == "#pragma" &&
			line_stream >> token && token == "once")
		{
			return true;
		}
	}
	
	return false;
}

/**
 * Handles `#pragma include` directives by loading the specified text files and inserting them in place.
 */
static void handle_includes(text_file& source, std::unordered_set<const text_file*>& include_once, resource_manager* resource_manager)
{
	// For each line in the source
	for (std::size_t i = 0; i < source.size(); ++i)
	{
		std::string token;
		std::istringstream line_stream(source[i]);
		
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
				text_file* include_file = resource_manager->load<text_file>(path);
				if (!include_file)
				{
					source[i] = "#error file not found (" + path + ")";
				}
				else
				{
					// If file has not been included or has no `#pragma once` directive
					if (!include_once.contains(include_file))
					{
						// If file has `#pragma once` directive
						if (has_pragma_once(*include_file))
						{
							// Add file to set of files to include once
							include_once.insert(include_file);
						}
						
						// Create a copy of the include file
						text_file include_file_copy = *include_file;
						
						// Handle `#pragma include` directives inside include file
						handle_includes(include_file_copy, include_once, resource_manager);
						
						// Replace #pragma include directive with include file contents
						source.erase(source.begin() + i);
						source.insert(source.begin() + i, include_file_copy.begin(), include_file_copy.end());
						i += include_file_copy.size() - 1;
					}
				}
			}
			else
			{
				source[i] = "#error malformed include directive (" + source[i] + ")";
			}
		}
	}
}

template <>
gl::shader_program* resource_loader<gl::shader_program>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load shader source file
	text_file* source_file = resource_loader<text_file>::load(resource_manager, file, path);
	
	// Make a copy of the shader source file
	text_file source_file_copy = *source_file;
	
	// Handle `#pragma include` directives
	std::unordered_set<const text_file*> include_once;
	include_once.insert(source_file);
	handle_includes(source_file_copy, include_once, resource_manager);
	
	// Join vector of source lines into single string
	std::ostringstream stream;
	std::copy(source_file_copy.begin(), source_file_copy.end(), std::ostream_iterator<std::string>(stream, "\n"));
	
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

template <>
render::shader_template* resource_loader<render::shader_template>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Load shader template source file
	text_file* source_file = resource_loader<text_file>::load(resource_manager, file, path);
	
	// Make a copy of the shader template source file
	text_file source_file_copy = *source_file;
	
	// Handle `#pragma include` directives
	std::unordered_set<const text_file*> include_once;
	include_once.insert(source_file);
	handle_includes(source_file_copy, include_once, resource_manager);
	
	// Join vector of source lines into single string
	std::ostringstream stream;
	std::copy(source_file_copy.begin(), source_file_copy.end(), std::ostream_iterator<std::string>(stream, "\n"));
	
	// Create shader template
	render::shader_template* shader = new render::shader_template(stream.str());
	
	return shader;
}
