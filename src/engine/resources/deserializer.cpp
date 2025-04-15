// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.resources.deserializer;
import engine.resources.deserialize_context;
import engine.utility.sized_types;
import <string>;

namespace engine::resources
{
	template <>
	void deserializer<bool>::deserialize(bool& value, deserialize_context& ctx)
	{
		u8 temp;
		ctx.read8(reinterpret_cast<std::byte*>(&temp), 1);
		value = (temp) ? true : false;
	};

	template <>
	void deserializer<u8>::deserialize(u8& value, deserialize_context& ctx)
	{
		ctx.read8(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<u16>::deserialize(u16& value, deserialize_context& ctx)
	{
		ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<u32>::deserialize(u32& value, deserialize_context& ctx)
	{
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<u64>::deserialize(u64& value, deserialize_context& ctx)
	{
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<i8>::deserialize(i8& value, deserialize_context& ctx)
	{
		ctx.read8(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<i16>::deserialize(i16& value, deserialize_context& ctx)
	{
		ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<i32>::deserialize(i32& value, deserialize_context& ctx)
	{
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(&value), 1);
	};

	template <>
	void deserializer<i64>::deserialize(i64& value, deserialize_context& ctx)
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
		u64 length = 0;
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
		value.resize(static_cast<usize>(length));
		ctx.read8(reinterpret_cast<std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void deserializer<std::u8string>::deserialize(std::u8string& value, deserialize_context& ctx)
	{
		u64 length = 0;
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
		value.resize(static_cast<usize>(length));
		ctx.read8(reinterpret_cast<std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void deserializer<std::u16string>::deserialize(std::u16string& value, deserialize_context& ctx)
	{
		u64 length = 0;
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
		value.resize(static_cast<usize>(length));
		ctx.read16<std::endian::big>(reinterpret_cast<std::byte*>(value.data()), static_cast<usize>(length));
	};

	template <>
	void deserializer<std::u32string>::deserialize(std::u32string& value, deserialize_context& ctx)
	{
		u64 length = 0;
		ctx.read64<std::endian::big>(reinterpret_cast<std::byte*>(&length), 1);
		value.resize(static_cast<usize>(length));
		ctx.read32<std::endian::big>(reinterpret_cast<std::byte*>(value.data()), static_cast<usize>(length));
	};
}
