/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include "resource-loader.hpp"
#include "physics/orbit/ephemeris.hpp"
#include "utility/bit-math.hpp"
#include <cstdint>
#include <physfs.h>

/// Offset to time data in the JPL DE header, in bytes.
static constexpr std::size_t jpl_de_offset_time = 0xA5C;

/// Offset to the first coefficient table in the JPL DE header, in bytes.
static constexpr std::size_t jpl_de_offset_table1 = 0xA88;

/// Offset to the DE version number in the JPL DE header, in bytes.
static constexpr std::size_t jpl_de_offset_denum = 0xB18;

/// Offset to the second coefficient table in the JPL DE header, in bytes.
static constexpr std::size_t jpl_de_offset_table2 = 0xB1C;

/// Offset to the third coefficient table in the JPL DE header, in bytes, if the constant limit has not been exceeded.
static constexpr std::size_t jpl_de_offset_table3 = 0xB28;

/// Mask to detect bytes in the most significant word of the JPL DE version number.
static constexpr std::int32_t jpl_de_denum_endian_mask = 0xFFFF0000;

/// Number of items in the first coefficient table.
static constexpr std::size_t jpl_de_table1_count = 12;

/// Number of items in the second coefficient table.
static constexpr std::size_t jpl_de_table2_count = 1;

/// Number of items in the third coefficient table.
static constexpr std::size_t jpl_de_table3_count = 2;

/// Maximum number of items in a JPL DE file.
static constexpr std::size_t jpl_de_max_item_count = jpl_de_table1_count + jpl_de_table2_count + jpl_de_table3_count;

/// Maximum number of constants in the first set of constant names.
static constexpr std::size_t jpl_de_constant_limit = 400;

/// Length of a constant name, in bytes.
static constexpr std::size_t jpl_de_constant_length = 6;

/// Enumerated IDs of the JPL DE items.
enum
{
	/// Mercury
	jpl_de_id_mercury,
	
	/// Venus
	jpl_de_id_venus,
	
	/// Earth-Moon barycenter
	jpl_de_id_embary,
	
	/// Mars
	jpl_de_id_mars,
	
	/// Jupiter
	jpl_de_id_jupiter,
	
	/// Saturn
	jpl_de_id_saturn,
	
	/// Uranus
	jpl_de_id_uranus,
	
	/// Neptune
	jpl_de_id_neptune,
	
	/// Pluto
	jpl_de_id_pluto,
	
	/// Moon
	jpl_de_id_moon,
	
	/// Sun
	jpl_de_id_sun,
	
	/// Earth nutation
	jpl_de_id_earth_nutation,
	
	/// Lunar mantle libration
	jpl_de_id_luma_libration,
	
	/// Lunar mantle angular velocity
	jpl_de_id_luma_angular_velocity,
	
	/// TT-TDB
	jpl_de_id_tt_tdb
};

/// Number of components for each JPL DE item.
static constexpr std::size_t jpl_de_component_count[jpl_de_max_item_count] =
{
	3, // Mercury: x,y,z (km)
	3, // Venus: x,y,z (km)
	3, // Earth-Moon barycenter: x,y,z (km)
	3, // Mars: x,y,z (km)
	3, // Jupiter: x,y,z (km)
	3, // Saturn: x,y,z (km)
	3, // Uranus: x,y,z (km)
	3, // Neptune: x,y,z (km)
	3, // Pluto: x,y,z (km)
	3, // Moon: x,y,z (km)
	3, // Sun: x,y,z (km)
	2, // Earth nutation: d_psi,d_epsilon (radians)
	3, // Lunar mantle libration: phi,theta,ps (radians)
	3, // Lunar mantle angular velocity: omega_x,omega_y,omega_z (radians/day)
	1  // TT-TDB: t (seconds)
};

/// Reads and swaps the byte order of 32-bit numbers.
static PHYSFS_sint64 read_swap32(PHYSFS_File* handle, void* buffer, PHYSFS_uint64 len)
{
	PHYSFS_sint64 status = PHYSFS_readBytes(handle, buffer, len);
	for (std::uint32_t* ptr32 = (uint32_t*)buffer; len >= 8; len -= 8, ++ptr32)
		*ptr32 = bit::swap32(*ptr32);
	return status;
}

/// Reads and swaps the byte order of 64-bit numbers.
static PHYSFS_sint64 read_swap64(PHYSFS_File* handle, void* buffer, PHYSFS_uint64 len)
{
	PHYSFS_sint64 status = PHYSFS_readBytes(handle, buffer, len);
	for (std::uint64_t* ptr64 = (uint64_t*)buffer; len >= 8; len -= 8, ++ptr64)
		*ptr64 = bit::swap64(*ptr64);
	return status;
}

template <>
physics::orbit::ephemeris<double>* resource_loader<physics::orbit::ephemeris<double>>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Init file reading function pointers
	PHYSFS_sint64 (*read32)(PHYSFS_File*, void*, PHYSFS_uint64) = &PHYSFS_readBytes;
	PHYSFS_sint64 (*read64)(PHYSFS_File*, void*, PHYSFS_uint64) = &PHYSFS_readBytes;
	
	// Read DE version number
	std::int32_t denum;
	PHYSFS_seek(file, jpl_de_offset_denum);
	PHYSFS_readBytes(file, &denum, sizeof(std::int32_t));
	
	// If file endianness does not match host
	if (denum & jpl_de_denum_endian_mask)
	{
		// Use endian-swapping read functions
		read32 = &read_swap32;
		read64 = &read_swap64;
	}
	
	// Read ephemeris time
	double ephemeris_time[3];
	PHYSFS_seek(file, jpl_de_offset_time);
	read64(file, ephemeris_time, sizeof(double) * 3);
	
	// Make time relative to J2000 epoch
	const double epoch = 2451545.0;
	ephemeris_time[0] -= epoch;
	ephemeris_time[1] -= epoch;
	
	// Read number of constants
	std::int32_t constant_count;
	read32(file, &constant_count, sizeof(std::int32_t));
	
	// Read first coefficient table
	std::int32_t coeff_table[jpl_de_max_item_count][3];
	PHYSFS_seek(file, jpl_de_offset_table1);
	read32(file, coeff_table, sizeof(std::int32_t) * jpl_de_table1_count * 3);
	
	// Read second coefficient table
	PHYSFS_seek(file, jpl_de_offset_table2);
	read32(file, &coeff_table[jpl_de_table1_count][0], sizeof(std::int32_t) * jpl_de_table2_count * 3);
	
	// Seek past extra constant names
	if (constant_count > jpl_de_constant_limit)
		PHYSFS_seek(file, jpl_de_offset_table3 + (constant_count - jpl_de_constant_limit) * jpl_de_constant_length);
	
	// Read third coefficient table
	read32(file, &coeff_table[jpl_de_table1_count + jpl_de_table2_count][0], sizeof(std::int32_t) * jpl_de_table3_count * 3);
	
	// Calculate number of coefficients per record
	std::int32_t record_coeff_count = 0;
	for (int i = 0; i < jpl_de_max_item_count; ++i)
	{
		std::int32_t coeff_count = coeff_table[i][0] + coeff_table[i][1] * coeff_table[i][2] * jpl_de_component_count[i] - 1;
		record_coeff_count = std::max(record_coeff_count, coeff_count);
	}
	
	// Calculate record size and record count
	std::size_t record_size = record_coeff_count * sizeof(double);
	std::size_t record_count = static_cast<std::size_t>((ephemeris_time[1] - ephemeris_time[0]) / ephemeris_time[2]);
	
	// Calculate coefficient strides
	std::size_t strides[11];
	for (int i = 0; i < 11; ++i)
		strides[i] = coeff_table[i][2] * coeff_table[i][1] * 3;
	
	// Allocate and resize ephemeris to accommodate items 0-10
	physics::orbit::ephemeris<double>* ephemeris = new physics::orbit::ephemeris<double>();
	ephemeris->resize(11);
	
	// Init trajectories
	for (int i = 0; i < 11; ++i)
	{
		auto& trajectory = (*ephemeris)[i];
		trajectory.t0 = ephemeris_time[0];
		trajectory.t1 = ephemeris_time[1];
		trajectory.dt = ephemeris_time[2] / static_cast<double>(coeff_table[i][2]);
		trajectory.n = coeff_table[i][1];
		trajectory.a.resize(record_count * strides[i]);
	}
	
	// Read coefficients
	for (std::size_t i = 0; i < record_count; ++i)
	{
		// Seek to coefficient record
		PHYSFS_seek(file, (i + 2) * record_size + 2 * sizeof(double));
		
		for (int j = 0; j < 11; ++j)
		{
			read64(file, &(*ephemeris)[j].a[i * strides[j]], sizeof(double) * strides[j]);
		}
	}
	
	return ephemeris;
}
