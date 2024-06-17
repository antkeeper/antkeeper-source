// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-sequence.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <format>
#include <nlohmann/json.hpp>

void animation_sequence::sample(void* context, float time) const
{
	for (const auto& [key, track]: m_tracks)
	{
		track.sample(context, time);
	}
}

/**
 * Deserializes an animation sequence.
 *
 * @param[out] sequence Animation sequence to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<animation_sequence>::deserialize(animation_sequence& sequence, deserialize_context& ctx)
{
	// Read file into buffer
	std::string file_buffer(ctx.size(), '\0');
	ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx.size());
	
	// Parse JSON from file buffer
	const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);

	// Check version string
	const auto& version = json.at("version").get_ref<const std::string&>();
	if (version != "1.0.0")
	{
		throw deserialize_error(std::format("Unsupported animation format (version {}).", version));
	}

	// Set sequence name
	sequence.name() = json.at("name").get_ref<const std::string&>();

	// Load tracks
	for (const auto& [track_path, track_element]: json.at("tracks").items())
	{
		const auto& channels_element = track_element.at("channels");

		// Allocate track
		animation_track track;

		// Allocate channels
		track.channels().resize(channels_element.size());

		// Load channels
		for (std::size_t i = 0; i < track.channels().size(); ++i)
		{
			auto& channel = track.channels()[i];
			const auto& channel_element = channels_element.at(i);
			
			// Determine interpolation mode
			const auto& interpolation_mode = channel_element.at("interpolation").get_ref<const std::string&>();
			if (interpolation_mode != "linear")
			{
				throw deserialize_error(std::format("Animation channel has unsupported interpolation mode (\"{}\").", interpolation_mode));
			}

			// Load keyframes
			const auto& keyframes_element = channel_element.at("keyframes");
			for (std::size_t j = 0; j < keyframes_element.size(); j += 2)
			{
				const auto time = keyframes_element.at(j).get<float>();
				const auto value = keyframes_element.at(j + 1).get<float>();
				channel.keyframes().emplace(time, value);
			}
		}

		// Add track to sequence
		sequence.tracks().emplace(track_path, std::move(track));
	}
}

template <>
std::unique_ptr<animation_sequence> resource_loader<animation_sequence>::load([[maybe_unused]] ::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	auto resource = std::make_unique<animation_sequence>();
	
	deserializer<animation_sequence>().deserialize(*resource, *ctx);
	
	return resource;
}

