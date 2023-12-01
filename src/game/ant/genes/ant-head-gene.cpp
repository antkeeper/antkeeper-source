// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-head-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_head_phene(ant_head_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.phragmosis), 1);
	
	std::uint8_t model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
	std::string model_filename(model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);
	
	phene.model = resource_manager.load<render::model>(model_filename);
}

} // namespace

template <>
std::unique_ptr<ant_head_gene> resource_loader<ant_head_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_head_gene> gene = std::make_unique<ant_head_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_head_phene);
	
	return gene;
}
