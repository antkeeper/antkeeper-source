// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-egg-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/render/model.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace engine::resources
{
	void load_ant_egg_phene(ant_egg_phene& phene, resources::resource_manager& resource_manager, resources::deserialize_context& ctx)
	{
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.incubation_period), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.eclosion_period), 1);

		u8 model_filename_length{0};
		ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
		std::string model_filename(model_filename_length, '\0');
		ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);

		phene.model = resource_manager.load<render::model>(model_filename);
	}

	template <>
	std::unique_ptr<ant_egg_gene> resource_loader<ant_egg_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_egg_gene> gene = std::make_unique<ant_egg_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_egg_phene);

		return gene;
	}
}
