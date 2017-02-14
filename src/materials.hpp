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

#ifndef MATERIALS
#define MATERIALS

#include <emergent/emergent.hpp>

using namespace Emergent;

enum class MaterialFormat
{
	UI,
	PHYSICAL
};

class UIMaterial: public Material
{
public:
	UIMaterial(): texture(nullptr) {}
	virtual ~UIMaterial() {}
	
	virtual unsigned int getMaterialFormatID() const;
	
	Texture* texture;
};

inline unsigned int UIMaterial::getMaterialFormatID() const
{
	return static_cast<unsigned int>(MaterialFormat::UI);
}

class PhysicalMaterial: public Material
{
public:
	PhysicalMaterial():
		colorMap(nullptr),
		roughnessMap(nullptr),
		metallicMap(nullptr),
		specularMap(nullptr),
		opacityMap(nullptr),
		normalMap(nullptr)
	{};
	virtual ~PhysicalMaterial() {};
	virtual unsigned int getMaterialFormatID() const;
	
	Vector3 color;
	float roughness;
	float metallic;
	float specular;
	float opacity;
	
	Texture* colorMap;
	Texture* roughnessMap;
	Texture* metallicMap;
	Texture* specularMap;
	Texture* opacityMap;
	Texture* normalMap;
	
	bool shadowCaster;
	bool shadowReceiver;
};

inline unsigned int PhysicalMaterial::getMaterialFormatID() const
{
	return static_cast<unsigned int>(MaterialFormat::PHYSICAL);
}

#endif // MATERIALS