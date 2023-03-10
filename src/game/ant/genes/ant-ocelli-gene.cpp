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

#include "game/ant/genes/ant-ocelli-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_ocelli_phene(ant_ocelli_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::uint8_t lateral_ocelli_present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&lateral_ocelli_present), 1);
	phene.lateral_ocelli_present = static_cast<bool>(lateral_ocelli_present);
	
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.lateral_ocelli_width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.lateral_ocelli_height), 1);
	
	std::uint8_t lateral_ocelli_model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&lateral_ocelli_model_filename_length), 1);
	std::string lateral_ocelli_model_filename(lateral_ocelli_model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(lateral_ocelli_model_filename.data()), lateral_ocelli_model_filename_length);
	
	if (phene.lateral_ocelli_present)
	{
		phene.lateral_ocelli_model = resource_manager.load<render::model>(lateral_ocelli_model_filename);
	}
	
	std::uint8_t median_ocellus_present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&median_ocellus_present), 1);
	phene.median_ocellus_present = static_cast<bool>(median_ocellus_present);
	
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.median_ocellus_width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.median_ocellus_height), 1);
	
	std::uint8_t median_ocellus_model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&median_ocellus_model_filename_length), 1);
	std::string median_ocellus_model_filename(median_ocellus_model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(median_ocellus_model_filename.data()), median_ocellus_model_filename_length);
	
	if (phene.median_ocellus_present)
	{
		phene.median_ocellus_model = resource_manager.load<render::model>(median_ocellus_model_filename);
	}
}

} // namespace

template <>
std::unique_ptr<ant_ocelli_gene> resource_loader<ant_ocelli_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_ocelli_gene> gene = std::make_unique<ant_ocelli_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_ocelli_phene);
	
	return gene;
}
