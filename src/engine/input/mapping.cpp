// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/input/mapping.hpp>
#include <engine/resources/serializer.hpp>
#include <engine/resources/serialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>

namespace engine::input
{
	gamepad_axis_mapping::gamepad_axis_mapping(input::gamepad* map_gamepad, gamepad_axis map_axis, bool map_direction):
		gamepad{map_gamepad},
		axis{map_axis},
		direction{map_direction}
	{}

	gamepad_button_mapping::gamepad_button_mapping(input::gamepad* map_gamepad, gamepad_button map_button):
		gamepad{map_gamepad},
		button{map_button}
	{}

	key_mapping::key_mapping(input::keyboard* map_keyboard, input::scancode map_scancode, u16 map_modifiers, bool map_repeat):
		keyboard{map_keyboard},
		scancode{map_scancode},
		repeat{map_repeat},
		modifiers{map_modifiers}
	{}

	mouse_button_mapping::mouse_button_mapping(input::mouse* map_mouse, mouse_button map_button):
		mouse{map_mouse},
		button{map_button}
	{}

	mouse_motion_mapping::mouse_motion_mapping(input::mouse* map_mouse, mouse_motion_axis map_axis, bool map_direction):
		mouse{map_mouse},
		axis{map_axis},
		direction{map_direction}
	{}

	mouse_scroll_mapping::mouse_scroll_mapping(input::mouse* map_mouse, mouse_scroll_axis map_axis, bool map_direction):
		mouse{map_mouse},
		axis{map_axis},
		direction{map_direction}
	{}
}

namespace engine::resources
{
	/// Serializes an input mapping.
	/// @param[in] value Input mapping to serialize.
	/// @param[in,out] ctx Serialize context.
	/// @throw serialize_error Write error.
	template <>
	void serializer<input::gamepad_axis_mapping>::serialize(const input::gamepad_axis_mapping& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value.axis), 1);
		const u8 direction = value.direction;
		ctx.write8(reinterpret_cast<const std::byte*>(&direction), 1);
	}

	/// @copydoc serializer<input::gamepad_axis_mapping>::serialize
	template <>
	void serializer<input::gamepad_button_mapping>::serialize(const input::gamepad_button_mapping& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value.button), 1);
	}

	/// @copydoc serializer<input::gamepad_axis_mapping>::serialize
	template <>
	void serializer<input::key_mapping>::serialize(const input::key_mapping& value, serialize_context& ctx)
	{
		ctx.write16<std::endian::big>(reinterpret_cast<const std::byte*>(&value.scancode), 1);
		ctx.write16<std::endian::big>(reinterpret_cast<const std::byte*>(&value.modifiers), 1);
		const u8 repeat = value.repeat;
		ctx.write8(reinterpret_cast<const std::byte*>(&repeat), 1);
	}

	/// @copydoc serializer<input::gamepad_axis_mapping>::serialize
	template <>
	void serializer<input::mouse_button_mapping>::serialize(const input::mouse_button_mapping& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value.button), 1);
	}

	/// @copydoc serializer<input::gamepad_axis_mapping>::serialize
	template <>
	void serializer<input::mouse_motion_mapping>::serialize(const input::mouse_motion_mapping& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value.axis), 1);
		const u8 direction = value.direction;
		ctx.write8(reinterpret_cast<const std::byte*>(&direction), 1);
	}

	/// @copydoc serializer<input::gamepad_axis_mapping>::serialize
	template <>
	void serializer<input::mouse_scroll_mapping>::serialize(const input::mouse_scroll_mapping& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value.axis), 1);
		const u8 direction = value.direction;
		ctx.write8(reinterpret_cast<const std::byte*>(&direction), 1);
	}

	/// Deserializes an input mapping.
	/// @param[out] value Input mapping to deserialize.
	/// @param[in,out] ctx Deserialize context.
	/// @throw deserialize_error Read error.
	template <>
	void deserializer<input::gamepad_axis_mapping>::deserialize(input::gamepad_axis_mapping& value, deserialize_context& ctx)
	{
		value.gamepad = nullptr;

		ctx.read8(reinterpret_cast<std::byte*>(&value.axis), 1);
		u8 direction = 0;
		ctx.read8(reinterpret_cast<std::byte*>(&direction), 1);
		value.direction = direction;
	}

	/// @copydoc deserializer<input::gamepad_axis_mapping>::deserialize
	template <>
	void deserializer<input::gamepad_button_mapping>::deserialize(input::gamepad_button_mapping& value, deserialize_context& ctx)
	{
		value.gamepad = nullptr;

		ctx.read8(reinterpret_cast<std::byte*>(&value.button), 1);
	}

	/// @copydoc deserializer<input::gamepad_axis_mapping>::deserialize
	template <>
	void deserializer<input::key_mapping>::deserialize(input::key_mapping& value, deserialize_context& ctx)
	{
		value.keyboard = nullptr;

		ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value.scancode), 1);
		ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value.modifiers), 1);
		u8 repeat = 0;
		ctx.read8(reinterpret_cast<std::byte*>(&repeat), 1);
		value.repeat = repeat;
	}

	/// @copydoc deserializer<input::gamepad_axis_mapping>::deserialize
	template <>
	void deserializer<input::mouse_button_mapping>::deserialize(input::mouse_button_mapping& value, deserialize_context& ctx)
	{
		value.mouse = nullptr;

		ctx.read8(reinterpret_cast<std::byte*>(&value.button), 1);
	}

	/// @copydoc deserializer<input::gamepad_axis_mapping>::deserialize
	template <>
	void deserializer<input::mouse_motion_mapping>::deserialize(input::mouse_motion_mapping& value, deserialize_context& ctx)
	{
		value.mouse = nullptr;

		ctx.read8(reinterpret_cast<std::byte*>(&value.axis), 1);
		u8 direction = 0;
		ctx.read8(reinterpret_cast<std::byte*>(&direction), 1);
		value.direction = direction;
	}

	/// @copydoc deserializer<input::gamepad_axis_mapping>::deserialize
	template <>
	void deserializer<input::mouse_scroll_mapping>::deserialize(input::mouse_scroll_mapping& value, deserialize_context& ctx)
	{
		value.mouse = nullptr;

		ctx.read8(reinterpret_cast<std::byte*>(&value.axis), 1);
		u8 direction = 0;
		ctx.read8(reinterpret_cast<std::byte*>(&direction), 1);
		value.direction = direction;
	}
}
