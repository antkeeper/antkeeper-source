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


#include "model-loader.hpp"
#include "material-loader.hpp"
#include <fstream>

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

ModelLoader::ModelLoader():
	materialLoader(nullptr)
{}

ModelLoader::~ModelLoader()
{}

Model* ModelLoader::load(const std::string& filename)
{
	// Open file
	std::ifstream file(filename.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
	if (!file.is_open())
	{
		std::cerr << "ModelLoader::load(): Failed to open model file \"" << filename << "\"" << std::endl;
		return nullptr;
	}
	
	// Allocate file data buffer
	int filesize = file.tellg();
	unsigned char* buffer = new unsigned char[filesize];
		
	// Read file data into buffer
	file.seekg(0, file.beg);
	file.read(reinterpret_cast<char*>(&buffer[0]), filesize);
	unsigned char* bufferOffset = &buffer[0];
	
	// Close file
	file.close();
	
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
					read16(&channel->keyFrameCount, &bufferOffset);
					
					// Allocate keyframes
					channel->keyFrames = new KeyFrameData[channel->keyFrameCount];
					
					// Read keyframes
					for (std::size_t k = 0; k < channel->keyFrameCount; ++k)
					{
						KeyFrameData* keyFrame = &channel->keyFrames[k];
						
						// Read keyframe time
						read32(&keyFrame->time, &bufferOffset);
						
						// Read keyframe translation
						read32(&keyFrame->transform.translation.x, &bufferOffset);
						read32(&keyFrame->transform.translation.y, &bufferOffset);
						read32(&keyFrame->transform.translation.z, &bufferOffset);
						
						// Read keyframe rotation
						read32(&keyFrame->transform.rotation.w, &bufferOffset);
						read32(&keyFrame->transform.rotation.x, &bufferOffset);
						read32(&keyFrame->transform.rotation.y, &bufferOffset);
						read32(&keyFrame->transform.rotation.z, &bufferOffset);
						
						// Read keyframe scale
						read32(&keyFrame->transform.scale.x, &bufferOffset);
						read32(&keyFrame->transform.scale.y, &bufferOffset);
						read32(&keyFrame->transform.scale.z, &bufferOffset);
					}
				}
			}
		}
	}
	
	// Free file data buffer
	delete[] buffer;
	
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
	glEnableVertexAttribArray(EMERGENT_VERTEX_POSITION);
	glVertexAttribPointer(EMERGENT_VERTEX_POSITION, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
	attribOffset += attribSize;
	
	// Vertex normal attribute
	attribSize = 3;
	glEnableVertexAttribArray(EMERGENT_VERTEX_NORMAL);
	glVertexAttribPointer(EMERGENT_VERTEX_NORMAL, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
	attribOffset += attribSize;
	
	// Vertex UV attribute
	if ((modelData->vertexFormat & UV) != 0)
	{
		attribSize = 2;
		glEnableVertexAttribArray(EMERGENT_VERTEX_TEXCOORD);
		glVertexAttribPointer(EMERGENT_VERTEX_TEXCOORD, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	
	// Vertex tangent and bitangent attributes
	if ((modelData->vertexFormat & TANGENT) != 0)
	{
		// Tangent
		attribSize = 4;
		glEnableVertexAttribArray(EMERGENT_VERTEX_TANGENT);
		glVertexAttribPointer(EMERGENT_VERTEX_TANGENT, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
		
		// Bitangent
		attribSize = 4;
		glEnableVertexAttribArray(EMERGENT_VERTEX_BITANGENT);
		glVertexAttribPointer(EMERGENT_VERTEX_BITANGENT, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	
	// Vertex indices and weights attributes
	if ((modelData->vertexFormat & TANGENT) != 0)
	{
		// Indices
		attribSize = 4;
		glEnableVertexAttribArray(EMERGENT_VERTEX_BONE_INDICES);
		glVertexAttribPointer(EMERGENT_VERTEX_BONE_INDICES, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
		
		// Weights
		attribSize = 4;
		glEnableVertexAttribArray(EMERGENT_VERTEX_BONE_WEIGHTS);
		glVertexAttribPointer(EMERGENT_VERTEX_BONE_WEIGHTS, attribSize, GL_FLOAT, GL_FALSE, sizeof(float) * vertexSize, (char*)0 + attribOffset * sizeof(float));
		attribOffset += attribSize;
	}
	
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
		ModelLoader::MaterialGroup* modelDataGroup = &modelData->groups[i];
		
		// Allocate model group
		Model::Group* modelGroup = new Model::Group();
		
		// Set model group name
		modelGroup->name = modelDataGroup->materialName;
		
		// Load material
		std::string materialFilename = std::string("data/materials/") + modelDataGroup->materialName + std::string(".mtl");
		if (materialLoader != nullptr)
		{
			modelGroup->material = materialLoader->load(materialFilename);
			if (!modelGroup->material)
			{
				std::cerr << "ModelLoader::load(): Failed to load material file \"" << materialFilename << "\" for model file \"" << filename << "\"" << std::endl;
			}
		}
		else
		{
			modelGroup->material = nullptr;
			std::cerr << "ModelLoader::load(): No valid material loader, material file \"" << materialFilename << "\" not loaded" << std::endl;
		}
		
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
			Animation* animation = new Animation();
			animation->setName(animationData->name);
			animation->setTimeFrame(animationData->startTime, animationData->endTime);
			
			for (std::size_t j = 0; j < animationData->channelCount; ++j)
			{
				ChannelData* channelData = &animationData->channels[j];
				AnimationChannel* channel = animation->createChannel(channelData->id);
				for (std::size_t k = 0; k < channelData->keyFrameCount; ++k)
				{
					KeyFrameData* keyFrameData = &channelData->keyFrames[k];
					KeyFrame* keyFrame = channel->insertKeyFrame(keyFrameData->time);
					keyFrame->setTransform(keyFrameData->transform);
				}
			}
			
			// Add animation to skeleton
			skeleton->addAnimation(animation);
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
				delete[] animation->channels[j].keyFrames;
			}
			
			delete[] animation->channels;
		}
		delete[] skeletonData->animations;
		
		delete skeletonData;
	}
	
	return model;
}

void ModelLoader::setMaterialLoader(MaterialLoader* materialLoader)
{
	this->materialLoader = materialLoader;
}

void ModelLoader::constructBoneHierarchy(Bone* bone, const BoneData* data, std::uint16_t index)
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
