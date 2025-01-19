// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-diet-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {

void load_ant_diet_phene([[maybe_unused]] ant_diet_phene& phene, [[maybe_unused]] ::resource_manager& resource_manager, [[maybe_unused]] deserialize_context& ctx)
{

}

} // namespace

template <>
std::unique_ptr<ant_diet_gene> resource_loader<ant_diet_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_diet_gene> gene = std::make_unique<ant_diet_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_diet_phene);
	
	return gene;
}
