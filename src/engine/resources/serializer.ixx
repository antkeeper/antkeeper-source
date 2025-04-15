// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.resources.serializer;
import engine.resources.serialize_context;

export namespace engine::resources
{
	/// Specializations of serializer define the serialization process for a given type.
	/// @tparam T Serializable type.
	template <class T>
	struct serializer
	{
		/// Serializes a value.
		/// @param value Value to serialize.
		/// @param ctx Serialize context.
		void serialize(const T& value, serialize_context& ctx);
	};
}
