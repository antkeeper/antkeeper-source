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

#include "game/ant/genes/ant-pigmentation-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/material.hpp>

namespace {

void load_ant_pigmentation_phene(ant_pigmentation_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::uint8_t material_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&material_filename_length), 1);
	std::string material_filename(material_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(material_filename.data()), material_filename_length);
	
	phene.material = resource_manager.load<render::material>(material_filename);
}

} // namespace

template <>
std::unique_ptr<ant_pigmentation_gene> resource_loader<ant_pigmentation_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_pigmentation_gene> gene = std::make_unique<ant_pigmentation_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_pigmentation_phene);
	
	return gene;
}
