// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-nest-site-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {

void load_ant_nest_site_phene([[maybe_unused]] ant_nest_site_phene& phene, [[maybe_unused]] ::resource_manager& resource_manager, [[maybe_unused]] deserialize_context& ctx)
{

}

} // namespace

template <>
std::unique_ptr<ant_nest_site_gene> resource_loader<ant_nest_site_gene>::load(::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
{
	std::unique_ptr<ant_nest_site_gene> gene = std::make_unique<ant_nest_site_gene>();
	
	load_ant_gene(*gene, resource_manager, *ctx, &load_ant_nest_site_phene);
	
	return gene;
}
