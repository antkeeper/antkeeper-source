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

#include "nest.hpp"
#include <algorithm>
#include <cmath>

#undef min
#undef max

Vector3 Shaft::getHelixPosition(float depth) const
{
	Vector3 position;
	position.x = entrance.x + std::cos(initialHelixAngle + depth / helixPitch) * helixRadius;
	position.y = entrance.y + depth;
	position.z = entrance.z + std::sin(initialHelixAngle + depth / helixPitch) * helixRadius;
	
	return position;
}

float Shaft::getHelixAngle(float depth) const
{
	return initialHelixAngle + depth / helixPitch;
}

Nest::Nest():
	root(nullptr)
{}

Nest::~Nest()
{
	if (root != nullptr)
	{
		free(root);
	}
}

void Nest::generate()
{
	if (root != nullptr)
	{
		// Delete existing shafts and chambers
		free(root);
	}
	
	// Seed random number generator
	rng.seed(parameters.randomSeed);
	
	// Generate shafts and chambers
	root = dig(nullptr);
	
	// Merge intersecting chambers
	
	// Create nest map
	map();
}

Shaft* Nest::dig(Chamber* parent) const
{
	Shaft* shaft = new Shaft();
	
	// Set child's parent
	shaft->parent = parent;
	
	if (parent != nullptr)
	{
		// Set parent's child to this shaft
		parent->child = shaft;
		
		// Increment generation
		shaft->generation = parent->parent->generation + 1;
		
		// Determine initial helix angle
		shaft->initialHelixAngle = parent->parent->getHelixAngle(parent->relativeDepth);
	}
	else
	{
		// Shaft is root shaft
		shaft->generation = 0;
		shaft->entrance = Vector3(0.0f);
		
		// Choose initial helix angle
		
	}
	
	shaft->initialHelixAngle = random(glm::radians(-180.0f), glm::radians(180.0f));
	
	if (parent == nullptr)
	{
		// Choose initial random parameters
		shaft->shaftRadius = random(parameters.minShaftRadius, parameters.maxShaftRadius);
		shaft->helixRadius = random(parameters.minShaftHelixRadius, parameters.maxShaftHelixRadius);
		shaft->helixPitch = random(parameters.minShaftHelixPitch, parameters.maxShaftHelixPitch);
	}
	else
	{
		// Copy initial random parameters from grandparent
		shaft->shaftRadius = parent->parent->shaftRadius;
		shaft->helixRadius = parent->parent->helixRadius;
		shaft->helixPitch = parent->parent->helixPitch;
	}
	
	// Choose random depth
	shaft->shaftDepth = random(parameters.minShaftDepth, parameters.maxShaftDepth);
	
	// Calculate entrance position
	if (parent != nullptr)
	{
		Vector3 helixPosition = parent->parent->getHelixPosition(parent->relativeDepth);
		float helixAngle = parent->parent->getHelixAngle(parent->relativeDepth);
		
		shaft->entrance.x = helixPosition.x + std::cos(helixAngle) * (parent->outerRadius - parent->innerRadius) - std::cos(shaft->initialHelixAngle) * shaft->helixRadius;
		shaft->entrance.y = helixPosition.y;
		shaft->entrance.z = helixPosition.z + std::sin(helixAngle) * (parent->outerRadius - parent->innerRadius) - std::sin(shaft->initialHelixAngle) * shaft->helixRadius;
	}
	
	// Determine potential child count (may be less, according to spacing between chambers)
	int maxChildCount = std::max<int>(1, random(parameters.minShaftChamberCount, parameters.maxShaftChamberCount));
	
	// Generate chambers, starting with final chamber (shaft must end with a chamber)
	for (float depth = shaft->shaftDepth; depth >= 0.0f;)
	{
		Chamber* chamber = new Chamber();
		shaft->children.push_back(chamber);
		
		chamber->parent = shaft;
		chamber->child = nullptr;
		chamber->relativeDepth = depth;
		chamber->absoluteDepth = shaft->entrance.y + chamber->relativeDepth;
		chamber->innerRadius = random(parameters.minChamberInnerRadius, parameters.maxChamberInnerRadius);
		chamber->outerRadius = random(parameters.minChamberOuterRadius, parameters.maxChamberOuterRadius);
		chamber->centralAngle = random(parameters.minChamberCentralAngle, parameters.maxChamberCentralAngle);
		
		// Check if maximum child count has been reached
		if (shaft->children.size() >= maxChildCount)
		{
			break;
		}
		
		// Decrease depth by random amount
		depth -= random(parameters.minShaftChamberPitch, parameters.maxShaftChamberPitch);
	}
	
	// Generate subshafts from chambers
	if (shaft->generation < parameters.maxShaftGeneration)
	{
		for (Chamber* chamber: shaft->children)
		{
			dig(chamber);
		}
	}
	
	return shaft;
}

void Nest::merge()
{
	
}

void Nest::map()
{
	
}

void Nest::free(Shaft* shaft)
{
	for (Chamber* chamber: shaft->children)
	{
		if (chamber->child != nullptr)
		{
			free(chamber->child);
		}
		
		delete chamber;
	}
	
	delete shaft;
}

inline float Nest::random(float minValue, float maxValue) const
{
	std::uniform_real_distribution<float> distribution(minValue, std::nextafter(maxValue, std::numeric_limits<float>::max()));
	return distribution(rng);
}

inline int Nest::random(int minValue, int maxValue) const
{
	std::uniform_int_distribution<int> distribution(minValue, std::nextafter(maxValue, std::numeric_limits<int>::max()));
	return distribution(rng);
}

void Nest::setParameters(const NestParameters& parameters)
{
	this->parameters = parameters;
}

const NestParameters& Nest::getParameters() const
{
	return parameters;
}