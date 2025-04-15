// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/control-profile.hpp"
import engine.debug.log;
import engine.resources.resource_loader;
import engine.resources.serializer;
import engine.resources.serialize_error;
import engine.resources.deserializer;
import engine.resources.deserialize_error;
import engine.utility.sized_types;

namespace engine::resources
{
	/// Serializes a control profile.
	/// @param[in] profile Control profile to serialize.
	/// @param[in,out] ctx Serialize context.
	/// @throw serialize_error Write error.
	/// @throw serialize_error Unsupported mapping type.
	template <>
	void serializer<control_profile>::serialize(const control_profile& profile, serialize_context& ctx)
	{
		// Write number of mappings
		u64 size = static_cast<u64>(profile.mappings.size());
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&size), 1);

		// Write mappings
		for (const auto& [key, value] : profile.mappings)
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
		serializer<dict<hash::fnv32_t>>().serialize(profile.settings, ctx);
	}

	/// Deserializes a control profile.
	/// @param[out] profile Control profile to deserialize.
	/// @param[in,out] ctx Deserialize context.
	/// @throw deserialize_error Read error.
	/// @throw deserialize_error Unsupported mapping type.
	template <>
	void deserializer<control_profile>::deserialize(control_profile& profile, resources::deserialize_context& ctx)
	{
		profile.mappings.clear();

		// Read number of mappings
		u64 size = 0;
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&size), 1);

		// Read mappings
		for (u64 i = 0; i < size; ++i)
		{
			// Read key
			hash::fnv32_t key{};
			ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&key), 1);

			// Read mapping type
			input::mapping_type mapping_type{};
			ctx.read8(reinterpret_cast<std::byte*>(&mapping_type), 1);

			// Read mapping
			switch (mapping_type)
			{
				case input::mapping_type::gamepad_axis:
				{
					input::gamepad_axis_mapping mapping{};
					deserializer<input::gamepad_axis_mapping>().deserialize(mapping, ctx);
					profile.mappings.emplace(key, std::make_unique<input::gamepad_axis_mapping>(std::move(mapping)));
					break;
				}

				case input::mapping_type::gamepad_button:
				{
					input::gamepad_button_mapping mapping{};
					deserializer<input::gamepad_button_mapping>().deserialize(mapping, ctx);
					profile.mappings.emplace(key, std::make_unique<input::gamepad_button_mapping>(std::move(mapping)));
					break;
				}

				case input::mapping_type::key:
				{
					input::key_mapping mapping{};
					deserializer<input::key_mapping>().deserialize(mapping, ctx);
					profile.mappings.emplace(key, std::make_unique<input::key_mapping>(std::move(mapping)));
					break;
				}

				case input::mapping_type::mouse_button:
				{
					input::mouse_button_mapping mapping{};
					deserializer<input::mouse_button_mapping>().deserialize(mapping, ctx);
					profile.mappings.emplace(key, std::make_unique<input::mouse_button_mapping>(std::move(mapping)));
					break;
				}

				case input::mapping_type::mouse_motion:
				{
					input::mouse_motion_mapping mapping{};
					deserializer<input::mouse_motion_mapping>().deserialize(mapping, ctx);
					profile.mappings.emplace(key, std::make_unique<input::mouse_motion_mapping>(std::move(mapping)));
					break;
				}

				case input::mapping_type::mouse_scroll:
				{
					input::mouse_scroll_mapping mapping{};
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
		deserializer<dict<hash::fnv32_t>>().deserialize(profile.settings, ctx);
	}

	template <>
	std::unique_ptr<control_profile> resource_loader<control_profile>::load(resources::resource_manager&, std::shared_ptr<resources::deserialize_context> ctx)
	{
		std::unique_ptr<control_profile> profile = std::make_unique<control_profile>();

		deserializer<control_profile>().deserialize(*profile, *ctx);

		return profile;
	}
}
