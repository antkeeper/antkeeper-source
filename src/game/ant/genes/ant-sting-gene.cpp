// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-sting-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
import engine.render.model;
import engine.resources.resource_loader;
import engine.resources.resource_manager;

namespace engine::resources
{
	void load_ant_sting_phene(ant_sting_phene& phene, resources::resource_manager& resource_manager, resources::deserialize_context& ctx)
	{
		u8 present{0};
		ctx.read8(reinterpret_cast<std::byte*>(&present), 1);
		phene.present = static_cast<bool>(present);

		u8 model_filename_length{0};
		ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
		std::string model_filename(model_filename_length, '\0');
		ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);

		if (phene.present)
		{
			phene.model = resource_manager.load<render::model>(model_filename);
		}
	}

	template <>
	std::unique_ptr<ant_sting_gene> resource_loader<ant_sting_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_sting_gene> gene = std::make_unique<ant_sting_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_sting_phene);

		return gene;
	}
}
