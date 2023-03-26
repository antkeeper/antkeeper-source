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

#include "game/ant/genes/ant-waist-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_waist_phene(ant_waist_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::uint8_t present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&present), 1);
	phene.present = static_cast<bool>(present);
	
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_height), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_spinescence), 1);
	
	std::uint8_t postpetiole_present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&postpetiole_present), 1);
	phene.postpetiole_present = static_cast<bool>(postpetiole_present);
	
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_height), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_spinescence), 1);
	
	std::uint8_t model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
	std::string model_filename(model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);
	
	if (phene.present)
	{
		phene.model = resource_manager.load<render::model>(model_filename);
	}
}

} // namespace

template <>
std::unique_ptr<ant_waist_gene> resource_loader<ant_waist_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_waist_gene> gene = std::make_unique<ant_waist_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_waist_phene);
	
	return gene;
}
