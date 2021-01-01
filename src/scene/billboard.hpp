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

#ifndef ANTKEEPER_BILLBOARD_HPP
#define ANTKEEPER_BILLBOARD_HPP

#include "scene/scene-object.hpp"
#include "geometry/aabb.hpp"
#include "utility/fundamental-types.hpp"

class material;

/// Enumerates billboard types.
enum class billboard_type
{
	// No alignment
	flat,
	
	// Aligns to face camera
	spherical,
	
	// Rotates about an alignment axis to face camera
	cylindrical
};

/**
 * A 2D unit quad with one material.
 */
class billboard: public scene_object<billboard>
{
public:
	billboard();
	billboard(const billboard& other);
	billboard& operator=(const billboard& other);

	void set_material(material* material);
	
	/// Sets the billboard alignment mode.
	void set_billboard_type(billboard_type type);
	
	/// Sets the axis around which the billboard will be rotated when the alignment is set to billboard_alignment::cylindrical.
	void set_alignment_axis(const float3& axis);
	
	virtual const bounding_volume<float>& get_bounds() const;

	material* get_material() const;
	billboard_type get_billboard_type() const;
	const float3& get_alignment_axis() const;
	
	virtual void update_tweens();

private:
	static const aabb<float> untransformed_bounds;
	
	virtual void transformed();
	
	
	aabb<float> bounds;
	material* material;
	billboard_type type;
	float3 alignment_axis;
};

inline const bounding_volume<float>& billboard::get_bounds() const
{
	return bounds;
}

inline material* billboard::get_material() const
{
	return material;
}

inline billboard_type billboard::get_billboard_type() const
{
	return type;
}

inline const float3& billboard::get_alignment_axis() const
{
	return alignment_axis;
}

#endif // ANTKEEPER_BILLBOARD_HPP

