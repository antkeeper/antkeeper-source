// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-waist-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/render/model.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace engine::resources
{
	void load_ant_waist_phene(ant_waist_phene& phene, resources::resource_manager& resource_manager, resources::deserialize_context& ctx)
	{
		u8 present{0};
		ctx.read8(reinterpret_cast<std::byte*>(&present), 1);
		phene.present = static_cast<bool>(present);

		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_length), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_width), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_height), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.petiole_spinescence), 1);

		u8 postpetiole_present{0};
		ctx.read8(reinterpret_cast<std::byte*>(&postpetiole_present), 1);
		phene.postpetiole_present = static_cast<bool>(postpetiole_present);

		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_length), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_width), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_height), 1);
		ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.postpetiole_spinescence), 1);

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
	std::unique_ptr<ant_waist_gene> resource_loader<ant_waist_gene>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		std::unique_ptr<ant_waist_gene> gene = std::make_unique<ant_waist_gene>();

		load_ant_gene(*gene, resource_manager, *ctx, &load_ant_waist_phene);

		return gene;
	}
}
