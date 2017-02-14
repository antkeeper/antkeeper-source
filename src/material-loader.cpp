/*
 * Copyright (C) 2017  Christopher J. Howard
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

#include "material-loader.hpp"
#include <fstream>
#include <sstream>
#include <vector>

MaterialLoader::MaterialLoader()
{}

MaterialLoader::~MaterialLoader()
{
	unload();
}

void MaterialLoader::unload()
{
	for (auto it = materialCache.begin(); it != materialCache.end(); ++it)
	{
		delete it->second;
	}
	materialCache.clear();
	
	for (auto it = textureCache.begin(); it != textureCache.end(); ++it)
	{
		delete it->second;
	}
	textureCache.clear();
}

PhysicalMaterial* MaterialLoader::load(const std::string& filename)
{
	// Check if material exists in cache
	auto it = materialCache.find(filename);
	if (it != materialCache.end())
	{
		return it->second;
	}
	
	// Allocate new material
	PhysicalMaterial* material = new PhysicalMaterial();
	
	// Open file
	std::ifstream file(filename.c_str(), std::ifstream::in);
	if (!file.is_open())
	{
		std::cerr << "MaterialLoader::load(): Failed to open material file \"" << filename << "\"" << std::endl;
		delete material;
		return nullptr;
	}
	
	// Parse lines
	std::string line;
	while (file.good() && std::getline(file, line))
	{
		const std::string whitespace = " \t";
		
		// Skip empty lines
		if (line.empty())
		{
			continue;
		}

		// Find position of first character in command string
		std::size_t firstCommand = line.find_first_not_of(whitespace, 0);
		if (firstCommand == std::string::npos)
		{
			continue;
		}
		
		// Find position of first character in delimeter string
		std::size_t firstDelimeter = line.find_first_of(whitespace, firstCommand);
		if (firstDelimeter == std::string::npos)
		{
			continue;
		}
		
		// Find position of first character in arguments string
		std::size_t firstArgument = line.find_first_not_of(whitespace, firstDelimeter);
		if (firstArgument == std::string::npos)
		{
			firstArgument = firstDelimeter + 1;
		}
		
		// Form command string and argument list string
		std::string command = line.substr(firstCommand, firstDelimeter - firstCommand);
		std::string argumentList = line.substr(firstArgument);
		
		// Form vector of argument strings
		std::vector<std::string> arguments;
		std::istringstream argumentStream(argumentList);
		std::string argument;
		while (argumentStream >> argument)
		{
			arguments.push_back(argument);
		}
		
		if (command == "color" && arguments.size() == 3)
		{
			std::stringstream(arguments[0]) >> material->color.x;
			std::stringstream(arguments[1]) >> material->color.y;
			std::stringstream(arguments[2]) >> material->color.z;
		}
		else if (command == "roughness" && arguments.size() == 1)
		{
			std::stringstream(arguments[0]) >> material->roughness;
		}
		else if (command == "metallic" && arguments.size() == 1)
		{
			std::stringstream(arguments[0]) >> material->metallic;
		}
		else if (command == "specular" && arguments.size() == 1)
		{
			std::stringstream(arguments[0]) >> material->specular;
		}
		else if (command == "opacity" && arguments.size() == 1)
		{
			std::stringstream(arguments[0]) >> material->opacity;
		}
		else if (command == "color-map")
		{
			material->colorMap = loadTexture(argumentList);
		}
		else if (command == "roughness-map")
		{
			material->roughnessMap = loadTexture(argumentList);
		}
		else if (command == "metallic-map")
		{
			material->metallicMap = loadTexture(argumentList);
		}
		else if (command == "specular-map")
		{
			material->specularMap = loadTexture(argumentList);
		}
		else if (command == "opacity-map")
		{
			material->opacityMap = loadTexture(argumentList);
		}
		else if (command == "normal-map")
		{
			material->normalMap = loadTexture(argumentList);
		}
		else if (command[0] != '#')
		{
			std::cerr << "MaterialLoader::load(): Invalid line \"" << line << "\" in file \"" << filename << "\"" << std::endl;
		}
	}
	
	// Close file
	file.close();
	
	// Add material to cache
	materialCache[filename] = material;
	
	return material;
}

Texture* MaterialLoader::loadTexture(const std::string& filename)
{
	// Check if texture exists in cache
	auto it = textureCache.find(filename);
	if (it != textureCache.end())
	{
		return it->second;
	}
	
	// Load texture
	std::string fullFilename = std::string("data/textures/") + filename;
	Texture* texture = new Texture();
	if (!texture->load(fullFilename))
	{
		std::cerr << "MaterialLoader::loadTexture(): Failed to load texture file \"" << fullFilename << "\"" << std::endl;
		delete texture;
		return nullptr;
	}
	
	// Add texture to cache
	textureCache[filename] = texture;
	
	return texture;
}
