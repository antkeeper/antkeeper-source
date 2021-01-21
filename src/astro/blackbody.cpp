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

#include "blackbody.hpp"
#include <algorithm>

namespace astro
{

/// Transforms colors from CIE XYZ to linear RGB
static constexpr double3x3 xyz_to_rgb =
{
	3.2404542, -0.9692660, 0.0556434,
	-1.5371385, 1.8760108, -0.2040259,
	-0.4985314, 0.0415560, 1.0572252
};

double3 blackbody(double t)
{
	// Approximate the Planckian locus in CIE 1960 UCS color space (Krystek's algorithm)
	double tt = t * t;
    double u = (0.860117757 + 1.54118254e-4 * t + 1.28641212e-7 * tt) / (1.0 + 8.42420235e-4 * t + 7.08145163e-7 * tt);
    double v = (0.317398726 + 4.22806245e-5 * t + 4.20481691e-8 * tt) / (1.0 - 2.89741816e-5 * t + 1.61456053e-7 * tt);
	
	// CIE 1960 UCS -> CIE xyY, Y = 1
    double2 xyy = double2{3.0 * u, 2.0 * v} / (2.0 * u - 8.0 * v + 4.0);
	
	// CIE xyY -> CIE XYZ
	double3 xyz = {xyy.x / xyy.y, 1.0, (1.0 - xyy.x - xyy.y) / xyy.y};
    
	// CIE XYZ -> linear RGB
    double3 rgb = xyz_to_rgb * xyz;
    
    // Normalize RGB to preserve chromaticity
	return rgb / std::max<double>(rgb.x, std::max<double>(rgb.y, rgb.z));
}

} // namespace astro