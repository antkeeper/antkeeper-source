// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-antennae-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
import engine.render.model;
import engine.resources.resource_loader;
import engine.resources.resource_manager;

namespace engine::resources
{
	void load_ant_antennae_phene(ant_antennae_phene& phene, resources::resource_manager& resource_manager, resources::deserialize_context& ctx)
	{
		ctx.read8(reinterpret_cast<std::byte*>(&phene.total_antennomere_count), 1);
		ctx.read8(reinterpret_cast<std::byte*>(&phene.club_antennomere_count), 1);

		u8 model_filename_length{0};
		ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
		std::string model_filename(model_filename_length, '\0');
		ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);

		phene.model = resource_manager.load<render::model>(model_filename);
	}

	template <>
	std::unique_ptr<ant_antennae_gene> resource_loader<ant_antennae_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_antennae_gene> gene = std::make_unique<ant_antennae_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_antennae_phene);

		return gene;
	}
}
