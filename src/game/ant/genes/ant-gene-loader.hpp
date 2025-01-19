// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANT_GENE_LOADER_HPP
#define ANTKEEPER_GAME_ANT_GENE_LOADER_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/resources/deserialize-context.hpp>
#include <engine/resources/deserialize-error.hpp>

/// Loads an ant gene.
/// @tparam T Phene type.
/// @param gene Gene to load.
/// @param resource_manager Resource manager.
/// @param ctx Deserialize context.
/// @param load_phene Pointer to phene load function.
template <class T>
void load_ant_gene(ant_gene<T>& gene, resource_manager& resource_manager, deserialize_context& ctx, void (*load_phene)(T&, ::resource_manager&, deserialize_context&))
{
	// Read file format identifier
	std::uint32_t format_identifier{0};
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&format_identifier), 1);
	
	// Validate file format identifier (U+1F9EC = DNA double helix)
	if (format_identifier != 0xaca79ff0)
	{
		throw deserialize_error("Invalid ant gene file");
	}
	
	// Read file format version
	std::uint16_t format_version{0};
	ctx.read16<std::endian::little>(reinterpret_cast<std::byte*>(&format_version), 1);
	
	// Validate file format version
	if (format_version != 1)
	{
		throw deserialize_error("Unsupported ant gene format");
	}
	
	// Read gene type
	ant_gene_type gene_type{0};
	ctx.read8(reinterpret_cast<std::byte*>(&gene_type), 1);
	
	// Validate gene type
	if (gene_type != gene.type())
	{
		throw deserialize_error("Mismatched ant gene type");
	}
	
	// Read gene name
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&gene.name), 1);
	
	// Read phene count
	std::uint8_t phene_count{0};
	ctx.read8(reinterpret_cast<std::byte*>(&phene_count), 1);
	
	// Allocate phenes
	gene.phenes.resize(phene_count);
	
	// Load phenes
	for (auto& phene: gene.phenes)
	{
		// Read phene caste flags
		std::uint8_t caste_flags{0};
		ctx.read8(reinterpret_cast<std::byte*>(&caste_flags), 1);
		
		// Load phene
		load_phene(phene, resource_manager, ctx);
		
		// Map flagged castes to phene
		int caste_count = std::popcount(caste_flags);
		if (caste_count == 1)
		{
			gene.phene_map[static_cast<ant_caste_type>(caste_flags)] = &phene;
		}
		else
		{
			for (std::uint8_t i = 0; i < 8; ++i)
			{
				const std::uint8_t caste_mask = 1 << i;
				if (caste_flags & caste_mask)
				{
					gene.phene_map[static_cast<ant_caste_type>(caste_mask)] = &phene;
					if (--caste_count; !caste_count)
					{
						break;
					}
				}
			}
		}
	}
}

#endif // ANTKEEPER_GAME_ANT_GENE_LOADER_HPP
