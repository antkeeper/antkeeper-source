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

#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <string>
#include <emergent/emergent.hpp>

using namespace Emergent;

class MaterialLoader;

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();
	Model* load(const std::string& filename);
	
	void setMaterialLoader(MaterialLoader* materialLoader);
	
private:
	struct MaterialGroup
	{
		std::string materialName;
		std::uint32_t indexOffset;
		std::uint32_t triangleCount;
		AABB bounds;
	};
	
	enum VertexFlags
	{
		UV      = 1,
		TANGENT = 2,
		WEIGHTS = 4
	};
	
	struct ModelData
	{
		std::uint32_t groupCount;
		MaterialGroup* groups;
		std::uint32_t vertexFormat;
		std::uint32_t vertexCount;
		AABB bounds;
		float* vertexData;
		std::uint32_t* indexData;
	};
	
	struct BoneData
	{
		std::string name;
		std::uint16_t parent;
		std::uint16_t childCount;
		std::uint16_t* children;
		Vector3 translation;
		Quaternion rotation;
		float length;
	};
	
	struct KeyFrameData
	{
		float time;
		Transform transform;
	};
	
	struct ChannelData
	{
		std::uint16_t id;
		std::uint16_t keyFrameCount;
		KeyFrameData* keyFrames;
	};
	
	struct AnimationData
	{
		std::string name;
		float startTime;
		float endTime;
		std::uint16_t channelCount;
		ChannelData* channels;
	};
	
	struct SkeletonData
	{
		std::uint16_t boneCount;
		BoneData* bones;
		std::uint16_t animationCount;
		AnimationData* animations;
	};
	
	static void constructBoneHierarchy(Bone* bone, const BoneData* data, std::uint16_t index);
	
	MaterialLoader* materialLoader;
};

#endif // MODEL_LOADER_HPP
