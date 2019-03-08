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

#include "terrain-system.hpp"
#include "graphics/vertex-format.hpp"

TerrainSystem::TerrainSystem(ComponentManager* componentManager):
	System(componentManager),
	terrainCreationGroup(componentManager),
	terrainGroup(componentManager)
{
	terrainCreationGroup.addGroupObserver(this);
	terrainGroup.addGroupObserver(this);

	patchSize = 100.0f;
}

TerrainSystem::~TerrainSystem()
{}

void TerrainSystem::update(float t, float dt)
{

	auto members = terrainGroup.getMembers();	
	for (const TerrainGroup::Member* member: *members)
	{
		ModelComponent* model = std::get<0>(member->components);
		TerrainPatchComponent* patch = std::get<1>(member->components);
		TransformComponent* transform = std::get<2>(member->components);
	}
}

void TerrainSystem::memberRegistered(const TerrainCreationGroup::Member* member)
{
	TerrainPatchComponent* patch = std::get<0>(member->components);
	TransformComponent* transform = std::get<1>(member->components);

	// Generate a subdivided plane mesh
	TriangleMesh* patchMesh = generatePlane(5);

	// Generate a model from the subdivided plane
	Model* patchModel = generateModel(patchMesh);

	// Add model component to the entity
	ModelComponent* model = new ModelComponent();
	model->model.setModel(patchModel);
	componentManager->addComponent(member->entity, model);

	// Set scale of the transform component
	transform->transform.scale = Vector3(patchSize);
	transform->transform.translation = Vector3(std::get<0>(patch->position), 0.0f, std::get<1>(patch->position)) * patchSize;
}

void TerrainSystem::memberUnregistered(const TerrainCreationGroup::Member* member)
{}

void TerrainSystem::memberRegistered(const TerrainGroup::Member* member)
{
	// Add terrain patch to the patch map
	TerrainPatchComponent* patch = std::get<1>(member->components);
	patchMap[patch->position] = member;
}

void TerrainSystem::memberUnregistered(const TerrainGroup::Member* member)
{
	// Remove terrain patch from the patch map
	TerrainPatchComponent* patch = std::get<1>(member->components);
	auto it = patchMap.find(patch->position);
	if (it != patchMap.end())
	{
		patchMap.erase(it);
	}

	// TODO: free created terrain patch model
}

TriangleMesh* TerrainSystem::generatePlane(int subdivisions)
{
	//std::size_t quadCount = std::pow(4, subdivisions);
	//std::size_t triangleCount = quadCount * 2;
	std::size_t columns = std::pow(2, subdivisions);
	std::size_t rows = columns;
	std::size_t vertexCount = (columns + 1) * (rows + 1);
	float vertexIncrement = 1.0f / static_cast<float>(columns);

	// Generate vertices
	std::vector<Vector3> vertices;
	Vector3 position(0.0f);
	position.z = -0.5f;
	for (std::size_t i = 0; i <= rows; ++i)
	{
		position.x = -0.5f;
		for (std::size_t j = 0; j <= columns; ++j)
		{
			vertices.push_back(position);
			position.x += vertexIncrement;

		}

		position.z += vertexIncrement;
	}

	// Generate indices
	std::vector<std::size_t> indices;
	for (std::size_t i = 0; i < rows; ++i)
	{
		for (std::size_t j = 0; j < columns; ++j)
		{
			unsigned int a = i * (columns + 1) + j;
			unsigned int b = (i + 1) * (columns + 1) + j;
			unsigned int c = i * (columns + 1) + j + 1;
			unsigned int d = (i + 1) * (columns + 1) + j + 1;
			
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(c);
			indices.push_back(b);
			indices.push_back(d);
		}
	}

	return new TriangleMesh(vertices, indices);

	/*
	// Generate navmesh
	surfaceNavmesh.create(surfaceVertices, surfaceIndices);
	
	// Calculate vertex normals
	calculateSurfaceNormals();
	
	// Create and load VAO, VBO, and IBO
	glGenVertexArrays(1, &surfaceVAO);
	glBindVertexArray(surfaceVAO);
	glGenBuffers(1, &surfaceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * surfaceVertexSize * surfaceVertexCount, surfaceVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(EMERGENT_VERTEX_POSITION);
	glVertexAttribPointer(EMERGENT_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, surfaceVertexSize * sizeof(float), (char*)0 + 0 * sizeof(float));
	glEnableVertexAttribArray(EMERGENT_VERTEX_NORMAL);
	glVertexAttribPointer(EMERGENT_VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, surfaceVertexSize * sizeof(float), (char*)0 + 3 * sizeof(float));
	glEnableVertexAttribArray(EMERGENT_VERTEX_TEXCOORD);
	glVertexAttribPointer(EMERGENT_VERTEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, surfaceVertexSize * sizeof(float), (char*)0 + 6 * sizeof(float));
	glGenBuffers(1, &surfaceIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surfaceIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * surfaceIndexCount, surfaceIndexData, GL_STATIC_DRAW);
	
	// Setup material
	//surfaceMaterial.flags = static_cast<unsigned int>(PhysicalMaterial::Flags::OBJECT);
	
	// Setup buffers
	surfaceModel.setVAO(surfaceVAO);
	surfaceModel.setVBO(surfaceVBO);
	surfaceModel.setIBO(surfaceIBO);
	
	// Create model group
	Model::Group* group = new Model::Group();
	group->name = "default";
	group->material = nullptr;//&surfaceMaterial;
	group->indexOffset = 0;
	group->triangleCount = surfaceTriangleCount;
	
	// Add group to the model
	surfaceModel.addGroup(group);
	
	// Set model bounds
	surfaceModel.setBounds(surfaceNavmesh.getBounds());
	*/
}

Model* TerrainSystem::generateModel(TriangleMesh* mesh)
{
	std::size_t triangleCount = mesh->getTriangles()->size();
	// Vertex position + vertex normal
	std::size_t vertexSize = 3 + 3;
	std::size_t vertexCount = triangleCount * 3;

	#if defined(DEBUG)
		const Vector3 barycentricCoordinates[3] =
		{
			Vector3(1, 0, 0),
			Vector3(0, 1, 0),
			Vector3(0, 0, 1)
		};

		vertexSize += 3;
	#endif // DEBUG

	// Generate vertex data
	float* vertexData = new float[vertexSize * vertexCount];
	float* v = vertexData;
	for (std::size_t i = 0; i < triangleCount; ++i)
	{
		const TriangleMesh::Triangle* triangle = (*mesh->getTriangles())[i];

		const TriangleMesh::Vertex* a = triangle->edge->vertex;
		const TriangleMesh::Vertex* b = triangle->edge->next->vertex;
		const TriangleMesh::Vertex* c = triangle->edge->previous->vertex;
		const TriangleMesh::Vertex* abc[] = {a, b, c};
		const Vector3& normal = triangle->normal;


		for (std::size_t j = 0; j < 3; ++j)
		{
			*(v++) = abc[j]->position[0];
			*(v++) = abc[j]->position[1];
			*(v++) = abc[j]->position[2];
			*(v++) = normal.x;
			*(v++) = normal.y;
			*(v++) = normal.z;

			#if defined(DEBUG)
			{
				*(v++) = barycentricCoordinates[j].x;
				*(v++) = barycentricCoordinates[j].y;
				*(v++) = barycentricCoordinates[j].z;
			}
			#endif // DEBUG
		}
			
		// Calculate smoothed vertex normal
		/*
		Vector3 normal(0.0f);
		TriangleMesh::Edge* start = vertex->edge;
		TriangleMesh::Edge* e = start;
		do
		{
			normal += e->triangle->normal;
			e = e->previous->symmetric;
		}
		while (e != start && e != nullptr);
		normal = glm::normalize(normal);
		*/
	}

	// Generate index data
	std::size_t indexCount = triangleCount * 3;
	std::uint32_t* indexData = new std::uint32_t[indexCount];
	std::uint32_t* index = indexData;
	for (std::size_t i = 0; i < triangleCount; ++i)
	{
		*(index++) = i * 3;
		*(index++) = i * 3 + 1;
		*(index++) = i * 3 + 2;
	}

	// Calculate AABB bounds
	AABB bounds;
	bounds.setMin(mesh->getVertices()->front()->position);
	bounds.setMax(mesh->getVertices()->front()->position);
	for (TriangleMesh::Vertex* vertex: *mesh->getVertices())
	{
		bounds.add(vertex->position);
	}

	GLuint vao;
	GLuint vbo;
	GLuint ibo;

	// Generate and bind VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate and bind VBO, then upload vertex data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize * vertexCount, vertexData, GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * indexCount, indexData, GL_STATIC_DRAW);

	// Delete vertex and index data
	delete[] vertexData;
	delete[] indexData;
	
	// Create model
	Model* model = new Model();
	
	// Set model buffers
	model->setVAO(vao);
	model->setVBO(vbo);
	model->setIBO(ibo);

	// Set model bounds
	model->setBounds(bounds);

	// Create model material
	Material* material = new Material();
	material->setShader(nullptr);
	ShaderVariable<Vector3>* albedo = material->addVariable<Vector3>("albedo");
	ShaderVariable<float>* roughness = material->addVariable<float>("roughness");
	ShaderVariable<float>* metalness = material->addVariable<float>("metalness");
	ShaderVariable<float>* opacity = material->addVariable<float>("opacity");
	albedo->setValue(Vector3(0.8f));
	roughness->setValue(0.5f);
	metalness->setValue(0.0f);
	opacity->setValue(1.0f);
	
	// Create model group
	Model::Group* group = new Model::Group();
	group->name = std::string();
	group->material = material;
	group->indexOffset = 0;
	group->triangleCount = triangleCount;
	
	// Add model group to the model
	model->addGroup(group);

	return model;
}

void TerrainSystem::projectMesh(TriangleMesh* mesh)
{

}

