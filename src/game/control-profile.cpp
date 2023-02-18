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

#include "game/control-profile.hpp"
#include "resources/serializer.hpp"
#include "resources/serialize-error.hpp"
#include "resources/deserializer.hpp"
#include "resources/deserialize-error.hpp"
#include "debug/log.hpp"

/**
 * Serializes a control profile.
 *
 * @param[in] profile Control profile to serialize.
 * @param[in,out] ctx Serialize context.
 *
 * @throw serialize_error Write error.
 * @throw serialize_error Unsupported mapping type.
 */
template <>
void serializer<game::control_profile>::serialize(const game::control_profile& profile, serialize_context& ctx)
{
	// Write number of mappings
	std::uint64_t size = static_cast<std::uint64_t>(profile.mappings.size());
	ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&size), 1);
	
	// Write mappings
	for (const auto& [key, value]: profile.mappings)
	{
		// Write key
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&key), 1);
		
		// Write mapping type
		const input::mapping_type mapping_type = value->get_mapping_type();
		ctx.write8(reinterpret_cast<const std::byte*>(&mapping_type), 1);
		
		// Write mapping
		switch (mapping_type)
		{
			case input::mapping_type::gamepad_axis:
				serializer<input::gamepad_axis_mapping>().serialize(*static_cast<const input::gamepad_axis_mapping*>(value.get()), ctx);
				break;
			
			case input::mapping_type::gamepad_button:
				serializer<input::gamepad_button_mapping>().serialize(*static_cast<const input::gamepad_button_mapping*>(value.get()), ctx);
				break;
			
			case input::mapping_type::key:
				serializer<input::key_mapping>().serialize(*static_cast<const input::key_mapping*>(value.get()), ctx);
				break;
			
			case input::mapping_type::mouse_button:
				serializer<input::mouse_button_mapping>().serialize(*static_cast<const input::mouse_button_mapping*>(value.get()), ctx);
				break;
			
			case input::mapping_type::mouse_motion:
				serializer<input::mouse_motion_mapping>().serialize(*static_cast<const input::mouse_motion_mapping*>(value.get()), ctx);
				break;
			
			case input::mapping_type::mouse_scroll:
				serializer<input::mouse_scroll_mapping>().serialize(*static_cast<const input::mouse_scroll_mapping*>(value.get()), ctx);
				break;
			
			default:
				throw serialize_error("Unsupported mapping type");
				break;
		}
	}
	
	// Write settings
	serializer<dict<std::uint32_t>>().serialize(profile.settings, ctx);
}

/**
 * Deserializes a control profile.
 *
 * @param[out] profile Control profile to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 * @throw deserialize_error Unsupported mapping type.
 */
template <>
void deserializer<game::control_profile>::deserialize(game::control_profile& profile, deserialize_context& ctx)
{
	profile.mappings.clear();
	
	// Read number of mappings
	std::uint64_t size = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&size), 1);
	
	// Read mappings
	for (std::uint64_t i = 0; i < size; ++i)
	{
		// Read key
		std::uint32_t key = 0;
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&key), 1);
		
		// Read mapping type
		input::mapping_type mapping_type;
		ctx.read8(reinterpret_cast<std::byte*>(&mapping_type), 1);
		
		// Read mapping
		switch (mapping_type)
		{
			case input::mapping_type::gamepad_axis:
			{
				input::gamepad_axis_mapping mapping;
				deserializer<input::gamepad_axis_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::gamepad_axis_mapping>(std::move(mapping)));
				break;
			}
			
			case input::mapping_type::gamepad_button:
			{
				input::gamepad_button_mapping mapping;
				deserializer<input::gamepad_button_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::gamepad_button_mapping>(std::move(mapping)));
				break;
			}
			
			case input::mapping_type::key:
			{
				input::key_mapping mapping;
				deserializer<input::key_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::key_mapping>(std::move(mapping)));
				break;
			}
			
			case input::mapping_type::mouse_button:
			{
				input::mouse_button_mapping mapping;
				deserializer<input::mouse_button_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::mouse_button_mapping>(std::move(mapping)));
				break;
			}
			
			case input::mapping_type::mouse_motion:
			{
				input::mouse_motion_mapping mapping;
				deserializer<input::mouse_motion_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::mouse_motion_mapping>(std::move(mapping)));
				break;
			}
			
			case input::mapping_type::mouse_scroll:
			{
				input::mouse_scroll_mapping mapping;
				deserializer<input::mouse_scroll_mapping>().deserialize(mapping, ctx);
				profile.mappings.emplace(key, std::make_unique<input::mouse_scroll_mapping>(std::move(mapping)));
				break;
			}
			
			default:
				throw deserialize_error("Unsupported mapping type");
				break;
		}
	}
	
	// Read settings
	deserializer<dict<std::uint32_t>>().deserialize(profile.settings, ctx);
}
