// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/textures/rgb-voronoi-noise.hpp"
#include <engine/math/noise/noise.hpp>
#include <engine/debug/log.hpp>
#include <algorithm>
#include <execution>
#include <fstream>
#include <stb/stb_image_write.h>

void generate_rgb_voronoi_noise(std::filesystem::path path)
{
	/*
	image img;
	img.format(4, 8);
	img.resize(1024, 1024);
	
	auto width = img.width();
	auto height = img.height();
	unsigned char* pixels = (unsigned char*)img.data();
	
	const float frequency = 512.0f;
	float scale_x = 1.0f / static_cast<float>(width - 1) * frequency;
	float scale_y = 1.0f / static_cast<float>(height - 1) * frequency;
	
	std::for_each
	(
		std::execution::par_unseq,
		img.begin<math::vec4<unsigned char>>(),
		img.end<math::vec4<unsigned char>>(),
		[pixels, width, height, scale_x, scale_y, frequency](auto& pixel)
		{
			const std::size_t i = &pixel - (math::vec4<unsigned char>*)pixels;
			const std::size_t y = i / width;
			const std::size_t x = i % width;
			
			const math::fvec2 position =
			{
				static_cast<float>(x) * scale_x,
				static_cast<float>(y) * scale_y
			};
			
			const auto
			[
				f1_sqr_distance,
				f1_displacement,
				f1_id,
				f1_edge_sqr_distance
			] = math::noise::voronoi::f1_edge<float, 2>(position, 1.0f, {frequency, frequency});
			
			const float f1_edge_distance = std::sqrt(f1_edge_sqr_distance);
			
			const float scale = 255.0f * (255.0f / 204.0f);
			pixel = 
			{
				static_cast<unsigned char>(f1_id & 255),
				static_cast<unsigned char>((f1_id >> 8) & 255),
				static_cast<unsigned char>((f1_id >> 16) & 255),
				static_cast<unsigned char>((f1_id >> 24) & 255)
			};
			
			// const float f1_distance = std::sqrt(f1_sqr_distance);
			// const math::fvec2 uv = (position + f1_displacement) / frequency;
			
			// pixel = 
			// {
				// static_cast<unsigned char>(std::min(255.0f, f1_distance * 255.0f)),
				// static_cast<unsigned char>(std::min(255.0f, uv[0] * 255.0f)),
				// static_cast<unsigned char>(std::min(255.0f, uv[1] * 255.0f)),
				// static_cast<unsigned char>(f1_id % 256)
			// };
		}
	);
	
	stbi_flip_vertically_on_write(1);
	stbi_write_png(path.string().c_str(), img.width(), img.height(), img.channel_count(), img.data(), img.width() * img.channel_count());
	*/
}
