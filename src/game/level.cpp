#include "level.hpp"
#include "../settings.hpp"
#include "../configuration.hpp"

#include <dirent.h>
#include <iostream>
#include <sstream>

LevelParameterSet::LevelParameterSet()
{}

LevelParameterSet::~LevelParameterSet()
{}

bool LevelParameterSet::load(const std::string& filename)
{
	this->filename = filename;
	
	ParameterDict parameters;
	if (!parameters.load(filename))
	{
		return false;
	}
	
	parameters.get("biome", &biome);
	parameters.get("heightmap", &heightmap);
	
	return true;
}

Level::Level()
{
	terrain.create(255, 255, Vector3(ANTKEEPER_TERRAIN_WIDTH, ANTKEEPER_TERRAIN_BASE_HEIGHT, ANTKEEPER_TERRAIN_DEPTH));
}

Level::~Level()
{}
	
bool Level::load(const LevelParameterSet& params)
{
	// Load terrain from heightmap
	std::string heightmapFilename = std::string("data/textures/") + params.heightmap;
	if (!terrain.load(heightmapFilename))
	{
		std::cerr << "Failed to load terrain from heightmap file \"" << heightmapFilename << "\" for level \"" << params.filename << "\"" << std::endl;
		return false;
	}
	
	//application->currentLevelTerrain->getSurfaceModel()->getGroup(0)->material = application->materialLoader->load("data/materials/debug-terrain-surface.mtl");
	
	// Setup terrain surface model instance
	terrainSurface.setModel(terrain.getSurfaceModel());
	terrainSurface.setTranslation(Vector3(0, 0, 0));
	
	// Setup terrain subsurface model instance
	terrainSubsurface.setModel(terrain.getSubsurfaceModel());
	terrainSubsurface.setTranslation(Vector3(0, 0, 0));

	return true;
}

Campaign::Campaign()
{}

Campaign::~Campaign()
{}

bool Campaign::load(const std::string& directory)
{
	// Open levels directory
	DIR* dir = opendir(directory.c_str());
	if (dir == nullptr)
	{
		std::cout << "Failed to open levels directory \"" << directory << "\"" << std::endl;
		return false;
	}
	
	// Scan directory for .lvl files
	for (struct dirent* entry = readdir(dir); entry != nullptr; entry = readdir(dir))
	{
		if (entry->d_type == DT_DIR || *entry->d_name == '.')
		{
			continue;
		}
		
		std::string filename = entry->d_name;
		std::string::size_type delimeter = filename.find_last_of('.');
		if (delimeter == std::string::npos)
		{
			continue;
		}
		
		std::string extension = filename.substr(delimeter + 1);
		if (extension != "lvl")
		{
			continue;
		}
		
		std::string worldIndexString = filename.substr(0, 2);
		std::string levelIndexString = filename.substr(3, 2);
		int worldIndex = -1;
		int levelIndex = -1;
		
		std::stringstream stream;
		stream << worldIndexString;
		stream >> worldIndex;
		worldIndex -= 1;
		
		stream.str(std::string());
		stream.clear();
		stream << levelIndexString;
		stream >> levelIndex;
		levelIndex -= 1;
		
		if (worldIndex < 0 || levelIndex < 0)
		{
			std::cout << "Invalid level parameters file \"" << filename << "\"" << std::endl;
			continue;
		}
		
		// Resize vector to accommodate maximum world index
		if (worldIndex >= static_cast<int>(levelParameterSets.size()))
		{
			levelParameterSets.resize(worldIndex + 1);
		}
		
		// Resize vector to accommodate maximum level file index
		if (levelIndex >= static_cast<int>(levelParameterSets[worldIndex].size()))
		{
			levelParameterSets[worldIndex].resize(levelIndex + 1);
		}
		
		// Load level parameters
		LevelParameterSet* levelParams = &levelParameterSets[worldIndex][levelIndex];
		if (!levelParams->load(directory + filename))
		{
			std::cout << "Failed to load parameters for level " << (worldIndex + 1) << "-" << (levelIndex + 1) << std::endl;
		}
		else
		{
			std::cout << "Loaded level parameters for level " << (worldIndex + 1) << "-" << (levelIndex + 1) << std::endl;
		}
	}
	
	// Close levels directory
	closedir(dir);
	
	return true;
}
