// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-pupa-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_pupa_phene(ant_pupa_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.development_period), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.eclosion_period), 1);
	
	std::uint8_t cocoon_present{0};
	ctx.read8(reinterpret_cast<std::byte*>(&cocoon_present), 1);
	phene.cocoon_present = static_cast<bool>(cocoon_present);
	
	std::uint8_t cocoon_model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&cocoon_model_filename_length), 1);
	std::string cocoon_model_filename(cocoon_model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(cocoon_model_filename.data()), cocoon_model_filename_length);
	
	if (phene.cocoon_present)
	{
		phene.cocoon_model = resource_manager.load<render::model>(cocoon_model_filename);
	}
}

} // namespace

template <>
std::unique_ptr<ant_pupa_gene> resource_loader<ant_pupa_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_pupa_gene> gene = std::make_unique<ant_pupa_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_pupa_phene);
	
	return gene;
}
