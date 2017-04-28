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

#include "pheromone.hpp"
#include <cmath>

void Pheromone::setStrength(float strength)
{
	this->strength = strength;
	
	// Calculate approximate radius
	const float minimumStrength = 0.01f;      // Pheromone strength epsilon
	const float quadraticAttenuation = 1.0f; // Quadratic attenuation factor (1.0 = inverse-square falloff)
	
	radiusSquared = strength / (quadraticAttenuation * minimumStrength);
	radius = std::sqrt(radiusSquared);
}

float Pheromone::getAttenuatedStrength(const Vector3& position) const
{
	Vector3 difference = this->position - position;
	
	float distanceSquared = glm::dot(difference, difference);
	
	if (distanceSquared == 0.0f)
	{
		return strength;
	}
	
	const float minimumStrength = 0.01f;      // Pheromone strength epsilon
	const float quadraticAttenuation = 1.0f; // Quadratic attenuation factor (1.0 = inverse-square falloff)
	
	return strength / (1.0f + quadraticAttenuation * distanceSquared);
}