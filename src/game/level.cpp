#include "level.hpp"
#include "../settings.hpp"

#if defined(_WIN32) || defined(__CYGWIN__)
#include "../windows-dirent.h"
#else
#include <dirent.h>
#endif

#include <iostream>
#include <sstream>

Level::Level():
	worldIndex(-1),
	levelIndex(-1)
{}

Level::~Level()
{}

bool Level::load()
{
	ParameterDict parameters;
	if (!parameters.load(filename))
	{
		return false;
	}
	
	parameters.get("biome", &biome);
	parameters.get("heightmap", &heightmap);
	
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
		
		stream.str(std::string());
		stream.clear();
		stream << levelIndexString;
		stream >> levelIndex;
		
		if (worldIndex < 0 || levelIndex < 0)
		{
			std::cout << "Invalid level \"" << filename << "\"" << std::endl;
			continue;
		}
		
		// Resize vector to accommodate maximum world index
		if (worldIndex >= static_cast<int>(levels.size()))
		{
			levels.resize(worldIndex + 1);
		}
		
		// Resize vector to accommodate maximum level index
		if (levelIndex >= static_cast<int>(levels[worldIndex].size()))
		{
			levels[worldIndex].resize(levelIndex + 1);
		}
		
		// Add level
		Level* level = &levels[worldIndex][levelIndex];
		level->filename = directory + filename;
		level->worldIndex = worldIndex;
		level->levelIndex = levelIndex;
	}
	
	// Close levels directory
	closedir(dir);
	
	// Load levels
	for (std::size_t i = ANTKEEPER_FIRST_WORLD_INDEX; i < levels.size(); ++i)
	{
		for (std::size_t j = ANTKEEPER_FIRST_LEVEL_INDEX; j < levels[i].size(); ++j)
		{
			Level* level = &levels[i][j];
			
			if (!level->load())
			{
				std::cout << "Failed to load level " << i << "-" << j << std::endl;
			}
			else
			{
				std::cout << "Loaded level " << i << "-" << j << std::endl;
			}
		}
	}
	
	return true;
}