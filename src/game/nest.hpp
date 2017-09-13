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

#ifndef NEST_HPP
#define NEST_HPP

#include <random>
#include <vector>
#include <emergent/emergent.hpp>

using namespace Emergent;

/**
 * General algorithm: Draw a series of lines straight down. Select multiple elevation levels on each line at which to create a chamber. Create helixes around lines. At the selected elevation levels on each line, create a corresponding chambers on the helixes at the same elevations, in the direction of the outside of the helix. Check for intersections between chambers and tunnels, and merge as necessary.
 */

struct Chamber;

/**
 * A vertically-directed helical shaft with one or more connected chambers.
 */
struct Shaft
{
	Chamber* parent;                // The parent chamber from which this shaft begins
	std::vector<Chamber*> children; // A list of chambers which are formed from this shaft
	int generation;                 // The generation index of this shaft. The root shaft is gen 0, the shafts from its chambers are gen 1, and so on.
	Vector3 entrance;               // Position of the entrance point of this shaft
	float shaftRadius;              // Radius of the shaft
	float shaftDepth;               // Total depth of this shaft
	float initialHelixAngle;        // Angle at which the shaft helix begins
	float helixRadius;              // Radius of the shaft helix
	float helixPitch;               // Pitch of the shaft helix
	
	/**
	 * Returns the position on the shaft's helix at the specified depth.
	 *
	 * @param depth Depth relative to the entrance of this shaft.
	 * @return Point on the helix at the specified depth.
	 */
	Vector3 getHelixPosition(float depth) const;
	
	/**
	 * Returns the angle to the helix at the specified depth.
	 *
	 * @param depth Depth relative to the entrance of this shaft.
	 * @return Angle to the helix at the specified depth.
	 */
	float getHelixAngle(float depth) const;
};

/**
 * A horizontal annular chamber with one parent shaft and a max of one child shaft. Chambers always face toward the outside of the parent shaft's helix.
 */
struct Chamber
{
	Shaft* parent;       // Parent shaft from which this chamber was formed
	Shaft* child;        // Child shaft which begins in this chamber
	int generation;      // The number of chambers from this chamber to the root shaft.
	float relativeDepth; // Depth from the entrance of the parent shaft to this chamber
	float absoluteDepth; // Depth from the entrance of the root shaft to this chamber
	float innerRadius;   // Inner radius of the annulus
	float outerRadius;   // Outer radius of the annulus
	float centralAngle;  // Angle of the annular sector
	float height;        // Height of the annular sector
	float childAngle;    // The angle on the annulus at which the child shaft begins
};

/**
 * Describes the parameters required to generate a nest.
 */
struct NestParameters
{
	// Random params
	unsigned int randomSeed;
	
	// Shaft params
	int maxShaftGeneration;
	float minShaftRadius;
	float maxShaftRadius;
	float minShaftDepth;
	float maxShaftDepth;
	float minShaftHelixRadius;
	float maxShaftHelixRadius;
	float minShaftHelixPitch;
	float maxShaftHelixPitch;
	int minShaftChamberCount;
	int maxShaftChamberCount;
	float minShaftChamberPitch;
	float maxShaftChamberPitch;
	
	// Chamber params
	float minChamberInnerRadius;
	float maxChamberInnerRadius;
	float minChamberOuterRadius;
	float maxChamberOuterRadius;
	float minChamberCentralAngle;
	float maxChamberCentralAngle;
};

class Nest
{
public:
	Nest();
	~Nest();
	
	/**
	 * Generates the nest and all of its shafts and chambers.
	 */
	void generate();
	
	/**
	 * Sets the nest generation parameters.
	 */
	void setParameters(const NestParameters& parameters);
	
	/**
	 * Returns the nest generation parameters.
	 */
	const NestParameters& getParameters() const;
	
	/**
	 * Returns a pointer to the root shaft of the nest.
	 */
	const Shaft* getRootShaft() const;
	
private:
	/**
	 * Recursive function which generates a connected system of shafts and chambers.
	 *
	 * @param parent Specifies the parent chamber of the shaft. A value of `nullptr` indicates the root shaft.
	 * @param generation The index of the shaft's generation. The root shaft is generation 0, the shafts formed from the root shaft's chambers are generation 1, and so on.
	 * @return The newly created shaft.
	 */
	Shaft* dig(Chamber* parent) const;
	
	/**
	 * Determines intersections between chambers and connects them.
	 */
	void merge();
	
	/**
	 * Creates a map (interconnected system of nodes) with which can be used to navigate the nest.
	 */
	void map();
	
	/**
	 * Recursive function which deletes a shaft and all of its connect chambers and subshafts.
	 *
	 * @param Specifies a shaft to delete.
	 */
	void free(Shaft* shaft);
	
	float random(float minValue, float maxValue) const;
	int random(int minValue, int maxValue) const;
	
	NestParameters parameters;
	Shaft* root;
	mutable std::default_random_engine rng;
};

inline const Shaft* Nest::getRootShaft() const
{
	return root;
}

#endif // NEST_HPP
