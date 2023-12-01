// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	
	std::uint8_t median_ocellus_present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&median_ocellus_present), 1);
	phene.median_ocellus_present = static_cast<bool>(median_ocellus_present);
	
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.median_ocellus_width), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.median_ocellus_height), 1);
	
	std::uint8_t model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
	std::string model_filename(model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);
	
	if (phene.lateral_ocelli_present || phene.median_ocellus_present)
	{
		phene.model = resource_manager.load<render::model>(model_filename);
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
