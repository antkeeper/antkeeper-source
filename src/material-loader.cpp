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
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

MaterialLoader::MaterialLoader()
{
	textureLoader.setGamma(1.0f);
	textureLoader.setMipmapChain(false);
	textureLoader.setMaxAnisotropy(1.0f);
}

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
	
	for (auto it = texture2DCache.begin(); it != texture2DCache.end(); ++it)
	{
		delete it->second;
	}
	texture2DCache.clear();
	
	for (auto it = textureCubeCache.begin(); it != textureCubeCache.end(); ++it)
	{
		delete it->second;
	}
	textureCubeCache.clear();
}

Material* MaterialLoader::load(const std::string& filename)
{
	// Check if material exists in cache
	auto it = materialCache.find(filename);
	if (it != materialCache.end())
	{
		return it->second;
	}
	
	// Allocate new material
	Material* material = new Material();
	
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

		// Find position of first character in variable name
		std::size_t variableNamePosition = line.find_first_not_of(whitespace, 0);
		if (variableNamePosition == std::string::npos)
		{
			// Skip whitespace-only lines
			continue;
		}
		
		// Find position of equals sign
		std::size_t equalsSignPosition = line.find_first_of("=", variableNamePosition);
		if (equalsSignPosition == std::string::npos)
		{
			// Skip lines with no equals sign
			continue;
		}
		
		// Find position of first character in variable type
		std::size_t variableTypePosition = line.find_first_not_of(whitespace, equalsSignPosition + 1);
		if (variableTypePosition == std::string::npos)
		{
			// Skip lines with no variable type definition
			continue;
		}
		
		// Count parentheses
		std::size_t leftParenthesisCount = std::count(line.begin() + variableNamePosition, line.end(), '(');
		std::size_t rightParenthesisCount = std::count(line.begin() + variableNamePosition, line.end(), ')');
		if (leftParenthesisCount != rightParenthesisCount || leftParenthesisCount == 0)
		{
			// Skip lines with invalid number of parentheses
			continue;
		}
		
		std::string variableName = line.substr(variableNamePosition, line.find_first_of(" \t=", variableNamePosition) - variableNamePosition);
		std::string variableType = line.substr(variableTypePosition, line.find_first_of(" \t[(", variableTypePosition) - variableTypePosition);
		std::size_t elementCount = leftParenthesisCount;
		
		std::size_t currentPosition = variableTypePosition;
		std::vector<std::vector<std::string>> elements;
		bool invalid = false;
		for (std::size_t i = 0; i < elementCount; ++i)
		{
			std::size_t leftParenthesisPosition = line.find_first_of("(", currentPosition);
			std::size_t rightParenthesisPosition = line.find_first_of(")", leftParenthesisPosition + 1);
			
			if (leftParenthesisPosition == std::string::npos || rightParenthesisPosition == std::string::npos)
			{
				invalid = true;
				break;
			}
			
			currentPosition = leftParenthesisPosition + 1;
			std::size_t argumentCount = std::count(line.begin() + leftParenthesisPosition + 1, line.begin() + rightParenthesisPosition, ',') + 1;
			std::vector<std::string> arguments;
			for (std::size_t j = 0; j < argumentCount; ++j)
			{
				std::size_t argumentStart = line.find_first_not_of(whitespace, currentPosition);
				std::size_t argumentEnd = line.find_first_of(" \t,)", argumentStart + 1);
				
				if (argumentStart == std::string::npos || argumentEnd == std::string::npos)
				{
					// Unable to parse argument
					invalid = true;
					break;
				}
				
				std::string argument = line.substr(argumentStart, argumentEnd - argumentStart);
				arguments.push_back(argument);
				
				currentPosition = argumentEnd + 1;
			}
			
			if (invalid)
			{
				// Unable to parse element
				break;
			}
			
			elements.push_back(arguments);
			
			currentPosition = rightParenthesisPosition + 1;
		}
		
		if (invalid)
		{
			// Unable to parse line
			continue;
		}
		
		if (variableType == "int")
		{
			ShaderInt* variable = material->addVariable<int>(variableName, elements.size());
			loadShaderInt(variable, elements);
		}
		else if (variableType == "float")
		{
			ShaderFloat* variable = material->addVariable<float>(variableName, elements.size());
			loadShaderFloat(variable, elements);
		}
		else if (variableType == "vec2")
		{
			ShaderVector2* variable = material->addVariable<Vector2>(variableName, elements.size());
			loadShaderVector2(variable, elements);
		}
		else if (variableType == "vec3")
		{
			ShaderVector3* variable = material->addVariable<Vector3>(variableName, elements.size());
			loadShaderVector3(variable, elements);
		}
		else if (variableType == "vec4")
		{
			ShaderVector4* variable = material->addVariable<Vector4>(variableName, elements.size());
			loadShaderVector4(variable, elements);
		}
		else if (variableType == "mat3")
		{
			ShaderMatrix3* variable = material->addVariable<Matrix3>(variableName, elements.size());
			loadShaderMatrix3(variable, elements);
		}
		else if (variableType == "mat4")
		{
			ShaderMatrix4* variable = material->addVariable<Matrix4>(variableName, elements.size());
			loadShaderMatrix4(variable, elements);
		}
		else if (variableType == "texture")
		{
			ShaderTexture2D* variable = material->addVariable<const Texture2D*>(variableName, elements.size());
			loadShaderTexture2D(variable, elements);
		}
		else if (variableType == "textureCube")
		{
			ShaderTextureCube* variable = material->addVariable<const TextureCube*>(variableName, elements.size());
			loadShaderTextureCube(variable, elements);
		}
	}
	
	// Close file
	file.close();
	
	// Add material to cache
	materialCache[filename] = material;
	
	return material;
}

Texture2D* MaterialLoader::loadTexture2D(const std::string& filename)
{
	// Check if texture exists in cache
	auto it = texture2DCache.find(filename);
	if (it != texture2DCache.end())
	{
		return it->second;
	}
	
	std::string fullFilename = std::string("data/textures/") + filename;
	
	// Load texture
	Texture2D* texture = textureLoader.load2D(fullFilename);
	if (!texture)
	{
		std::cerr << "MaterialLoader::loadTexture2D(): Failed to load texture file \"" << fullFilename << "\"" << std::endl;
		return nullptr;
	}
	
	// Add texture to cache
	texture2DCache[filename] = texture;
	
	return texture;
}

TextureCube* MaterialLoader::loadTextureCube(const std::string& filename)
{
	// Check if texture exists in cache
	auto it = textureCubeCache.find(filename);
	if (it != textureCubeCache.end())
	{
		return it->second;
	}
	
	std::string fullFilename = std::string("data/textures/") + filename;
	
	// Load texture
	TextureCube* texture = textureLoader.loadCube(fullFilename);
	if (!texture)
	{
		std::cerr << "MaterialLoader::loadTextureCube(): Failed to load texture file \"" << fullFilename << "\"" << std::endl;
		return nullptr;
	}
	
	// Add texture to cache
	textureCubeCache[filename] = texture;
	
	return texture;
}

bool MaterialLoader::loadShaderInt(ShaderInt* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		int value;
		std::stringstream stream;
		stream << elements[i][0];
		stream >> value;
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderFloat(ShaderFloat* variable, const std::vector<std::vector<std::string>>& elements)
{	
	for (int i = 0; i < elements.size(); ++i)
	{
		float value;
		std::stringstream stream;
		stream << elements[i][0];
		stream >> value;
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderVector2(ShaderVector2* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		Vector2 value;
		
		for (int j = 0; j < 2; ++j)
		{
			std::stringstream stream;
			stream << elements[i][j];
			stream >> value[j];
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderVector3(ShaderVector3* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		Vector3 value;
		
		for (int j = 0; j < 3; ++j)
		{
			std::stringstream stream;
			stream << elements[i][j];
			stream >> value[j];
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderVector4(ShaderVector4* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		Vector4 value;
		
		for (int j = 0; j < 4; ++j)
		{
			std::stringstream stream;
			stream << elements[i][j];
			stream >> value[j];
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderMatrix3(ShaderMatrix3* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		Matrix3 value;
		
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				std::stringstream stream;
				stream << elements[i][k * 3 + j];
				stream >> value[j][k];
			}
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderMatrix4(ShaderMatrix4* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		Matrix4 value;
		
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				std::stringstream stream;
				stream << elements[i][k * 4 + j];
				stream >> value[j][k];
			}
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderTexture2D(ShaderTexture2D* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		std::string filename;
		std::stringstream stream;
		stream << elements[i][0];
		stream >> filename;
		
		Texture2D* value = loadTexture2D(filename);
		if (!value)
		{
			std::cerr << "MaterialLoader::loadShaderTexture2D(): Failed to load 2D texture \"" << filename << "\"" << std::endl;
			return false;
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}

bool MaterialLoader::loadShaderTextureCube(ShaderTextureCube* variable, const std::vector<std::vector<std::string>>& elements)
{
	for (int i = 0; i < elements.size(); ++i)
	{
		std::string filename;
		std::stringstream stream;
		stream << elements[i][0];
		stream >> filename;
		
		TextureCube* value = loadTextureCube(filename);
		if (!value)
		{
			std::cerr << "MaterialLoader::loadShaderTextureCube(): Failed to load cube texture \"" << filename << "\"" << std::endl;
			return false;
		}
		
		variable->setValue(i, value);
	}
	
	return true;
}
