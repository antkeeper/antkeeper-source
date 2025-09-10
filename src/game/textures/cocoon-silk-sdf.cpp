// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <execution>
#include <stb/stb_image_write.h>
#include "game/textures/cocoon-silk-sdf.hpp"
#include <engine/debug/log.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/functions.hpp>
#include <algorithm>
#include <fstream>

using namespace engine;

void generate_cocoon_silk_sdf(std::filesystem::path /*path*/)
{
	/*
	debug::log_info("Generating cocoon silk SDF image...");
	
	image img;
	img.format(4, 8);
	img.resize(2048, 2048);
	
	auto width = img.width();
	auto height = img.height();
	unsigned char* pixels = (unsigned char*)img.data();
	
	const float frequency = 100.0f;
	float scale_x = 1.0f / static_cast<float>(width - 1) * frequency;
	float scale_y = 1.0f / static_cast<float>(height - 1) * frequency;
	
	std::for_each
	(
		std::execution::par_unseq,
		img.begin<math::vec4<unsigned char>>(),
		img.end<math::vec4<unsigned char>>(),
		[pixels, width, height, scale_x, scale_y, frequency](auto& pixel)
		{
			const usize i = &pixel - (math::vec4<unsigned char>*)pixels;
			const usize y = i / width;
			const usize x = i % width;
			
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
			] = noise::voronoi::f1_edge<float, 2>(position, 1.0f, {frequency, frequency});
			
			const float f1_edge_distance = math::sqrt(f1_edge_sqr_distance);
			
			const float scale = 255.0f * (255.0f / 204.0f);
			pixel = 
			{
				static_cast<unsigned char>(math::min(255.0f, f1_edge_distance * scale)),
				static_cast<unsigned char>(math::min(255.0f, f1_edge_distance * scale)),
				static_cast<unsigned char>(math::min(255.0f, f1_edge_distance * scale)),
				255
			};
			
			// const float f1_distance = math::sqrt(f1_sqr_distance);
			// const math::fvec2 uv = (position + f1_displacement) / frequency;
			
			// pixel = 
			// {
				// static_cast<unsigned char>(math::min(255.0f, f1_distance * 255.0f)),
				// static_cast<unsigned char>(math::min(255.0f, uv[0] * 255.0f)),
				// static_cast<unsigned char>(math::min(255.0f, uv[1] * 255.0f)),
				// static_cast<unsigned char>(f1_id % 256)
			// };
		}
	);
	debug::log_info("Generated cocoon silk SDF image");
	
	debug::log_info("Saving cocoon silk SDF image to \"{}\"...", path.string());
	
	stbi_flip_vertically_on_write(1);
	stbi_write_png(path.string().c_str(), img.width(), img.height(), img.channel_count(), img.data(), img.width() * img.channel_count());
	
	debug::log_info("Saved cocoon silk SDF image to \"{}\"", path.string());
	*/
}
