// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-pigmentation-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/render/material.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace engine::resources
{
	void load_ant_pigmentation_phene(ant_pigmentation_phene& phene, resources::resource_manager& resource_manager, resources::deserialize_context& ctx)
	{
		u8 material_filename_length{0};
		ctx.read8(reinterpret_cast<std::byte*>(&material_filename_length), 1);
		std::string material_filename(material_filename_length, '\0');
		ctx.read8(reinterpret_cast<std::byte*>(material_filename.data()), material_filename_length);

		phene.material = resource_manager.load<render::material>(material_filename);
	}

	template <>
	std::unique_ptr<ant_pigmentation_gene> resource_loader<ant_pigmentation_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_pigmentation_gene> gene = std::make_unique<ant_pigmentation_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_pigmentation_phene);

		return gene;
	}
}
