// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-sting-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_sting_phene(ant_sting_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::uint8_t present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&present), 1);
	phene.present = static_cast<bool>(present);
	
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
std::unique_ptr<ant_sting_gene> resource_loader<ant_sting_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_sting_gene> gene = std::make_unique<ant_sting_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_sting_phene);
	
	return gene;
}
