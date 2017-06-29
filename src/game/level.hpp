#ifndef LEVEL_SELECTOR_HPP
#define LEVEL_SELECTOR_HPP

#include "../configuration.hpp"
#include <string>
#include <vector>

class Level
{
public:
	Level();
	~Level();
	
	bool load();
	
	std::string filename;
	int worldIndex;
	int levelIndex;
	
	std::string biome;
	std::string heightmap;
};

class Campaign
{
public:
	Campaign();
	~Campaign();
	
	bool load(const std::string& directory);
	
	std::vector<std::vector<Level>> levels;
};

#endif // LEVEL_SELECTOR_HPP
