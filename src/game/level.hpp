#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "../configuration.hpp"
#include "terrain.hpp"
#include <string>
#include <vector>

/**
 * Contains the parameters required to load a level.
 */
class LevelParameterSet
{
public:
	LevelParameterSet();
	~LevelParameterSet();
	
	// Loads level parameters from a .lvl file
	bool load(const std::string& filename);
	
	std::string filename;
	std::string biome;
	std::string heightmap;
};

/**
 * A level.
 */
class Level
{
public:
	Level();
	~Level();
	
	// Loads a level from a level file
	bool load(const LevelParameterSet& params);
	
	Terrain terrain;
	ModelInstance terrainSurface;
	ModelInstance terrainSubsurface;
};

/**
 * A collection of level parameters which constitute a campaign.
 */
class Campaign
{
public:
	Campaign();
	~Campaign();
	
	// Loads all level parameter sets in a directory with the file name pattern `<world>-<level>.lvl`
	bool load(const std::string& directory);
	
	// Returns the number of worlds in the campaign
	std::size_t getWorldCount() const;
	
	// Returns the number of levels in a world
	std::size_t getLevelCount(std::size_t worldIndex) const;
	
	// Returns the file for the level with the specified indices
	const LevelParameterSet* getLevelParams(std::size_t worldIndex, std::size_t levelIndex) const;
	
private:
	std::vector<std::vector<LevelParameterSet>> levelParameterSets;
};

inline std::size_t Campaign::getWorldCount() const
{
	return levelParameterSets.size();
}
	
inline std::size_t Campaign::getLevelCount(std::size_t worldIndex) const
{
	return levelParameterSets[worldIndex].size();
}

inline const LevelParameterSet* Campaign::getLevelParams(std::size_t worldIndex, std::size_t levelIndex) const
{
	return &levelParameterSets[worldIndex][levelIndex];
}

#endif // LEVEL_HPP
