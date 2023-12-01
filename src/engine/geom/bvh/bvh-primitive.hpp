// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_BVH_PRIMITIVE_HPP
#define ANTKEEPER_GEOM_BVH_PRIMITIVE_HPP

#include <engine/geom/primitives/box.hpp>
#include <engine/math/vector.hpp>

namespace geom {

/**
 * BVH primitive.
 */
struct bvh_primitive
{
	/// Geometric center of the primitive.
	math::fvec3 centroid;
	
	/// Axis-aligned bounding box containing the primitive.
	geom::box<float> bounds;
};

} // namespace geom

#endif // ANTKEEPER_GEOM_BVH_PRIMITIVE_HPP
