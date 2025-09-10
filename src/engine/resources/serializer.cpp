// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/resources/serializer.hpp>
#include <engine/resources/serialize-context.hpp>
#include <engine/utility/sized-types.hpp>
#include <string>

namespace engine::resources
{
	template <>
	void serializer<bool>::serialize(const bool& value, serialize_context& ctx)
	{
		const u8 temp = (value) ? 1 : 0;
		ctx.write8(reinterpret_cast<const std::byte*>(&temp), 1);
	};

	template <>
	void serializer<u8>::serialize(const u8& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<u16>::serialize(const u16& value, serialize_context& ctx)
	{
		ctx.write16<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<u32>::serialize(const u32& value, serialize_context& ctx)
	{
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<u64>::serialize(const u64& value, serialize_context& ctx)
	{
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<i8>::serialize(const i8& value, serialize_context& ctx)
	{
		ctx.write8(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<i16>::serialize(const i16& value, serialize_context& ctx)
	{
		ctx.write16<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<i32>::serialize(const i32& value, serialize_context& ctx)
	{
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<i64>::serialize(const i64& value, serialize_context& ctx)
	{
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<float>::serialize(const float& value, serialize_context& ctx)
	{
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<double>::serialize(const double& value, serialize_context& ctx)
	{
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&value), 1);
	};

	template <>
	void serializer<std::string>::serialize(const std::string& value, serialize_context& ctx)
	{
		const u64 length = static_cast<u64>(value.length());
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&length), 1);
		ctx.write8(reinterpret_cast<const std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void serializer<std::u8string>::serialize(const std::u8string& value, serialize_context& ctx)
	{
		const u64 length = static_cast<u64>(value.length());
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&length), 1);
		ctx.write8(reinterpret_cast<const std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void serializer<std::u16string>::serialize(const std::u16string& value, serialize_context& ctx)
	{
		const u64 length = static_cast<u64>(value.length());
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&length), 1);
		ctx.write16<std::endian::big>(reinterpret_cast<const std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void serializer<std::u32string>::serialize(const std::u32string& value, serialize_context& ctx)
	{
		const u64 length = static_cast<u64>(value.length());
		ctx.write64<std::endian::big>(reinterpret_cast<const std::byte*>(&length), 1);
		ctx.write32<std::endian::big>(reinterpret_cast<const std::byte*>(value.data()), static_cast<usize>(length));
	};
}
