// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/gl/texture.hpp>
#include <engine/resources/deserialize-context.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {
	
	enum texture_type: std::uint8_t
	{
		texture_type_1d,
		texture_type_1d_array,
		texture_type_2d,
		texture_type_2d_array,
		texture_type_3d,
		texture_type_cube,
		texture_type_cube_array
	};
	
	[[nodiscard]] std::unique_ptr<gl::texture> load_texture(::resource_manager& resource_manager, deserialize_context& ctx, std::uint8_t texture_type)
	{
		if (ctx.path().extension() == ".json")
		{
			// Read file into buffer
			// std::string file_buffer(ctx.size(), '\0');
			// ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());
			// 
			// // Parse JSON from file buffer
			// const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);

			return nullptr;
		}
		else
		{
			// Read file format identifier
			std::uint32_t format_identifier{0};
			ctx.read32_le(reinterpret_cast<std::byte*>(&format_identifier), 1);
			
			// Validate file format identifier (U+1F5BC = framed picture)
			if (format_identifier != 0xbc969ff0)
			{
				throw deserialize_error("Invalid texture file.");
			}
			
			// Read file format version
			std::uint32_t format_version{0};
			ctx.read32_le(reinterpret_cast<std::byte*>(&format_version), 1);
			
			// Validate file format version
			if (format_version != 1)
			{
				throw deserialize_error("Unsupported texture file version.");
			}
			
			// Read image path
			std::uint32_t image_path_length{0};
			ctx.read32_le(reinterpret_cast<std::byte*>(&image_path_length), 1);
			std::string image_path(image_path_length, '\0');
			ctx.read8(reinterpret_cast<std::byte*>(image_path.data()), image_path_length);
			
			// Load image
			std::shared_ptr<gl::image> image;
			switch (texture_type)
			{
				case texture_type_1d:
				case texture_type_1d_array:
					image = resource_manager.load<gl::image_1d>(image_path);
					break;
				
				case texture_type_2d:
				case texture_type_2d_array:
					image = resource_manager.load<gl::image_2d>(image_path);
					break;
					
				case texture_type_3d:
					image = resource_manager.load<gl::image_3d>(image_path);
					break;
				
				case texture_type_cube:
				case texture_type_cube_array:
					image = resource_manager.load<gl::image_cube>(image_path);
					break;
				
				default:
					throw deserialize_error("Invalid texture type.");
			}
			
			// Read image view parameters
			gl::format format;
			std::uint32_t first_mip_level;
			std::uint32_t mip_level_count;
			std::uint32_t first_array_layer;
			std::uint32_t array_layer_count;
			ctx.read32_le(reinterpret_cast<std::byte*>(&format), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&first_mip_level), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&mip_level_count), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&first_array_layer), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&array_layer_count), 1);
			
			// Handle automatic mip level count (`0`)
			if (!mip_level_count)
			{
				mip_level_count = image->get_mip_levels();
			}
			
			// Handle automatic array layer count (`0`)
			if (!array_layer_count)
			{
				array_layer_count = image->get_array_layers();
			}
			
			// Read sampler parameters
			gl::sampler_filter mag_filter;
			gl::sampler_filter min_filter;
			gl::sampler_mipmap_mode mipmap_mode;
			gl::sampler_address_mode address_mode_u;
			gl::sampler_address_mode address_mode_v;
			gl::sampler_address_mode address_mode_w;
			float mip_lod_bias;
			float max_anisotropy;
			std::uint8_t compare_enabled;
			gl::compare_op compare_op;
			float min_lod;
			float max_lod;
			std::array<float, 4> border_color;
			ctx.read8(reinterpret_cast<std::byte*>(&mag_filter), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&min_filter), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&mipmap_mode), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&address_mode_u), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&address_mode_v), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&address_mode_w), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&mip_lod_bias), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&max_anisotropy), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&compare_enabled), 1);
			ctx.read8(reinterpret_cast<std::byte*>(&compare_op), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&min_lod), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(&max_lod), 1);
			ctx.read32_le(reinterpret_cast<std::byte*>(border_color.data()), 4);
			
			// Construct sampler
			std::shared_ptr<gl::sampler> sampler = std::make_shared<gl::sampler>
			(
				mag_filter,
				min_filter,
				mipmap_mode,
				address_mode_u,
				address_mode_v,
				address_mode_w,
				mip_lod_bias,
				max_anisotropy,
				compare_enabled,
				compare_op,
				min_lod,
				max_lod,
				border_color
			);
			
			// Construct image view and texture
			switch (texture_type)
			{
				case texture_type_1d:
				{
					auto image_view = std::make_shared<gl::image_view_1d>(image, format, first_mip_level, mip_level_count, first_array_layer);
					return std::make_unique<gl::texture_1d>(std::move(image_view), std::move(sampler));
				}
				
				case texture_type_1d_array:
				{
					auto image_view = std::make_shared<gl::image_view_1d_array>(image, format, first_mip_level, mip_level_count, first_array_layer, array_layer_count);
					return std::make_unique<gl::texture_1d_array>(std::move(image_view), std::move(sampler));
				}
					
				case texture_type_2d:
				{
					auto image_view = std::make_shared<gl::image_view_2d>(image, format, first_mip_level, mip_level_count, first_array_layer);
					return std::make_unique<gl::texture_2d>(std::move(image_view), std::move(sampler));
				}
				
				case texture_type_2d_array:
				{
					auto image_view = std::make_shared<gl::image_view_2d_array>(image, format, first_mip_level, mip_level_count, first_array_layer, array_layer_count);
					return std::make_unique<gl::texture_2d_array>(std::move(image_view), std::move(sampler));
				}
					
				case texture_type_3d:
				{
					auto image_view = std::make_shared<gl::image_view_3d>(image, format, first_mip_level, mip_level_count);
					return std::make_unique<gl::texture_3d>(std::move(image_view), std::move(sampler));
				}
				
				case texture_type_cube:
				{
					auto image_view = std::make_shared<gl::image_view_cube>(image, format, first_mip_level, mip_level_count, first_array_layer);
					return std::make_unique<gl::texture_cube>(std::move(image_view), std::move(sampler));
				}
				
				case texture_type_cube_array:
				{
					auto image_view = std::make_shared<gl::image_view_cube_array>(image, format, first_mip_level, mip_level_count, first_array_layer, array_layer_count);
					return std::make_unique<gl::texture_cube_array>(std::move(image_view), std::move(sampler));
				}
				
				default:
					return nullptr;
			}
		}
	}
}

template <>
std::unique_ptr<gl::texture_1d> resource_loader<gl::texture_1d>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_1d>(static_cast<gl::texture_1d*>(load_texture(resource_manager, *ctx, texture_type_1d).release()));
}

template <>
std::unique_ptr<gl::texture_1d_array> resource_loader<gl::texture_1d_array>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_1d_array>(static_cast<gl::texture_1d_array*>(load_texture(resource_manager, *ctx, texture_type_1d_array).release()));
}

template <>
std::unique_ptr<gl::texture_2d> resource_loader<gl::texture_2d>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_2d>(static_cast<gl::texture_2d*>(load_texture(resource_manager, *ctx, texture_type_2d).release()));
}

template <>
std::unique_ptr<gl::texture_2d_array> resource_loader<gl::texture_2d_array>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_2d_array>(static_cast<gl::texture_2d_array*>(load_texture(resource_manager, *ctx, texture_type_2d_array).release()));
}

template <>
std::unique_ptr<gl::texture_3d> resource_loader<gl::texture_3d>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_3d>(static_cast<gl::texture_3d*>(load_texture(resource_manager, *ctx, texture_type_3d).release()));
}

template <>
std::unique_ptr<gl::texture_cube> resource_loader<gl::texture_cube>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_cube>(static_cast<gl::texture_cube*>(load_texture(resource_manager, *ctx, texture_type_cube).release()));
}

template <>
std::unique_ptr<gl::texture_cube_array> resource_loader<gl::texture_cube_array>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	return std::unique_ptr<gl::texture_cube_array>(static_cast<gl::texture_cube_array*>(load_texture(resource_manager, *ctx, texture_type_cube_array).release()));
}
