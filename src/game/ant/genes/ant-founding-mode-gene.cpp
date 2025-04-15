// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-founding-mode-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
import engine.resources.resource_loader;
import engine.resources.resource_manager;

namespace engine::resources
{
	void load_ant_founding_mode_phene(ant_founding_mode_phene&, resources::resource_manager&, resources::deserialize_context&)
	{

	}

	template <>
	std::unique_ptr<ant_founding_mode_gene> resource_loader<ant_founding_mode_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_founding_mode_gene> gene = std::make_unique<ant_founding_mode_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_founding_mode_phene);

		return gene;
	}
}
