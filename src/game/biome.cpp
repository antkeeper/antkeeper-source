#include "biome.hpp"
#include "../settings.hpp"

#if defined(_WIN32) || defined(__CYGWIN__)
#include "../windows-dirent.h"
#else
#include <dirent.h>
#endif

#include <iostream>


Biome::Biome()
{}

Biome::~Biome()
{}

bool Biome::load()
{
	ParameterDict parameters;
	if (!parameters.load(filename))
	{
		return false;
	}
	
	parameters.get("name", &name);
	parameters.get("soil-horizon-o", &soilHorizonOFilename);
	parameters.get("soil-horizon-a", &soilHorizonAFilename);
	parameters.get("soil-horizon-b", &soilHorizonBFilename);
	parameters.get("soil-horizon-c", &soilHorizonCFilename);
	parameters.get("cubemap", &cubemapName);
	
	TextureLoader textureLoader;
	textureLoader.setCubemap(false);
	textureLoader.setMipmapChain(false);
	textureLoader.setWrapS(true);
	textureLoader.setWrapT(true);
	
	// Load soil horizon textures
	soilHorizonO = textureLoader.load(std::string("data/textures/") + soilHorizonOFilename);
	soilHorizonA = textureLoader.load(std::string("data/textures/") + soilHorizonAFilename);
	soilHorizonB = textureLoader.load(std::string("data/textures/") + soilHorizonBFilename);
	soilHorizonC = textureLoader.load(std::string("data/textures/") + soilHorizonCFilename);
	
	// Load diffuse cubemap
	textureLoader.setCubemap(true);
	textureLoader.setMipmapChain(false);
	textureLoader.setWrapS(false);
	textureLoader.setWrapT(false);
	textureLoader.setWrapR(false);
	
	std::string diffuseCubemapFilename = std::string("data/textures/") + cubemapName + std::string("-diffuse.png");
	diffuseCubemap = textureLoader.load(diffuseCubemapFilename);
	if (!diffuseCubemap)
	{
		std::cerr << "Failed to load diffuse cubemap \"" << diffuseCubemapFilename << "\"" << std::endl;
	}
	
	// Load specular cubemap
	textureLoader.setCubemap(true);
	textureLoader.setMipmapChain(true);
	std::string specularCubemapFilename = std::string("data/textures/") + cubemapName + std::string("-specular_m%02d.png");
	specularCubemap = textureLoader.load(specularCubemapFilename);
	if (!specularCubemap)
	{
		std::cerr << "Failed to load specular cubemap \"" << specularCubemapFilename << "\"" << std::endl;
	}
	
	return true;
}

bool Biosphere::load(const std::string& directory)
{
	// Open biomes directory
	DIR* dir = opendir(directory.c_str());
	if (dir == nullptr)
	{
		std::cout << "Failed to open biome directory \"" << directory << "\"" << std::endl;
		return false;
	}
	
	// Scan directory for .bio files
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
		if (extension != "bio")
		{
			continue;
		}
		
		// Add biome
		std::string name = filename.substr(0, delimeter);
		Biome* biome = &biomes[name];
		biome->filename = directory + filename;
	}
	
	// Close biomes directory
	closedir(dir);
	
	// Load biomes
	for (std::map<std::string, Biome>::iterator it = biomes.begin(); it != biomes.end(); ++it)
	{
		Biome* biome = &it->second;
		
		if (!biome->load())
		{
			std::cout << "Failed to load biome \"" << biome->filename << "\"" << std::endl;
		}
		else
		{
			std::cout << "Loaded biome \"" << biome->filename << "\"" << std::endl;
		}
	}
	
	return true;
}
