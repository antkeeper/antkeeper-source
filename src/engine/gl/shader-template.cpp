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

#include <engine/gl/shader-template.hpp>
#include <algorithm>
#include <engine/gl/shader-object.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/text-file.hpp>
#include <engine/utility/hash/combine.hpp>
#include <sstream>
#include <unordered_set>

namespace gl {

shader_template::shader_template(const text_file& source_code):
	m_template_source{source_code}
{
	find_directives();
	rehash();
}

shader_template::shader_template(text_file&& source_code):
	m_template_source{source_code}
{
	find_directives();
	rehash();
}

shader_template::shader_template(text_file&& source_code, std::vector<std::shared_ptr<text_file>>&& include_files):
	m_template_source{source_code},
	m_include_files{include_files}
{
	find_directives();
	rehash();
}

void shader_template::source(const text_file& source_code)
{
	m_template_source = source_code;
	m_include_files.clear();
	find_directives();
	rehash();
}

void shader_template::source(text_file&& source_code)
{
	m_template_source = source_code;
	m_include_files.clear();
	find_directives();
	rehash();
}

std::string shader_template::configure(gl::shader_stage stage, const dictionary_type& definitions) const
{
	replace_stage_directives(stage);
	replace_define_directives(definitions);
	
	// Join vector of source lines into single string
	std::string string;
	for (const auto& line: m_template_source.lines)
	{
		string += line;
		string += '\n';
	}
	
	return string;
}

std::unique_ptr<gl::shader_object> shader_template::compile(gl::shader_stage stage, const dictionary_type& definitions) const
{
	// Generate shader object source
	const std::string object_source = configure(stage, definitions);
	
	// Create shader object
	std::unique_ptr<gl::shader_object> object = std::make_unique<gl::shader_object>(stage);
	
	// Set shader object source
	object->source(object_source);
	
	// Compile shader object
	object->compile();
	
	return object;
}

std::unique_ptr<gl::shader_program> shader_template::build(const dictionary_type& definitions) const
{
	std::unique_ptr<gl::shader_object> vertex_object;
	std::unique_ptr<gl::shader_object> fragment_object;
	std::unique_ptr<gl::shader_object> geometry_object;
	
	// Create shader program
	std::unique_ptr<gl::shader_program> program = std::make_unique<gl::shader_program>();
	
	if (has_vertex_directive())
	{
		// Compile vertex shader object and attach to shader program
		vertex_object = compile(gl::shader_stage::vertex, definitions);
		program->attach(*vertex_object);
	}
	
	if (has_fragment_directive())
	{
		// Compile fragment shader object and attach to shader program
		fragment_object = compile(gl::shader_stage::fragment, definitions);
		program->attach(*fragment_object);
	}
	
	if (has_geometry_directive())
	{
		// Compile fragment shader object and attach to shader program
		geometry_object = compile(gl::shader_stage::geometry, definitions);
		program->attach(*geometry_object);
	}
	
	// Link attached shader objects into shader program
	program->link();
	
	// Detach all attached shader objects
	program->detach_all();
	
	return program;
}

void shader_template::find_directives()
{
	// Reset directives
	m_vertex_directives.clear();
	m_fragment_directives.clear();
	m_geometry_directives.clear();
	m_define_directives.clear();
	
	// Parse directives
	for (std::size_t i = 0; i < m_template_source.lines.size(); ++i)
	{
		std::istringstream line_stream(m_template_source.lines[i]);
		std::string token;
		
		// Detect `#pragma` directives
		if (line_stream >> token && token == "#pragma")
		{
			if (line_stream >> token)
			{
				// Map line numbers of supported directives
				if (token == "define")
				{
					if (line_stream >> token)
					{
						m_define_directives.insert({token, i});
					}
				}
				else if (token == "vertex")
				{
					m_vertex_directives.insert(i);
				}
				else if (token == "fragment")
				{
					m_fragment_directives.insert(i);
				}
				else if (token == "geometry")
				{
					m_geometry_directives.insert(i);
				}
			}
		}
	}
}

void shader_template::rehash()
{
	m_hash = 0;
	for (const auto& line: m_template_source.lines)
	{
		m_hash = hash::combine(m_hash, std::hash<std::string>{}(line));
	}
}

void shader_template::replace_stage_directives(gl::shader_stage stage) const
{
	// Determine stage directives according to the shader stage being generated
	const char* vertex_directive = (stage == gl::shader_stage::vertex) ? "#define __VERTEX__" : "/* #undef __VERTEX__ */";
	const char* fragment_directive = (stage == gl::shader_stage::fragment) ? "#define __FRAGMENT__" : "/* #undef __FRAGMENT__ */";
	const char* geometry_directive = (stage == gl::shader_stage::geometry) ? "#define __GEOMETRY__" : "/* #undef __GEOMETRY__ */";
	
	// Handle `#pragma <stage>` directives
	for (const auto directive_line: m_vertex_directives)
	{
		m_template_source.lines[directive_line] = vertex_directive;
	}
	for (const auto directive_line: m_fragment_directives)
	{
		m_template_source.lines[directive_line] = fragment_directive;
	}
	for (const auto directive_line: m_geometry_directives)
	{
		m_template_source.lines[directive_line] = geometry_directive;
	}
}

void shader_template::replace_define_directives(const dictionary_type& definitions) const
{
	// For each `#pragma define <key>` directive
	for (const auto& define_directive: m_define_directives)
	{
		// Get a reference to the directive line
		std::string& line = m_template_source.lines[define_directive.second];
		
		// Check if the corresponding definition was given by the configuration
		auto definitions_it = definitions.find(define_directive.first);
		if (definitions_it != definitions.end())
		{
			// Definition found, replace `#pragma define <key>` with `#define <key>` or `#define <key> <value>`
			line = "#define " + define_directive.first;
			if (!definitions_it->second.empty())
			{
				line += " " + definitions_it->second;
			}
		}
		else
		{
			// Definition not found, replace `#pragma define <key>` with the comment `/* #undef <key> */`.
			line = "/* #undef " + define_directive.first + " */";
		}
	}
}

bool shader_template::has_define_directive(const std::string& key) const
{
	return (m_define_directives.find(key) != m_define_directives.end());
}

} // namespace gl

/**
 * Scans a text file for the presence of a `#pragma once` directive.
 *
 * @param source Text file to scan.
 *
 * @return `true` if the file contains a `#pragma once` directive, `false` otherwise.
 */
static bool has_pragma_once(const text_file& source)
{
	for (const auto& line: source.lines)
	{
		std::istringstream line_stream(line);
		std::string token;
		
		// If line contains a `#pragma once` directive
		if (line_stream >> token && token == "#pragma")
		{
			if (line_stream >> token && token == "once")
			{
				return true;
			}
		}
	}
	
	return false;
}

/**
 * Handles `#pragma include` directives by loading the specified text files and inserting them in place.
 */
static void handle_includes(std::vector<std::shared_ptr<text_file>>& include_files, text_file& source, std::unordered_set<std::filesystem::path>& include_once, resource_manager& resource_manager)
{
	// For each line in the source
	for (std::size_t i = 0; i < source.lines.size(); ++i)
	{
		std::string token;
		std::istringstream line_stream(source.lines[i]);
		
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
				const std::filesystem::path path = token.substr(1, token.length() - 2);
				
				// Skip pre-included files that contain a `#pragma once` directive
				if (include_once.contains(path))
				{
					source.lines[i] = "/* #pragma exclude " + token + " */";
					continue;
				}
				
				// Load include file
				auto include_file = resource_manager.load<text_file>(path);
				if (!include_file)
				{
					source.lines[i] = "#error file not found: " + path.string();
					continue;
				}
				else
				{
					include_files.emplace_back(include_file);
				}
				
				// If file has `#pragma once` directive
				if (has_pragma_once(*include_file))
				{
					// Add file to set of files to include once
					include_once.insert(path);
				}
				
				// Create a copy of the include file
				text_file include_file_copy = *include_file;
				
				// Handle `#pragma include` directives inside include file
				handle_includes(include_files, include_file_copy, include_once, resource_manager);
				
				// Replace #pragma include directive with include file contents
				source.lines.erase(source.lines.begin() + i);
				source.lines.insert(source.lines.begin() + i, include_file_copy.lines.begin(), include_file_copy.lines.end());
				i += include_file_copy.lines.size() - 1;
			}
			else
			{
				source.lines[i] = "#error malformed include directive: \"" + source.lines[i] + "\"";
			}
		}
	}
}

template <>
std::unique_ptr<gl::shader_template> resource_loader<gl::shader_template>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Load shader template source file
	const auto source_file = resource_loader<text_file>::load(resource_manager, ctx);
	
	// Make a copy of the shader template source file
	text_file source_file_copy = *source_file;
	
	std::vector<std::shared_ptr<text_file>> include_files;
	
	// Handle `#pragma include` directives
	std::unordered_set<std::filesystem::path> include_once;
	include_once.insert(ctx.path());
	handle_includes(include_files, source_file_copy, include_once, resource_manager);
	
	// Construct shader template
	return std::make_unique<gl::shader_template>(std::move(source_file_copy), std::move(include_files));
}
