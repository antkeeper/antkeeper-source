// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-pilosity-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace engine::resources
{
	void load_ant_pilosity_phene(ant_pilosity_phene& phene, resources::resource_manager&, resources::deserialize_context& ctx)
	{
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.density), 1);
	}

	template <>
	std::unique_ptr<ant_pilosity_gene> resource_loader<ant_pilosity_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_pilosity_gene> gene = std::make_unique<ant_pilosity_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_pilosity_phene);

		return gene;
	}
}
