/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_PHEROMONE_MATRIX_HPP
#define ANTKEEPER_PHEROMONE_MATRIX_HPP

/**
 * A double-buffered matrix containing floating point pheromone strengths.
 */
struct pheromone_matrix
{
	/// Number of columns in the matrix.
	int columns;

	/// Number of rows in the matrix.
	int rows;

	/// Two buffers containing pheromone strengths
	float** buffers;

	/// Index of the current buffer
	int current;
};

/**
 * Performs a convolution on a pheromone matrix.
 *
 * @param matrix Pointer to a pheromone matrix.
 * @param kernel Convolution kernel.
 * @param kernel_size Size of the kernel.
 */
void convolve(pheromone_matrix* matrix, const float* kernel, int kernel_size);

/**
 * Causes all pheromones in a pheromone matrix to decrease in strength according to the specified evaporation rate.
 *
 * @param matrix Pointer to the pheromone matrix.
 * @param factor Evaporation factor by which each pheromone strength will be multiplied.
 */
void evaporate(pheromone_matrix* matrix, float rate);

/**
 * Causes all pheromones in a pheromone matrix to diffuse.
 */
void diffuse(pheromone_matrix* matrix);

#endif // ANTKEEPER_PHEROMONE_MATRIX_HPP

