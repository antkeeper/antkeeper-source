// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-body-size-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {

void load_ant_body_size_phene(ant_body_size_phene& phene, [[maybe_unused]] ::resource_manager& resource_manager, deserialize_context& ctx)
{
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.min_mesosoma_length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.max_mesosoma_length), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&phene.mean_mesosoma_length), 1);
}

} // namespace

template <>
std::unique_ptr<ant_body_size_gene> resource_loader<ant_body_size_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_body_size_gene> gene = std::make_unique<ant_body_size_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_body_size_phene);
	
	return gene;
}
