// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/typeface.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/type/freetype/ft-typeface.hpp>
#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace type {

typeface::typeface(typeface_style style, int weight):
	style(style),
	weight(weight)
{}

typeface::typeface():
	style(typeface_style::normal),
	weight(400)
{}

void typeface::set_style(typeface_style style)
{
	this->style = style;
}

void typeface::set_weight(int weight)
{
	this->weight = weight;
}

} // namespace type

template <>
std::unique_ptr<type::typeface> resource_loader<type::typeface>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	// Init FreeType library object
	FT_Library library;
	if (FT_Error error = FT_Init_FreeType(&library))
	{
		throw std::runtime_error("Failed to init FreeType library (error code " + std::to_string(error) + ")");
	}
	
	// Read file into file buffer
	std::unique_ptr<std::vector<FT_Byte>> file_buffer = std::make_unique<std::vector<FT_Byte>>(ctx.size());
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer->data()), ctx.size());
	
	// Load FreeType face from file buffer
	FT_Face face;
	if (FT_Error error = FT_New_Memory_Face(library, file_buffer->data(), static_cast<FT_Long>(file_buffer->size()), 0, &face))
	{
		FT_Done_FreeType(library);
		throw deserialize_error("Failed to load FreeType face (error code " + std::to_string(error) + ")");
	}
	
	return std::make_unique<type::ft_typeface>(library, face, std::move(file_buffer));
}
