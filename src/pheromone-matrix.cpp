/*
 * Copyright (C) 2020  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pheromone-matrix.hpp"
#include <vmq/vmq.hpp>

void convolve(pheromone_matrix* matrix, const float* kernel, int kernel_size)
{
	float* front = matrix->buffers[matrix->current];
	float* back = matrix->buffers[(matrix->current + 1) % 2];
	const int kernel_radius = kernel_size << 1;

	// For each pheromone matrix row
	for (int i = 0; i < matrix->rows; ++i)
	{
		// For each pheromone in the row
		for (int j = 0; j < matrix->columns; ++j)
		{
			// Reset accumulator
			float accumulator = 0.0f;

			// For each kernel row
			for (int k = -kernel_radius; k <= kernel_radius; ++k)
			{
				// For each kernel element
				for (int l = -kernel_radius; l <= kernel_radius; ++l)
				{
					// Determine row and column of pheromone corresponding to the kernel element.
					int m = i + k;
					int n = j + l;

					// If the row and column indices are within the matrix
					if (m >= 0 && m < matrix->rows && n >= 0 && n < matrix->columns)
					{
						// Multiply pheromone strength in the front buffer by the kernel value and add it to the accumulator.
						accumulator += front[m * matrix->columns + n] * kernel[(k + kernel_radius) * kernel_size + l + kernel_radius];
					}
				}
			}

			// Set pheromone strength in the back buffer equal to the accumulator value
			back[i * matrix->columns + j] = accumulator;
		}
	}

	// Swap buffers
	matrix->current = (matrix->current + 1) % 2;
}

void evaporate(pheromone_matrix* matrix, float factor)
{
	const int size = matrix->columns * matrix->rows;

	for (int i = 0; i < size; ++i)
	{
		matrix->buffers[matrix->current][i] *= factor;
	}
}

void diffuse(pheromone_matrix* matrix)
{
	const vmq::matrix<float, 3, 3> diffusion_kernel =
		vmq::mul(
			vmq::matrix<float, 3, 3>
			{{
				{1, 2, 1},
				{2, 4, 2},
			 	{1, 2, 1}
			}},
			1.0f / 16.0f);
	
	convolve(matrix, &diffusion_kernel[0][0], 3);
}

