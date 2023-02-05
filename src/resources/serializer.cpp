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

#include "resources/serializer.hpp"
#include <string>

template <>
void serializer<bool>::serialize(const bool& value, serialize_context& ctx)
{
	const std::uint8_t temp = (value) ? 1 : 0;
	ctx.write8(reinterpret_cast<const std::byte*>(&temp), 1);
};

template <>
void serializer<std::uint8_t>::serialize(const std::uint8_t& value, serialize_context& ctx)
{
	ctx.write8(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::uint16_t>::serialize(const std::uint16_t& value, serialize_context& ctx)
{
	ctx.write16(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::uint32_t>::serialize(const std::uint32_t& value, serialize_context& ctx)
{
	ctx.write32(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::uint64_t>::serialize(const std::uint64_t& value, serialize_context& ctx)
{
	ctx.write64(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::int8_t>::serialize(const std::int8_t& value, serialize_context& ctx)
{
	ctx.write8(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::int16_t>::serialize(const std::int16_t& value, serialize_context& ctx)
{
	ctx.write16(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::int32_t>::serialize(const std::int32_t& value, serialize_context& ctx)
{
	ctx.write32(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::int64_t>::serialize(const std::int64_t& value, serialize_context& ctx)
{
	ctx.write64(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<float>::serialize(const float& value, serialize_context& ctx)
{
	ctx.write32(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<double>::serialize(const double& value, serialize_context& ctx)
{
	ctx.write64(reinterpret_cast<const std::byte*>(&value), 1);
};

template <>
void serializer<std::string>::serialize(const std::string& value, serialize_context& ctx)
{
	const std::uint64_t length = static_cast<std::uint64_t>(value.length());
	ctx.write64(reinterpret_cast<const std::byte*>(&length), 1);
	ctx.write8(reinterpret_cast<const std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void serializer<std::u8string>::serialize(const std::u8string& value, serialize_context& ctx)
{
	const std::uint64_t length = static_cast<std::uint64_t>(value.length());
	ctx.write64(reinterpret_cast<const std::byte*>(&length), 1);
	ctx.write8(reinterpret_cast<const std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void serializer<std::u16string>::serialize(const std::u16string& value, serialize_context& ctx)
{
	const std::uint64_t length = static_cast<std::uint64_t>(value.length());
	ctx.write64(reinterpret_cast<const std::byte*>(&length), 1);
	ctx.write16(reinterpret_cast<const std::byte*>(value.data()), static_cast<std::size_t>(length));
};

template <>
void serializer<std::u32string>::serialize(const std::u32string& value, serialize_context& ctx)
{
	const std::uint64_t length = static_cast<std::uint64_t>(value.length());
	ctx.write64(reinterpret_cast<const std::byte*>(&length), 1);
	ctx.write32(reinterpret_cast<const std::byte*>(value.data()), static_cast<std::size_t>(length));
};
