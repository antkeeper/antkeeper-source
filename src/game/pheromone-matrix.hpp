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

#ifndef PHEROMONE_MATRIX_HPP
#define PHEROMONE_MATRIX_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

class PheromoneMatrix
{
public:
	PheromoneMatrix(int columns, int rows, const Vector2& boundsMin, const Vector2& boundsMax);
	
	~PheromoneMatrix();
	
	void clear();
	void evaporate();
	void diffuse();
	
	float query(const Vector2& position) const;
	float query(const Vector2& position, float radius) const;
	
	void deposit(const Vector2& position, float concentration);
	
	float getCellWidth() const;
	float getCellHeight() const;
	
	const float* getActiveBuffer() const;
	
private:
	int columns;
	int rows;
	int size;
	float* bufferA;
	float* bufferB;
	float* activeBuffer;
	float evaporationFactor;
	Vector2 boundsMin;
	Vector2 boundsMax;
	float matrixWidth;
	float matrixHeight;
	float matrixHalfWidth;
	float matrixHalfHeight;
	float cellWidth;
	float cellHeight;
	int diffusionKernelRadius;
	int diffusionKernelSize;
	float** diffusionKernel;
};

inline float PheromoneMatrix::getCellWidth() const
{
	return cellWidth;
}

inline float PheromoneMatrix::getCellHeight() const
{
	return cellHeight;
}

inline const float* PheromoneMatrix::getActiveBuffer() const
{
	return activeBuffer;
}

#endif // PHEROMONE_MATRIX_HPP