#ifndef BIOME_HPP
#define BIOME_HPP

#include <map>
#include <string>
#include <emergent/emergent.hpp>

using namespace Emergent;

class Biome
{
public:
	Biome();
	~Biome();
	
	bool load();
	
	std::string filename;
	std::string name;
	std::string soilHorizonOFilename;
	std::string soilHorizonAFilename;
	std::string soilHorizonBFilename;
	std::string soilHorizonCFilename;
	std::string cubemapName;
	
	Texture* soilHorizonO;
	Texture* soilHorizonA;
	Texture* soilHorizonB;
	Texture* soilHorizonC;
	Texture* diffuseCubemap;
	Texture* specularCubemap;
};

class Biosphere
{
public:
	bool load(const std::string& directory);
	
	std::map<std::string, Biome> biomes;
};

#endif // BIOME_HPP
