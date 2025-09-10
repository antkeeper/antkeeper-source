// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-foraging-time-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace engine::resources
{
	void load_ant_foraging_time_phene(ant_foraging_time_phene& phene, resources::resource_manager&, resources::deserialize_context& ctx)
	{
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.min_solar_altitude), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.max_solar_altitude), 1);
	}

	template <>
	std::unique_ptr<ant_foraging_time_gene> resource_loader<ant_foraging_time_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_foraging_time_gene> gene = std::make_unique<ant_foraging_time_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_foraging_time_phene);

		return gene;
	}
}
