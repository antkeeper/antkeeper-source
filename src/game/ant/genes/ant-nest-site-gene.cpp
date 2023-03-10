/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "game/ant/genes/ant-nest-site-gene.hpp"
#include "game/ant/genes/ant-gene-loader.hpp"
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>

namespace {

void load_ant_nest_site_phene(ant_nest_site_phene& phene, ::resource_manager& resource_manager, deserialize_context& ctx)
{

}

} // namespace

template <>
std::unique_ptr<ant_nest_site_gene> resource_loader<ant_nest_site_gene>::load(::resource_manager& resource_manager, deserialize_context& ctx)
{
	std::unique_ptr<ant_nest_site_gene> gene = std::make_unique<ant_nest_site_gene>();
	
	load_ant_gene(*gene, resource_manager, ctx, &load_ant_nest_site_phene);
	
	return gene;
}
