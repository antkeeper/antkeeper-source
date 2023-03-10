/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_ANT_GENE_LOADER_HPP
#define ANTKEEPER_GAME_ANT_GENE_LOADER_HPP

#include "game/ant/genes/ant-gene.hpp"
#include <engine/resources/resource-manager.hpp>
#include <engine/resources/deserialize-context.hpp>
#include <engine/resources/deserialize-error.hpp>

/**
 * Loads an ant gene.
 *
 * @tparam T Phene type.
 *
 * @param gene Gene to load.
 * @param resource_manager Resource manager.
 * @param ctx Deserialize context.
 * @param load_phene Pointer to phene load function.
 */
template <class T>
void load_ant_gene(ant_gene<T>& gene, resource_manager& resource_manager, deserialize_context& ctx, void (*load_phene)(T&, ::resource_manager&, deserialize_context&))
{
	// Read file format identifier
	std::uint32_t format_identifier{0};
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&format_identifier), 1);
	
	// Validate file format identifier
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
