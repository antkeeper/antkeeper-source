// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/utility/text-file.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/resource-loader.hpp>

/// Deserializes a text file.
/// @param[out] file Text file to deserialize.
/// @param[in,out] ctx Deserialize context.
/// @throw deserialize_error Read error.
template <>
void deserializer<text_file>::deserialize(text_file& file, deserialize_context& ctx)
{
	file.lines.resize(1);
	std::string* line = &file.lines.front();
	line->clear();
	
	char c;
	while (ctx.read8(reinterpret_cast<std::byte*>(&c), 1) == 1)
	{
		if (c == '\n')
		{
			line = &file.lines.emplace_back(std::string());
		}
		else if (c != '\r')
		{
			line->append(1, c);
		}
	}
	
	if (line->empty())
	{
		file.lines.pop_back();
	}
}

template <>
std::unique_ptr<text_file> resource_loader<text_file>::load([[maybe_unused]] ::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	auto resource = std::make_unique<text_file>();
	
	deserializer<text_file>().deserialize(*resource, *ctx);
	
	return resource;
}
