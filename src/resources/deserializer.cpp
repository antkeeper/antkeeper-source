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

#include "resources/deserializer.hpp"
#include <string>

template <>
void deserializer<bool>::deserialize(bool& value, deserialize_context& ctx)
{
	std::uint8_t temp;
	ctx.read8(reinterpret_cast<std::byte*>(&temp), 1);
	value = (temp) ? true : false;
};

template <>
void deserializer<std::uint8_t>::deserialize(std::uint8_t& value, deserialize_context& ctx)
{
	ctx.read8(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::uint16_t>::deserialize(std::uint16_t& value, deserialize_context& ctx)
{
	ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::uint32_t>::deserialize(std::uint32_t& value, deserialize_context& ctx)
{
	ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::uint64_t>::deserialize(std::uint64_t& value, deserialize_context& ctx)
{
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::int8_t>::deserialize(std::int8_t& value, deserialize_context& ctx)
{
	ctx.read8(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::int16_t>::deserialize(std::int16_t& value, deserialize_context& ctx)
{
	ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::int32_t>::deserialize(std::int32_t& value, deserialize_context& ctx)
{
	ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::int64_t>::deserialize(std::int64_t& value, deserialize_context& ctx)
{
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<float>::deserialize(float& value, deserialize_context& ctx)
{
	ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<double>::deserialize(double& value, deserialize_context& ctx)
{
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
};

template <>
void deserializer<std::string>::deserialize(std::string& value, deserialize_context& ctx)
{
	std::uint64_t length = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
	value.resize(static_cast<std::size_t>(length));
	ctx.read8(reinterpret_cast<std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void deserializer<std::u8string>::deserialize(std::u8string& value, deserialize_context& ctx)
{
	std::uint64_t length = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
	value.resize(static_cast<std::size_t>(length));
	ctx.read8(reinterpret_cast<std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void deserializer<std::u16string>::deserialize(std::u16string& value, deserialize_context& ctx)
{
	std::uint64_t length = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
	value.resize(static_cast<std::size_t>(length));
	ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void deserializer<std::u32string>::deserialize(std::u32string& value, deserialize_context& ctx)
{
	std::uint64_t length = 0;
	ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
	value.resize(static_cast<std::size_t>(length));
	ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(value.data()), static_cast<std::size_t>(length));
};
