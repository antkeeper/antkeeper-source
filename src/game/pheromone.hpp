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

#ifndef PHEROMONE_HPP
#define PHEROMONE_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

class Pheromone
{
public:
	void setPosition(const Vector3& position);
	void setStrength(float strength);
	
	const Vector3& getPosition() const;
	float getStrength() const;
	float getRadius() const;
	float getRadiusSquared() const;
	
	float getAttenuatedStrength(const Vector3& position) const;
	
private:
	Vector3 position;
	float strength;
	float radius;
	float radiusSquared;
};

inline void Pheromone::setPosition(const Vector3& position)
{
	this->position = position;
}

inline const Vector3& Pheromone::getPosition() const
{
	return position;
}

inline float Pheromone::getStrength() const
{
	return strength;
}

inline float Pheromone::getRadius() const
{
	return radius;
}

inline float Pheromone::getRadiusSquared() const
{
	return radiusSquared;
}

#endif // PHEROMONE_HPP