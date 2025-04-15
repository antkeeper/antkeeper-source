// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.physics.orbit.ephemeris;
import engine.resources.deserializer;
import engine.resources.resource_loader;
import engine.utility.sized_types;
import engine.math.functions;
import <bit>;
import <functional>;

namespace engine::resources
{
	/// Offset to time data in the JPL DE header, in bytes.
	static constexpr usize jpl_de_offset_time = 0xA5C;

	/// Offset to the first coefficient table in the JPL DE header, in bytes.
	static constexpr usize jpl_de_offset_table1 = 0xA88;

	/// Offset to the DE version number in the JPL DE header, in bytes.
	static constexpr usize jpl_de_offset_denum = 0xB18;

	/// Offset to the second coefficient table in the JPL DE header, in bytes.
	static constexpr usize jpl_de_offset_table2 = 0xB1C;

	/// Offset to the third coefficient table in the JPL DE header, in bytes, if the constant limit has not been exceeded.
	static constexpr usize jpl_de_offset_table3 = 0xB28;

	/// Mask to detect bytes in the most significant word of the JPL DE version number.
	static constexpr i32 jpl_de_denum_endian_mask = 0xFFFF0000;

	/// Number of items in the first coefficient table.
	static constexpr usize jpl_de_table1_count = 12;

	/// Number of items in the second coefficient table.
	static constexpr usize jpl_de_table2_count = 1;

	/// Number of items in the third coefficient table.
	static constexpr usize jpl_de_table3_count = 2;

	/// Maximum number of items in a JPL DE file.
	static constexpr usize jpl_de_max_item_count = jpl_de_table1_count + jpl_de_table2_count + jpl_de_table3_count;

	/// Maximum number of constants in the first set of constant names.
	static constexpr usize jpl_de_constant_limit = 400;

	/// Length of a constant name, in bytes.
	static constexpr usize jpl_de_constant_length = 6;

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
	static constexpr u8 jpl_de_component_count[jpl_de_max_item_count] =
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

	/// Deserializes an ephemeris.
	/// @param[out] value Ephemeris to deserialize.
	/// @param[in,out] ctx Deserialize context.
	/// @throw deserialize_error Read error.
	template <>
	void deserializer<physics::orbit::ephemeris<double>>::deserialize(physics::orbit::ephemeris<double>& value, deserialize_context& ctx)
	{
		auto& ephemeris = value;

		ephemeris.trajectories.clear();

		// Init file reading function pointers
		usize(deserialize_context:: * read32)(std::byte*, usize) = &deserialize_context::read32<std::endian::native>;
		usize(deserialize_context:: * read64)(std::byte*, usize) = &deserialize_context::read64<std::endian::native>;

		// Read DE version number
		i32 denum;
		ctx.seek(jpl_de_offset_denum);
		ctx.read8(reinterpret_cast<std::byte*>(&denum), sizeof(i32));

		// If file endianness does not match host endianness
		if (denum & jpl_de_denum_endian_mask)
		{
			// Use endian-swapping read functions
			if constexpr (std::endian::native == std::endian::little)
			{
				read32 = &deserialize_context::read32<std::endian::big>;
				read64 = &deserialize_context::read64<std::endian::big>;
			}
			else
			{
				read32 = &deserialize_context::read32<std::endian::little>;
				read64 = &deserialize_context::read64<std::endian::little>;
			}
		}

		// Read ephemeris time
		double ephemeris_time[3];
		ctx.seek(jpl_de_offset_time);
		std::invoke(read64, ctx, reinterpret_cast<std::byte*>(ephemeris_time), 3);

		// Make time relative to J2000 epoch
		const double epoch = 2451545.0;
		ephemeris_time[0] -= epoch;
		ephemeris_time[1] -= epoch;

		// Read number of constants
		i32 constant_count;
		std::invoke(read32, ctx, reinterpret_cast<std::byte*>(&constant_count), 1);

		// Read first coefficient table
		i32 coeff_table[jpl_de_max_item_count][3];
		ctx.seek(jpl_de_offset_table1);
		std::invoke(read32, ctx, reinterpret_cast<std::byte*>(coeff_table), jpl_de_table1_count * 3);

		// Read second coefficient table
		ctx.seek(jpl_de_offset_table2);
		std::invoke(read32, ctx, reinterpret_cast<std::byte*>(&coeff_table[jpl_de_table1_count][0]), jpl_de_table2_count * 3);

		// Seek past extra constant names
		if (constant_count > jpl_de_constant_limit)
		{
			ctx.seek(jpl_de_offset_table3 + (constant_count - jpl_de_constant_limit) * jpl_de_constant_length);
		}

		// Read third coefficient table
		std::invoke(read32, ctx, reinterpret_cast<std::byte*>(&coeff_table[jpl_de_table1_count + jpl_de_table2_count][0]), jpl_de_table3_count * 3);

		// Calculate number of coefficients per record
		i32 record_coeff_count = 0;
		for (int i = 0; i < jpl_de_max_item_count; ++i)
		{
			i32 coeff_count = coeff_table[i][0] + coeff_table[i][1] * coeff_table[i][2] * static_cast<i32>(jpl_de_component_count[i]) - 1;
			record_coeff_count = math::max(record_coeff_count, coeff_count);
		}

		// Calculate record size and record count
		usize record_size = record_coeff_count * sizeof(double);
		usize record_count = static_cast<usize>((ephemeris_time[1] - ephemeris_time[0]) / ephemeris_time[2]);

		// Calculate coefficient strides
		usize strides[11];
		for (int i = 0; i < 11; ++i)
		{
			strides[i] = coeff_table[i][2] * coeff_table[i][1] * 3;
		}

		// Resize ephemeris to accommodate items 0-10
		ephemeris.trajectories.resize(11);

		// Init trajectories
		for (int i = 0; i < 11; ++i)
		{
			auto& trajectory = ephemeris.trajectories[i];
			trajectory.t0 = ephemeris_time[0];
			trajectory.t1 = ephemeris_time[1];
			trajectory.dt = ephemeris_time[2] / static_cast<double>(coeff_table[i][2]);
			trajectory.n = coeff_table[i][1];
			trajectory.a.resize(record_count * strides[i]);
		}

		// Read coefficients
		for (usize i = 0; i < record_count; ++i)
		{
			// Seek to coefficient record
			ctx.seek((i + 2) * record_size + 2 * sizeof(double));

			for (int j = 0; j < 11; ++j)
			{
				std::invoke(read64, ctx, reinterpret_cast<std::byte*>(&ephemeris.trajectories[j].a[i * strides[j]]), strides[j]);
			}
		}
	}

	template <>
	std::unique_ptr<physics::orbit::ephemeris<double>> resource_loader<physics::orbit::ephemeris<double>>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		auto resource = std::make_unique<physics::orbit::ephemeris<double>>();

		deserializer<physics::orbit::ephemeris<double>>().deserialize(*resource, *ctx);

		return resource;
	}
}
