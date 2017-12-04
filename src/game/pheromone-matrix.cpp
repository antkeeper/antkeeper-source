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

#include "pheromone-matrix.hpp"
#include "../configuration.hpp"
#include <algorithm>
#include <cmath>

PheromoneMatrix::PheromoneMatrix(int columns, int rows, const Vector2& boundsMin, const Vector2& boundsMax)
{
	this->columns = columns;
	this->rows = rows;
	size = columns * rows;
	bufferA = new float[size];
	bufferB = new float[size];
	activeBuffer = bufferA;
	this->boundsMin = boundsMin;
	this->boundsMax = boundsMax;
	matrixWidth = boundsMax.x - boundsMin.x;
	matrixHeight = boundsMax.y - boundsMin.y;
	matrixHalfWidth = matrixWidth * 0.5f;
	matrixHalfHeight = matrixHeight * 0.5f;
	cellWidth = matrixWidth / static_cast<float>(columns);
	cellHeight = matrixHeight / static_cast<float>(rows);
	
	diffusionKernelSize = 3;
	diffusionKernelRadius = 1;
	diffusionKernel = new float*[diffusionKernelSize];
	for (int i = 0; i < diffusionKernelSize; ++i)
	{
		diffusionKernel[i] = new float[diffusionKernelSize];
	}
	
	diffusionKernel[0][0] = 0.0083333f; diffusionKernel[0][1] = 0.0166667f; diffusionKernel[0][2] = 0.0083333f;
	
	diffusionKernel[1][0] = 0.0166667f; diffusionKernel[1][1] = 0.9f; diffusionKernel[1][2] = 0.0166667f;
	
	diffusionKernel[2][0] = 0.0083333f; diffusionKernel[2][1] = 0.0166667f; diffusionKernel[2][2] = 0.0083333f;
	
	clear();
}

PheromoneMatrix::~PheromoneMatrix()
{
	delete[] bufferA;
	delete[] bufferB;
	
	for (int i = 0; i < diffusionKernelSize; ++i)
	{
		delete[] diffusionKernel[i];
	}
	delete[] diffusionKernel;
}

void PheromoneMatrix::clear()
{
	for (int i = 0; i < size; ++i)
	{
		activeBuffer[i] = 0.0f;
	}
}

void PheromoneMatrix::evaporate()
{
	for (int i = 0; i < size; ++i)
	{
		activeBuffer[i] *= EVAPORATION_FACTOR;
	}
}

void PheromoneMatrix::diffuse()
{
	float* diffusionBuffer = (activeBuffer == bufferA) ? bufferB : bufferA;
	
	int index = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			float concentration = 0.0f;

			for (int k = -diffusionKernelRadius; k <= diffusionKernelRadius; ++k)
			{
				int row = std::max<int>(0, std::min<int>(rows - 1, i + k));

				for (int l = -diffusionKernelRadius; l <= diffusionKernelRadius; ++l)
				{
					int column = std::max<int>(0, std::min<int>(columns - 1, j + l));
					concentration += activeBuffer[row * columns + column] * diffusionKernel[k + diffusionKernelRadius][l + diffusionKernelRadius];
				}
			}

			diffusionBuffer[index++] = concentration;
		}
	}

	activeBuffer = diffusionBuffer;
}

float PheromoneMatrix::query(const Vector2& position) const
{
	int column = static_cast<int>((matrixHalfWidth + position.x) / cellWidth);
	int row = static_cast<int>((matrixHalfHeight + position.y) / cellHeight);
	
	if (columns < 0 || column >= columns || row < 0 || row >= rows)
	{
		return 0.0f;
	}
	
	int index = row * columns + column;
	return activeBuffer[index];
}

float PheromoneMatrix::query(const Vector2& position, float radius) const
{
	float radiusSquared = radius * radius;
	float concentration = 0.0f;

	for (float y = position.y - radius; y <= position.y + radius; y += cellHeight)
	{
		int row = static_cast<int>((matrixHalfHeight + y) / cellHeight);
		if (row < 0)
			continue;
		else if (row >= rows)
			break;

		float dy = y - position.y;

		for (float x = position.x - radius; x <= position.x + radius; x += cellWidth)
		{
			int column = floor((matrixHalfWidth + x) / cellWidth);
			if (column < 0)
				continue;
			else if (column >= columns)
				break;

			float dx = x - position.x;

			float distanceSquared = dx * dx + dy * dy;
			if (distanceSquared <= radiusSquared)
			{
				concentration += activeBuffer[row * columns + column];
			}
		}
	}

	return concentration;
}

void PheromoneMatrix::deposit(const Vector2& position, float concentration)
{
	int column = static_cast<int>((matrixHalfWidth + position.x) / cellWidth);
	int row = static_cast<int>((matrixHalfHeight + position.y) / cellHeight);
	
	if (columns < 0 || column >= columns || row < 0 || row >= rows)
	{
		return;
	}
	
	int index = row * columns + column;
	activeBuffer[index] += concentration;
}
