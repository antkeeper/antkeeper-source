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

#ifndef MATERIAL_LOADER_HPP
#define MATERIAL_LOADER_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include <map>
#include <string>

class MaterialLoader
{
public:
	MaterialLoader();
	~MaterialLoader();
	
	void unload();
	
	Material* load(const std::string& filename);
	
private:
	Shader* loadShader(const std::string& filename);
	Texture2D* loadTexture2D(const std::string& filename);
	TextureCube* loadTextureCube(const std::string& filename);
	
	bool loadShaderInt(ShaderInt* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderFloat(ShaderFloat* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderVector2(ShaderVector2* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderVector3(ShaderVector3* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderVector4(ShaderVector4* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderMatrix3(ShaderMatrix3* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderMatrix4(ShaderMatrix4* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderTexture2D(ShaderTexture2D* variable, const std::vector<std::vector<std::string>>& elements);
	bool loadShaderTextureCube(ShaderTextureCube* variable, const std::vector<std::vector<std::string>>& elements);
	
	std::map<std::string, Shader*> shaderCache;
	std::map<std::string, Texture2D*> texture2DCache;
	std::map<std::string, TextureCube*> textureCubeCache;
	std::map<std::string, Material*> materialCache;
	
	TextureLoader textureLoader;
};

#endif // MATERIAL_LOADER_HPP
