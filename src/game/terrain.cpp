#include "terrain.hpp"

Terrain::Terrain()
{
	surfaceOctree = nullptr;
}

Terrain::~Terrain()
{
	delete surfaceOctree;
}

void Terrain::create(int columns, int rows, const Vector3& dimensions)
{
	this->columns = columns;
	this->rows = rows;
	this->dimensions = dimensions;
	
	createSurface();
	createSubsurface();
}

void Terrain::createSurface()
{
	surfaceVertexSize = 3 + 3 + 2;
	surfaceVertexCount = (columns + 1) * (rows + 1);
	surfaceTriangleCount = columns * rows * 2;
	surfaceIndexCount = surfaceTriangleCount * 3;	
	surfaceVertexData = new float[surfaceVertexSize * surfaceVertexCount];
	surfaceIndexData = new std::uint32_t[surfaceIndexCount];
	surfaceVertices.resize(surfaceVertexCount);
	surfaceIndices.resize(surfaceIndexCount);
	
	// Calculate scale and offset
	Vector2 scale(dimensions.x / (float)columns, dimensions.z / (float)rows);
	Vector2 offset(dimensions.x * -0.5f, dimensions.z * -0.5f);
	
	// Calculate vertex positions
	for (int i = 0; i <= rows; ++i)
	{
		for (int j = 0; j <= columns; ++j)
		{
			std::size_t index = i * (columns + 1) + j;
			
			Vector3* vertex = &surfaceVertices[index];
			vertex->x = (float)j * scale.x + offset.x;
			vertex->y = 0.0f;
			vertex->z = (float)i * scale.y + offset.y;
			
			float* data = &surfaceVertexData[index * surfaceVertexSize];
			*(data++) = vertex->x;
			*(data++) = vertex->y;
			*(data++) = vertex->z;
			*(data++) = 0.0f;
			*(data++) = 1.0f;
			*(data++) = 0.0f;
			*(data++) = static_cast<float>(j) / static_cast<float>(columns);
			*(data++) = static_cast<float>(i) / static_cast<float>(rows);
		}
	}
	
	// Generate indices
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			unsigned int a = i * (columns + 1) + j;
			unsigned int b = (i + 1) * (columns + 1) + j;
			unsigned int c = i * (columns + 1) + j + 1;
			unsigned int d = (i + 1) * (columns + 1) + j + 1;
			
			std::size_t index = (i * columns + j) * 2 * 3;
			surfaceIndices[index++] = a;
			surfaceIndices[index++] = b;
			surfaceIndices[index++] = c;
			surfaceIndices[index++] = c;
			surfaceIndices[index++] = b;
			surfaceIndices[index] = d;
		}
	}
	
	// Generate index data
	for (std::size_t i = 0; i < surfaceIndexCount; ++i)
	{
		surfaceIndexData[i] = surfaceIndices[i];
	}
	
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
	
	// Calculate octree
	surfaceOctree = surfaceNavmesh.createOctree(5);
}

void Terrain::createSubsurface()
{
	subsurfaceVertexSize = 3 + 3 + 2;
	subsurfaceVertexCount = (columns + 1) * 4 + (rows + 1) * 4;
	subsurfaceTriangleCount = columns * 4 + rows * 4 + 2;
	subsurfaceIndexCount = subsurfaceTriangleCount * 3;	
	subsurfaceVertexData = new float[subsurfaceVertexSize * subsurfaceVertexCount];
	subsurfaceIndexData = new std::uint32_t[subsurfaceIndexCount];
	subsurfaceVertices.resize(subsurfaceVertexCount);
	subsurfaceIndices.resize(subsurfaceIndexCount);
	
	float maxDimension = dimensions.y;
	float textureScaleX = dimensions.x / maxDimension;
	float textureScaleY = dimensions.y / maxDimension;
	float textureScaleZ = dimensions.z / maxDimension;
		
	// Calculate floor position
	float subsurfaceFloor = -dimensions.y;
	
	// Calculate vertex positions
	Vector3* vertex = &subsurfaceVertices[0];
	float* data = &subsurfaceVertexData[0];
	
	// Top row
	for (int j = 0; j <= columns; ++j)
	{
		int i = 0;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		const Vector3& surfaceVertex = surfaceVertices[surfaceIndex];
		float u = 1.0f - (static_cast<float>(j) / static_cast<float>(columns)) * textureScaleX;
		
		*(vertex++) = surfaceVertex;
		*(data++) = surfaceVertex.x;
		*(data++) = surfaceVertex.y;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = 0.0f;
		
		*(vertex++) = Vector3(surfaceVertex.x, subsurfaceFloor, surfaceVertex.z);
		*(data++) = surfaceVertex.x;
		*(data++) = subsurfaceFloor;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = textureScaleY;
	}
	
	// Bottom row
	for (int j = 0; j <= columns; ++j)
	{
		int i = rows;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		const Vector3& surfaceVertex = surfaceVertices[surfaceIndex];
		float u = (static_cast<float>(j) / static_cast<float>(columns)) * textureScaleX;
		
		*(vertex++) = surfaceVertex;
		*(data++) = surfaceVertex.x;
		*(data++) = surfaceVertex.y;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = 0.0f;
		
		*(vertex++) = Vector3(surfaceVertex.x, subsurfaceFloor, surfaceVertex.z);
		*(data++) = surfaceVertex.x;
		*(data++) = subsurfaceFloor;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = textureScaleY;
	}
	
	// Left column
	for (int i = 0; i <= rows; ++i)
	{
		int j = 0;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		const Vector3& surfaceVertex = surfaceVertices[surfaceIndex];
		float u = (static_cast<float>(i) / static_cast<float>(rows)) * textureScaleZ;
		
		*(vertex++) = surfaceVertex;
		*(data++) = surfaceVertex.x;
		*(data++) = surfaceVertex.y;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = 0.0f;
		
		*(vertex++) = Vector3(surfaceVertex.x, subsurfaceFloor, surfaceVertex.z);
		*(data++) = surfaceVertex.x;
		*(data++) = subsurfaceFloor;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = textureScaleY;
	}
	
	// Right column
	for (int i = 0; i <= rows; ++i)
	{
		int j = columns;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		const Vector3& surfaceVertex = surfaceVertices[surfaceIndex];
		float u = 1.0f - (static_cast<float>(i) / static_cast<float>(rows)) * textureScaleZ;

		*(vertex++) = surfaceVertex;
		*(data++) = surfaceVertex.x;
		*(data++) = surfaceVertex.y;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = 0.0f;
		
		*(vertex++) = Vector3(surfaceVertex.x, subsurfaceFloor, surfaceVertex.z);
		*(data++) = surfaceVertex.x;
		*(data++) = subsurfaceFloor;
		*(data++) = surfaceVertex.z;
		*(data++) = 0.0f;
		*(data++) = 0.0f;
		*(data++) = 1.0f;
		*(data++) = u;
		*(data++) = textureScaleY;
	}
	
	// Generate indices
	std::size_t* index = &subsurfaceIndices[0];
	
	for (int i = 0; i < columns; ++i)
	{
		std::size_t a = i * 2;
		std::size_t b = i * 2 + 1;
		std::size_t c = (i + 1) * 2;
		std::size_t d = (i + 1) * 2 + 1;
		
		(*(index++)) = b;
		(*(index++)) = a;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = d;
		
		a += (columns + 1) * 2;
		b += (columns + 1) * 2;
		c += (columns + 1) * 2;
		d += (columns + 1) * 2;
		
		(*(index++)) = a;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = d;
	}
	
	for (int i = 0; i < rows; ++i)
	{
		std::size_t a = (columns + 1) * 4 + i * 2;
		std::size_t b = (columns + 1) * 4 + i * 2 + 1;
		std::size_t c = (columns + 1) * 4 + (i + 1) * 2;
		std::size_t d = (columns + 1) * 4 + (i + 1) * 2 + 1;
		
		(*(index++)) = a;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = d;
		
		a += (rows + 1) * 2;
		b += (rows + 1) * 2;
		c += (rows + 1) * 2;
		d += (rows + 1) * 2;
		
		(*(index++)) = b;
		(*(index++)) = a;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = d;
	}
	
	// Floor
	std::size_t a = 1;
	std::size_t b = 1 + (rows + 1) * 2;
	std::size_t c = columns * 2 + 1;
	std::size_t d = columns * 2 + 1 + (columns + 1) * 2;
	(*(index++)) = a;
	(*(index++)) = c;
	(*(index++)) = b;
	(*(index++)) = b;
	(*(index++)) = c;
	(*(index++)) = d;
	
	// Generate index data
	for (std::size_t i = 0; i < subsurfaceIndexCount; ++i)
	{
		subsurfaceIndexData[i] = subsurfaceIndices[i];
	}
	
	// Generate navmesh
	subsurfaceNavmesh.create(subsurfaceVertices, subsurfaceIndices);
	
	// Create and load VAO, VBO, and IBO
	glGenVertexArrays(1, &subsurfaceVAO);
	glBindVertexArray(subsurfaceVAO);
	glGenBuffers(1, &subsurfaceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, subsurfaceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * subsurfaceVertexSize * subsurfaceVertexCount, subsurfaceVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(EMERGENT_VERTEX_POSITION);
	glVertexAttribPointer(EMERGENT_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, subsurfaceVertexSize * sizeof(float), (char*)0 + 0 * sizeof(float));
	glEnableVertexAttribArray(EMERGENT_VERTEX_NORMAL);
	glVertexAttribPointer(EMERGENT_VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, subsurfaceVertexSize * sizeof(float), (char*)0 + 3 * sizeof(float));
	glEnableVertexAttribArray(EMERGENT_VERTEX_TEXCOORD);
	glVertexAttribPointer(EMERGENT_VERTEX_TEXCOORD, 2, GL_FLOAT, GL_FALSE, subsurfaceVertexSize * sizeof(float), (char*)0 + 6 * sizeof(float));
	glGenBuffers(1, &subsurfaceIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subsurfaceIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * subsurfaceIndexCount, subsurfaceIndexData, GL_STATIC_DRAW);
	
	// Setup material
	//subsurfaceMaterial.flags = static_cast<unsigned int>(PhysicalMaterial::Flags::SOIL);
	
	// Setup buffers
	subsurfaceModel.setVAO(subsurfaceVAO);
	subsurfaceModel.setVBO(subsurfaceVBO);
	subsurfaceModel.setIBO(subsurfaceIBO);
	
	// Create model group
	Model::Group* group = new Model::Group();
	group->name = "default";
	group->material = nullptr;//&subsurfaceMaterial;
	group->indexOffset = 0;
	group->triangleCount = subsurfaceTriangleCount;
	
	// Add group to the model
	subsurfaceModel.addGroup(group);
	
	// Set model bounds
	subsurfaceModel.setBounds(subsurfaceNavmesh.getBounds());
}

void Terrain::calculateSurfaceNormals()
{
	for (std::size_t i = 0; i < surfaceVertexCount; ++i)
	{
		const Navmesh::Vertex* vertex = (*surfaceNavmesh.getVertices())[i];
		
		Vector3 normal(0.0f);
		const Navmesh::Edge* start = vertex->edge;
		const Navmesh::Edge* e = start;
		do
		{
			normal += e->triangle->normal;
			e = e->previous->symmetric;
		}
		while (e != start && e != nullptr);
		normal = glm::normalize(normal);
		
		float* data = &surfaceVertexData[i * surfaceVertexSize];
		data[3] = normal.x;
		data[4] = normal.y;
		data[5] = normal.z;
	}
}

bool Terrain::load(const std::string& filename)
{
	int width;
	int height;
	int channels;
	
	stbi_set_flip_vertically_on_load(true);
	
	// Load image data
	unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &channels, 1);
	
	if (width != columns + 1 || height != rows + 1)
	{
		// Free loaded image data
		stbi_image_free(pixels);
		return false;
	}
	
	// Set surface vertex heights
	for (int i = 0; i <= rows; ++i)
	{
		for (int j = 0; j <= columns; ++j)
		{
			std::size_t index = i * (columns + 1) + j;
			
			float elevation = (float)pixels[index] / 255.0f * 5.0f;
			
			surfaceVertexData[index * surfaceVertexSize + 1] = elevation;
			surfaceVertices[index].y = elevation;
			(*surfaceNavmesh.getVertices())[index]->position.y = elevation;
		}
	}
	
	// Free loaded image data
	stbi_image_free(pixels);
	
	// Set subsurface vertex heights
	std::size_t subsurfaceIndex = 0;
	
	// Top row
	for (int j = 0; j <= columns; ++j)
	{
		int i = 0;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		float elevation = surfaceVertices[surfaceIndex].y;
		
		subsurfaceVertexData[subsurfaceIndex * subsurfaceVertexSize + 1] = elevation;
		subsurfaceVertices[subsurfaceIndex].y = elevation;
		(*subsurfaceNavmesh.getVertices())[subsurfaceIndex]->position.y = elevation;
		subsurfaceIndex += 2;
	}
	// Bottom row
	for (int j = 0; j <= columns; ++j)
	{
		int i = rows;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		float elevation = surfaceVertices[surfaceIndex].y;
		
		subsurfaceVertexData[subsurfaceIndex * subsurfaceVertexSize + 1] = elevation;
		subsurfaceVertices[subsurfaceIndex].y = elevation;
		(*subsurfaceNavmesh.getVertices())[subsurfaceIndex]->position.y = elevation;
		subsurfaceIndex += 2;
	}
	// Left column
	for (int i = 0; i <= rows; ++i)
	{
		int j = 0;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		float elevation = surfaceVertices[surfaceIndex].y;
		
		subsurfaceVertexData[subsurfaceIndex * subsurfaceVertexSize + 1] = elevation;
		subsurfaceVertices[subsurfaceIndex].y = elevation;
		(*subsurfaceNavmesh.getVertices())[subsurfaceIndex]->position.y = elevation;
		subsurfaceIndex += 2;
	}
	// Right column
	for (int i = 0; i <= rows; ++i)
	{
		int j = columns;
		std::size_t surfaceIndex = i * (columns + 1) + j;
		float elevation = surfaceVertices[surfaceIndex].y;
		
		subsurfaceVertexData[subsurfaceIndex * subsurfaceVertexSize + 1] = elevation;
		subsurfaceVertices[subsurfaceIndex].y = elevation;
		(*subsurfaceNavmesh.getVertices())[subsurfaceIndex]->position.y = elevation;
		subsurfaceIndex += 2;
	}
	
	// Calculate navmesh normals
	surfaceNavmesh.calculateNormals();
	subsurfaceNavmesh.calculateNormals();
	
	// Calculate navmesh bounds
	surfaceNavmesh.calculateBounds();
	subsurfaceNavmesh.calculateBounds();
	
	// Calculate vertex normals
	calculateSurfaceNormals();
	
	// Update VBOs
	glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, surfaceVertexCount * surfaceVertexSize * sizeof(float), surfaceVertexData);
	glBindBuffer(GL_ARRAY_BUFFER, subsurfaceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, subsurfaceVertexCount * subsurfaceVertexSize * sizeof(float), subsurfaceVertexData);
	
	// Update bounds
	surfaceModel.setBounds(surfaceNavmesh.getBounds());
	subsurfaceModel.setBounds(subsurfaceNavmesh.getBounds());
	
	// Calculate octree
	delete surfaceOctree;
	surfaceOctree = surfaceNavmesh.createOctree(5);
	
	return true;
}

struct voxel
{
	glm::vec3 vertices[8];
	float values[8];
};

// LUT to map isosurface vertices to intersecting edges
static const int EDGE_TABLE[256] =
{
	0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};

static const int TRIANGLE_TABLE[256][16] =
{
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// Lookup table which contains the indices of the vertices which define an edge.
static const int VERTEX_TABLE[12][2] =
{
	{0, 1},
	{1, 2},
	{2, 3},
	{3, 0},
	
	{4, 5},
	{5, 6},
	{6, 7},
	{7, 4},
	
	{0, 4},
	{1, 5},
	{2, 6},
	{3, 7}
};

/*
 * The marching cubes algorithm can produce a maximum of 5 triangles per cell. Therefore the maximum triangle count of a grid is `w * h * d * 5`.
 */

struct triangle
{
	glm::vec3 vertices[3];
};

bool less_than(const glm::vec3& a, const glm::vec3& b)
{
	if (a.x < b.x)
		return true;
	else if (a.x > b.x)
		return false;
	if (a.y < b.y)
		return true;
	else if (a.y > b.y)
		return false;
	if (a.z < b.z)
		return true;
	return false;
}

glm::vec3 interpolate(float isolevel, glm::vec3 p0, glm::vec3 p1, float v0, float v1)
{
	static const float epsilon = 0.00001f;
	
	if (less_than(p1, p0))
	{
		glm::vec3 ptemp = p0;
		p0 = p1;
		p1 = ptemp;
		
		float vtemp = v0;
		v0 = v1;
		v1 = vtemp;
	}
	
	if (std::fabs(v0 - v1) > epsilon)
	{
		return p0 + ((p1 - p0) / (v1 - v0) * (isolevel - v0));
	}
	
	return p0;
}

int polygonize(const voxel& vox, float isolevel, triangle* triangles)
{
	// Set bitflags for each of the cube's 8 vertices, indicating whether or not they are inside the isosurface.
	int edge_index = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (vox.values[i] < isolevel)
			edge_index |= (1 << i);
	}
	
	// Get edge flags from lookup table
	int edge_flags = EDGE_TABLE[edge_index];
	
	if (edge_flags == 0)
	{
		// No intersections, cube is completely in or out of the isosurface.
		return 0;
	}
	
	// Calculate vertex positions
	glm::vec3 vertices[12];
	
	// For each edge
	for (int i = 0; i < 12; ++i)
	{
		// If this edge is intersected
		if (edge_flags & (1 << i))
		{
			int a = VERTEX_TABLE[i][0];
			int b = VERTEX_TABLE[i][1];
			vertices[i] = interpolate(isolevel, vox.vertices[a], vox.vertices[b], vox.values[a], vox.values[b]);
		}
	}
	
	// Form triangles
	int triangle_count = 0;
	for (int i = 0; TRIANGLE_TABLE[edge_index][i] != -1; i += 3)
	{
		int a = TRIANGLE_TABLE[edge_index][i];
		int b = TRIANGLE_TABLE[edge_index][i + 1];
		int c = TRIANGLE_TABLE[edge_index][i + 2];
		triangles[triangle_count].vertices[0] = vertices[a];
		triangles[triangle_count].vertices[1] = vertices[b];
		triangles[triangle_count].vertices[2] = vertices[c];
		++triangle_count;
	}
	
	return triangle_count;
}


