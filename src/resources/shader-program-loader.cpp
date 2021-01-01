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
#include "rasterizer/shader-type.hpp"
#include "rasterizer/shader.hpp"
#include "rasterizer/shader-program.hpp"
#include <sstream>
#include <iterator>

/**
 * Tokenizes a line of shader source code.
 */
static std::vector<std::string> tokenize(const std::string& line)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream linestream(line);
	while (linestream >> token)
		tokens.push_back(token);
	
	return tokens;
}

/**
 * Handles `#pragma include` directives by loading the specified text files and inserting them in place.
 */
static void handle_includes(text_file* source, resource_manager* resource_manager)
{
	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		// Tokenize line
		std::vector<std::string> tokens = tokenize((*source)[i]);

		// Look for `#pragma include` directives
		if (tokens.size() == 3 && tokens[0] == "#pragma" && tokens[1] == "include")
		{
			// Get path to include file
			std::string path = tokens[2].substr(1, tokens[2].length() - 2);

			// Load include file
			if (!resource_manager->load<text_file>(path))
			{
				std::string message = std::string("Failed to load shader include file \"") + path + std::string("\"");
				throw std::runtime_error(message.c_str());
			}
			text_file include_file = *(resource_manager->load<text_file>(path));

			// Handle `#pragma include` directives inside include file
			handle_includes(&include_file, resource_manager);
			
			// Replace #pragma include directive with include file contents
			source->erase(source->begin() + i);
			source->insert(source->begin() + i, include_file.begin(), include_file.end());
			i += include_file.size() - 1;
		}
	}
}

/**
 * Injects the `DEBUG` or `NDEBUG` macros after the `#version` directive.
 */
static void inject_debug_macro(text_file* source)
{
	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		// Tokenize line
		std::vector<std::string> tokens = tokenize((*source)[i]);

		// Inject DEBUG and NDEBUG macros
		if (!tokens.empty() && tokens[0] == "#version")
		{
			#if defined(NDEBUG)
				source->insert(source->begin() + i + 1, "#define NDEBUG");
			#else
				source->insert(source->begin() + i + 1, "#define DEBUG");
			#endif

			return;
		}
	}
}

/**
 * Injects a shader type macro definition after the `#version` directive.
 */
static void inject_shader_type_macro(text_file* source, shader_type type)
{
	const char* vertex_macro = "#define _VERTEX";
	const char* fragment_macro = "#define _FRAGMENT";
	const char* geometry_macro = "#define _GEOMETRY";

	const char* macro = nullptr;
	if (type == shader_type::vertex)
		macro = vertex_macro;
	else if (type == shader_type::fragment)
		macro = fragment_macro;
	else if (type == shader_type::geometry)
		macro = geometry_macro;

	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		// Tokenize line
		std::vector<std::string> tokens = tokenize((*source)[i]);

		// Inject shader type macro
		if (!tokens.empty() && tokens[0] == "#version")
		{
			source->insert(source->begin() + i + 1, macro);
			return;
		}
	}
}

static std::list<shader_type> detect_shader_types(text_file* source)
{
	std::list<shader_type> types;

	// For each line in the source
	for (std::size_t i = 0; i < source->size(); ++i)
	{
		// Tokenize line
		std::vector<std::string> tokens = tokenize((*source)[i]);

		// Look for `#pragma include` directives
		if (tokens.size() == 2 && tokens[0] == "#pragma")
		{
			if (tokens[1] == "vertex")
			{
				types.push_back(shader_type::vertex);
			}
			else if (tokens[1] == "fragment")
			{
				types.push_back(shader_type::fragment);
			}
			else if (tokens[1] == "geometry")
			{
				types.push_back(shader_type::geometry);
			}
		}
	}

	return types;
}

static std::string generate_source_buffer(const std::vector<std::string>& source)
{
	std::ostringstream stream;
	std::copy(source.begin(), source.end(), std::ostream_iterator<std::string>(stream, "\n"));
	return stream.str();
}

template <>
shader_program* resource_loader<shader_program>::load(resource_manager* resource_manager, PHYSFS_File* file)
{
	// Load shader source
	text_file* source = resource_loader<text_file>::load(resource_manager, file);

	// Handle `#pragma include` directives
	handle_includes(source, resource_manager);

	// Inject `DEBUG` or `NDEBUG` macro definitions
	inject_debug_macro(source);

	// Detect declared shader types via the `#pragma vertex`, `#pragma fragment` and `#pragma geometry` directives
	std::list<shader_type> shader_types = detect_shader_types(source);

	// Load detected shaders
	std::list<shader*> shaders;
	for (shader_type type: shader_types)
	{
		text_file type_source = *source;
		inject_shader_type_macro(&type_source, type);
		std::string source_buffer = generate_source_buffer(type_source);
		shaders.push_back(new shader(type, source_buffer));
	}

	// Create shader program
	shader_program* program = new shader_program(shaders);

	// Delete shaders
	for (shader* shader: shaders)
	{
		delete shader;
	}

	return program;
}

