// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/ant/genes/ant-founding-mode-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {

void load_ant_founding_mode_phene([[maybe_unused]] ant_founding_mode_phene& phene, [[maybe_unused]] ::resource_manager& resource_manager, [[maybe_unused]] deserialize_context& ctx)
{

}

} // namespace

template <>
std::unique_ptr<ant_founding_mode_gene> resource_loader<ant_founding_mode_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_founding_mode_gene> gene = std::make_unique<ant_founding_mode_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_founding_mode_phene);
	
	return gene;
}
