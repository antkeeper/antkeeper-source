// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-legs-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/render/model.hpp>

namespace {

void load_ant_legs_phene(ant_legs_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.standing_height), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.creeping_speed), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.walking_speed), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.running_speed), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.stride_length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.max_angular_frequency), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.grip), 1);
	
	std::uint8_t model_filename_length{0};
	ctx.read8(reinterpret_cast<std::byte*>(&model_filename_length), 1);
	std::string model_filename(model_filename_length, '\0');
	ctx.read8(reinterpret_cast<std::byte*>(model_filename.data()), model_filename_length);
	
	phene.model = resource_manager.load<render::model>(model_filename);
}

} // namespace

template <>
std::unique_ptr<ant_legs_gene> resource_loader<ant_legs_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_legs_gene> gene = std::make_unique<ant_legs_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_legs_phene);
	
	return gene;
}
