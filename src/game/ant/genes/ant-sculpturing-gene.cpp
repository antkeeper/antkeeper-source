// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-sculpturing-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/texture.hpp>

namespace {

void load_ant_sculpturing_phene(ant_sculpturing_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.roughness), 1);
	
	std::uint8_t normal_map_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&normal_map_filename_length), 1);
	std::string normal_map_filename(normal_map_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(normal_map_filename.data()), normal_map_filename_length);
	
	phene.normal_map = resource_manager.load<gl::texture_2d>(normal_map_filename);
}

} // namespace

template <>
std::unique_ptr<ant_sculpturing_gene> resource_loader<ant_sculpturing_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_sculpturing_gene> gene = std::make_unique<ant_sculpturing_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_sculpturing_phene);
	
	return gene;
}
