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

#ifndef ANTKEEPER_WEATHER_SYSTEM_HPP
#define ANTKEEPER_WEATHER_SYSTEM_HPP

#include "entity-system.hpp"
#include "utility/fundamental-types.hpp"

class sky_pass;
class shadow_map_pass;
class material_pass;
class ambient_light;
class directional_light;
class image;

class weather_system:
	public entity_system
{
public:
	weather_system(entt::registry& registry);
	virtual void update(double t, double dt);
	
	void set_coordinates(const float2& coordinates);
	
	void set_ambient_light(ambient_light* light);
	void set_sun_light(directional_light* light);
	void set_moon_light(directional_light* light);
	void set_sky_pass(::sky_pass* pass);
	void set_shadow_map_pass(::shadow_map_pass* pass);
	void set_material_pass(::material_pass* pass);
	
	/// @param tc Timezone correction, in hours
	void set_time(int year, int month, int day, int hour, int minute, int second, double tc);
	void set_time_scale(float scale);
	
	void set_sky_palette(const ::image* image);
	void set_sun_palette(const ::image* image);
	void set_moon_palette(const ::image* image);
	void set_ambient_palette(const ::image* image);
	void set_shadow_palette(const ::image* image);
	
private:
	double jd;
	
	float2 coordinates;
	
	float time_scale;
	float3 sun_direction;
	ambient_light* ambient_light;
	directional_light* sun_light;
	directional_light* moon_light;
	directional_light* shadow_light;
	sky_pass* sky_pass;
	shadow_map_pass* shadow_map_pass;
	material_pass* material_pass;
	const image* sky_palette;
	const image* sun_palette;
	const image* moon_palette;
	const image* ambient_palette;
	const image* shadow_palette;
	std::vector<float3> sun_colors;
	std::vector<float3> moon_colors;
	std::vector<float3> ambient_colors;
	std::vector<float> shadow_strengths;
	std::vector<std::array<float4, 4>> sky_gradients;
};

#endif // ANTKEEPER_WEATHER_SYSTEM_HPP
