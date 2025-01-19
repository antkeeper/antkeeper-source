// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/resources/deserializer.hpp>
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
