// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/textures/rgb-voronoi-noise.hpp"
#include <engine/debug/log.hpp>
#include <algorithm>
#include <execution>
#include <fstream>
#include <stb/stb_image_write.h>
#include <engine/noise/voronoi.hpp>

void generate_rgb_voronoi_noise(std::filesystem::path path, float frequency)
{
	int image_width = 8192;
	int image_height = image_width;
	int image_bpp = 4;

	auto image_data = std::make_unique<std::byte[]>(image_width * image_height * image_bpp);

	// auto frequency = 300.0f;
	auto scale = 1.0f / math::fvec2{static_cast<float>(image_width - 1), static_cast<float>(image_height - 1)} * frequency;

	auto thread_worker = [&](int start_y, int end_y)
	{
		std::byte* pixel = image_data.get() + start_y * image_width * image_bpp;
		math::fvec2 position;

		for (int y = start_y; y < end_y; ++y)
		{
			position.y() = static_cast<float>(y) * scale.y();

			for (int x = 0; x < image_width; ++x)
			{
				position.x() = static_cast<float>(x) * scale.x();

				const auto
				[
					f1_sqr_distance,
					f1_displacement,
					f1_id
				] = noise::voronoi_f1<float, 2>(position, 1.0f, {frequency, frequency});

				*(pixel++) = static_cast<std::byte>(f1_id & 255);
				*(pixel++) = static_cast<std::byte>((f1_id >> 8) & 255);
				*(pixel++) = static_cast<std::byte>((f1_id >> 16) & 255);
				*(pixel++) = static_cast<std::byte>((f1_id >> 24) & 255);
			}
		}
	};

	const auto thread_count = std::thread::hardware_concurrency();
	const auto rows_per_thread = image_height / thread_count;

	std::vector<std::thread> threads;
	for (std::size_t i = 0; i < thread_count; ++i)
	{
		const int start_y = static_cast<int>(i * rows_per_thread);
		const int end_y = (i == thread_count - 1) ? image_height : start_y + rows_per_thread;
		threads.emplace_back(thread_worker, start_y, end_y);
	}

	for (auto& thread: threads)
	{
		thread.join();
	}

	stbi_flip_vertically_on_write(1);
	stbi_write_png(path.string().c_str(), image_width, image_height, image_bpp, image_data.get(), image_width * image_bpp);
	/*

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
			] = noise::voronoi::f1_edge<float, 2>(position, 1.0f, {frequency, frequency});
			
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
