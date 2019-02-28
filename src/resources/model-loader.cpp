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
#include "../graphics/vertex-format.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

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

struct KeyframeData
{
	float time;
	Transform transform;
};

struct ChannelData
{
	std::uint16_t id;
	std::uint16_t keyframeCount;
	KeyframeData* keyframes;
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

static void constructBoneHierarchy(Bone* bone, const BoneData* data, std::uint16_t index)
{
	bone->setName(data[index].name);
	
	Transform transform;
	transform.translation = data[index].translation;
	transform.rotation = data[index].rotation;
	transform.scale = Vector3(1.0f);
	
	bone->setRelativeTransform(transform);
	bone->setLength(data[index].length);
	
	for (std::uint16_t i = 0; i < data[index].childCount; ++i)
	{
		constructBoneHierarchy(bone->createChild(), data, data[index].children[i]);
	}
}

template <typename T>
inline static void read8(T* result, unsigned char** data)
{
	std::uint8_t temp = (*data)[0];
	*result = *reinterpret_cast<T*>(&temp);
	*data += 1;
}

template <typename T>
inline static void read16(T* result, unsigned char** data)
{
	std::uint16_t temp = ((*data)[0] << 0) | ((*data)[1] << 8);
	*result = *reinterpret_cast<T*>(&temp);
	*data += 2;
}

template <typename T>
inline static void read32(T* result, unsigned char** data)
{
	std::uint32_t temp = ((*data)[0] << 0) | ((*data)[1] << 8) | ((*data)[2] << 16) | ((*data)[3] << 24);
	*result = *reinterpret_cast<T*>(&temp);
	*data += 4;
}

inline static void readString(std::string* result, unsigned char** data)
{
	result->resize((*data)[0]);
	for (std::size_t i = 0; i < result->size(); ++i)
	{
		(*result)[i] = (*data)[i + 1];
	}
	
	*data += result->size() + 1;
}

template <>
Model* ResourceLoader<Model>::load(ResourceManager* resourceManager, std::istream* is)
{
<<<<<<< HEAD:src/resources/model-loader.cpp
=======
	// Open file
	std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
	if (!file.is_open())
	{
		std::cerr << std::string("ModelLoader::load(): Failed to open model file \"") << filename << std::string("\"") << std::endl;
		return nullptr;
	}
	
>>>>>>> df8405f4e83febb81a5ce8f772bd7f5b9e9b6036:src/model-loader.cpp
	// Allocate file data buffer
	is->seekg(0, is->end);
	int filesize = is->tellg();
	unsigned char* buffer = new unsigned char[filesize];
		
	// Read file data into buffer
	is->seekg(0, is->beg);
	is->read(reinterpret_cast<char*>(&buffer[0]), filesize);
	unsigned char* bufferOffset = &buffer[0];
	
	// Allocate model data
	ModelData* modelData = new ModelData();
	SkeletonData* skeletonData = nullptr;
	
	// Allocate material groups
	read32(&modelData->groupCount, &bufferOffset);
	modelData->groups = new MaterialGroup[modelData->groupCount];
	
	// Read material groups (and calculate triangle count)
	std::uint32_t triangleCount = 0;
	for (std::size_t i = 0; i < modelData->groupCount; ++i)
	{
		MaterialGroup* group = &modelData->groups[i];
		
		readString(&group->materialName, &bufferOffset);
		read32(&group->indexOffset, &bufferOffset);
		read32(&group->triangleCount, &bufferOffset);
		
		// Read bounds
		Vector3 min;
		Vector3 max;
		read32(&min.x, &bufferOffset);
		read32(&min.y, &bufferOffset);
		read32(&min.z, &bufferOffset);
		read32(&max.x, &bufferOffset);
		read32(&max.y, &bufferOffset);
		read32(&max.z, &bufferOffset);
		group->bounds.setMin(min);
		group->bounds.setMax(max);
		
		triangleCount += group->triangleCount;
	}
		
	// Read vertex format and count
	read32(&modelData->vertexFormat, &bufferOffset);
	read32(&modelData->vertexCount, &bufferOffset);
	
	// Read bounds
	Vector3 min;
	Vector3 max;
	read32(&min.x, &bufferOffset);
	read32(&min.y, &bufferOffset);
	read32(&min.z, &bufferOffset);
	read32(&max.x, &bufferOffset);
	read32(&max.y, &bufferOffset);
	read32(&max.z, &bufferOffset);
	modelData->bounds.setMin(min);
	modelData->bounds.setMax(max);
	
	// Calculate vertex size
	std::uint32_t vertexSize =
		3 // Position
		+ 3 // Normal
		+ 2 * ((modelData->vertexFormat & UV) != 0) // UV
		+ 4 * ((modelData->vertexFormat & TANGENT) != 0) // Tangent
		+ 4 * ((modelData->vertexFormat & TANGENT) != 0) // Bitangent
		+ 4 * ((modelData->vertexFormat & WEIGHTS) != 0) // Indices
		+ 4 * ((modelData->vertexFormat & WEIGHTS) != 0); // Weights
	
	// Allocate vertex data
	modelData->vertexData = new float[modelData->vertexCount * vertexSize];
	
	// Read vertex data
	float* vertexDataOffset = &modelData->vertexData[0];
	for (std::size_t i = 0; i < modelData->vertexCount; ++i)
	{
		for (std::size_t j = 0; j < vertexSize; ++j)
		{
			read32(vertexDataOffset, &bufferOffset);
			++vertexDataOffset;
		}
	}
	
	// Allocate index data
	std::uint32_t indexCount = triangleCount * 3;
	modelData->indexData = new std::uint32_t[indexCount];
	
	// Read index data
	for (std::size_t i = 0; i < indexCount; ++i)
	{
		read32(&modelData->indexData[i], &bufferOffset);
	}
	
	// Read skeleton data
	if (modelData->vertexFormat & WEIGHTS)
	{
		// Allocate skeleton data
		skeletonData = new SkeletonData();
		skeletonData->animations = nullptr;
		
		// Read bone count
		read16(&skeletonData->boneCount, &bufferOffset);
		
		// Allocate bones
		skeletonData->bones = new BoneData[skeletonData->boneCount];
		
		// Read bones
		for (std::size_t i = 0; i < skeletonData->boneCount; ++i)
		{
			BoneData* bone = &skeletonData->bones[i];
			bone->children = nullptr;
			
			readString(&bone->name, &bufferOffset);
			read16(&bone->parent, &bufferOffset);
			read16(&bone->childCount, &bufferOffset);
			bone->children = new std::uint16_t[bone->childCount];
			for (std::size_t j = 0; j < bone->childCount; ++j)
			{
				read16(&bone->children[j], &bufferOffset);
			}
			read32(&bone->translation.x, &bufferOffset);
			read32(&bone->translation.y, &bufferOffset);
			read32(&bone->translation.z, &bufferOffset);
			read32(&bone->rotation.w, &bufferOffset);
			read32(&bone->rotation.x, &bufferOffset);
			read32(&bone->rotation.y, &bufferOffset);
			read32(&bone->rotation.z, &bufferOffset);
			read32(&bone->length, &bufferOffset);
		}
		
		// Read animation count
		read16(&skeletonData->animationCount, &bufferOffset);
		
		if (skeletonData->animationCount != 0)
		{
			// Allocate animations
			skeletonData->animations = new AnimationData[skeletonData->animationCount];
			
			// Read animations
			for (std::size_t i = 0; i < skeletonData->animationCount; ++i)
			{
				AnimationData* animation = &skeletonData->animations[i];
				
				// Read animation name
				readString(&animation->name, &bufferOffset);
				
				// Read time frame
				read32(&animation->startTime, &bufferOffset);
				read32(&animation->endTime, &bufferOffset);
				
				// Read channel count
				read16(&animation->channelCount, &bufferOffset);
				
				// Allocate channels
				animation->channels = new ChannelData[animation->channelCount];
				
				// Read channels
				for (std::size_t j = 0; j < animation->channelCount; ++j)
				{
					ChannelData* channel = &animation->channels[j];
					
					// Read channel ID
					read16(&channel->id, &bufferOffset);
					
					// Read keyframe count
					read16(&channel->keyframeCount, &bufferOffset);
					
					// Allocate keyframes
					channel->keyframes = new KeyframeData[channel->keyframeCount];
					
					// Read keyframes
					for (std::size_t k = 0; k < channel->keyframeCount; ++k)
					{
						KeyframeData* keyframe = &channel->keyframes[k];
						
						// Read keyframe time
						read32(&keyframe->time, &bufferOffset);
						
						// Read keyframe translation
						read32(&keyframe->transform.translation.x, &bufferOffset);
						read32(&keyframe->transform.translation.y, &bufferOffset);
						read32(&keyframe->transform.translation.z, &bufferOffset);
						
						// Read keyframe rotation
						read32(&keyframe->transform.rotation.w, &bufferOffset);
						read32(&keyframe->transform.rotation.x, &bufferOffset);
						read32(&keyframe->transform.rotation.y, &bufferOffset);
						read32(&keyframe->transform.rotation.z, &bufferOffset);
						
						// Read keyframe scale
						read32(&keyframe->transform.scale.x, &bufferOffset);
						read32(&keyframe->transform.scale.y, &bufferOffset);
						read32(&keyframe->transform.scale.z, &bufferOffset);
					}
				}
			}
		}
	}
	
	// Free file data buffer
	delete[] buffer;

	#if defined(DEBUG)
		std::uint32_t newVertexCount = triangleCount * 3;
		std::uint32_t newVertexSize = vertexSize + 3;
		float* newVertexData = new float[newVertexCount * newVertexSize];
		const Vector3 barycentricCoordinates[3] =
		{
			Vector3(1, 0, 0),
			Vector3(0, 1, 0),
			Vector3(0, 0, 1)
		};

		for (std::size_t i = 0; i < indexCount; i += 3)
		{
			// For each triangle vertex
			for (std::size_t j = 0; j < 3; ++j)
			{
				float* oldVertex = &modelData->vertexData[modelData->indexData[i + j] * vertexSize];
				float* newVertex = &newVertexData[(i + j) * newVertexSize];

				// Copy old vertex data
				for (std::size_t k = 0; k < vertexSize; ++k)
				{
					*(newVertex++) = *(oldVertex++);
				}

				// Add barycentric coordinates
				*(newVertex++) = barycentricCoordinates[j].x;
				*(newVertex++) = barycentricCoordinates[j].y;
				*(newVertex) = barycentricCoordinates[j].z;

				// Reassign indices
				modelData->indexData[i + j] = i + j;
			}
		}

		// Replace old vertex buffer with new vertex buffer
		vertexSize = newVertexSize;
		delete[] modelData->vertexData;
		modelData->vertexData = newVertexData;
		modelData->vertexCount = newVertexCount;
	#endif // DEBUG

	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	
	// Generate and bind VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Generate and bind VBO, then upload vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize * modelData->vertexCount, modelData->vertexData, GL_STATIC_DRAW);

	// Setup vertex attribute arrays
	std::size_t attribOffset = 0;
	std::size_t attribSize = 0;
	
	// Vertex position attribute
	attribSize = 3;
	glEnableVertexAttribArray(VERTEX_POSITION);
	glVertexAttribPointer(VERTEX_POSITION, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
	attribOffset += attribSize;
	
	// Vertex normal attribute
	attribSize = 3;
	glEnableVertexAttribArray(VERTEX_NORMAL);
	glVertexAttribPointer(VERTEX_NORMAL, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
	attribOffset += attribSize;
	
	// Vertex UV attribute
	if ((modelData->vertexFormat & UV) != 0)
	{
		attribSize = 2;
		glEnableVertexAttribArray(VERTEX_TEXCOORD);
		glVertexAttribPointer(VERTEX_TEXCOORD, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	
	// Vertex tangent and bitangent attributes
	if ((modelData->vertexFormat & TANGENT) != 0)
	{
		// Tangent
		attribSize = 4;
		glEnableVertexAttribArray(VERTEX_TANGENT);
		glVertexAttribPointer(VERTEX_TANGENT, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
		
		// Bitangent
		attribSize = 4;
		glEnableVertexAttribArray(VERTEX_BITANGENT);
		glVertexAttribPointer(VERTEX_BITANGENT, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	
	// Vertex indices and weights attributes
	if ((modelData->vertexFormat & WEIGHTS) != 0)
	{
		// Indices
		attribSize = 4;
		glEnableVertexAttribArray(VERTEX_BONE_INDICES);
		glVertexAttribPointer(VERTEX_BONE_INDICES, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
		
		// Weights
		attribSize = 4;
		glEnableVertexAttribArray(VERTEX_BONE_WEIGHTS);
		glVertexAttribPointer(VERTEX_BONE_WEIGHTS, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}

	#if defined(DEBUG)
	{
		// Vertex barycentric coordinates attribute
		attribSize = 3;
		glEnableVertexAttribArray(VERTEX_BARYCENTRIC);
		glVertexAttribPointer(VERTEX_BARYCENTRIC, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	#endif // DEBUG
	
	// Generate and bind IBO, then upload index data
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * indexCount, modelData->indexData, GL_STATIC_DRAW);

	// Delete vertex and index data
	delete[] modelData->vertexData;
	delete[] modelData->indexData;
		
	// Allocate model
	Model* model = new Model();
	model->setVAO(vao);
	model->setVBO(vbo);
	model->setIBO(ibo);
	model->setVertexFormat(modelData->vertexFormat);
	model->setBounds(modelData->bounds);
	
	// Create model groups
	for (std::size_t i = 0; i < modelData->groupCount; ++i)
	{
		MaterialGroup* modelDataGroup = &modelData->groups[i];
		
		// Allocate model group
		Model::Group* modelGroup = new Model::Group();
		
		// Set model group name
		modelGroup->name = modelDataGroup->materialName;
		
		// Load material
<<<<<<< HEAD:src/resources/model-loader.cpp
		std::string materialFilename = modelDataGroup->materialName + std::string(".mtl");
		modelGroup->material = resourceManager->load<Material>(materialFilename);
=======
		std::string materialFilename = std::string("data/materials/") + modelDataGroup->materialName + std::string(".mtl");
		if (materialLoader != nullptr)
		{
			modelGroup->material = materialLoader->load(materialFilename);
			if (!modelGroup->material)
			{
				std::cerr << std::string("ModelLoader::load(): Failed to load material file \"") << materialFilename << std::string("\" for model file \"") << filename << std::string("\"") << std::endl;
			}
		}
		else
		{
			modelGroup->material = nullptr;
			std::cerr << std::string("ModelLoader::load(): No valid material loader, material file \"") << materialFilename << std::string("\" not loaded") << std::endl;
		}
>>>>>>> df8405f4e83febb81a5ce8f772bd7f5b9e9b6036:src/model-loader.cpp
		
		// Setup model group geometry
		modelGroup->indexOffset = modelDataGroup->indexOffset;
		modelGroup->triangleCount = modelDataGroup->triangleCount;
		modelGroup->bounds = modelDataGroup->bounds;
		
		// Add model group to model
		model->addGroup(modelGroup);
	}
	
	// Create skeleton
	if (skeletonData != nullptr)
	{
		// Allocate skeleton
		Skeleton* skeleton = new Skeleton();
		
		// Construct bone hierarchy from bone data
		constructBoneHierarchy(skeleton->getRootBone(), skeletonData->bones, 0);
		
		// Calculate bind pose
		skeleton->calculateBindPose();
		
		// Create animations
		for (std::size_t i = 0; i < skeletonData->animationCount; ++i)
		{
			AnimationData* animationData = &skeletonData->animations[i];
			AnimationClip<Transform>* clip = new AnimationClip<Transform>();
			clip->setInterpolator(lerp<Transform>);
			
			for (std::size_t j = 0; j < animationData->channelCount; ++j)
			{
				ChannelData* channelData = &animationData->channels[j];
				AnimationChannel<Transform>* channel = clip->addChannel(channelData->id);
				for (std::size_t k = 0; k < channelData->keyframeCount; ++k)
				{
					KeyframeData* keyframeData = &channelData->keyframes[k];
					channel->insertKeyframe(keyframeData->time, keyframeData->transform);
				}
			}
			
			// Add animation clip to skeleton
			skeleton->addAnimationClip(animationData->name, clip);
		}
		
		// Add skeleton to model
		model->setSkeleton(skeleton);
	}
	
	// Delete model data groups
	delete[] modelData->groups;
	
	// Delete model data
	delete modelData;
	
	// Delete skeleton data
	if (skeletonData != nullptr)
	{
		for (std::size_t i = 0; i < skeletonData->boneCount; ++i)
		{
			delete[] skeletonData->bones[i].children;
		}
		delete[] skeletonData->bones;
		
		for (std::size_t i = 0; i < skeletonData->animationCount; ++i)
		{
			AnimationData* animation = &skeletonData->animations[i];
			for (std::size_t j = 0; j < animation->channelCount; ++j)
			{
				delete[] animation->channels[j].keyframes;
			}
			
			delete[] animation->channels;
		}
		delete[] skeletonData->animations;
		
		delete skeletonData;
	}
	
	return model;
}

