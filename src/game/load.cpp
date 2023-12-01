// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/load.hpp"
#include <engine/debug/log.hpp>
#include <engine/utility/json.hpp>
#include <engine/resources/resource-manager.hpp>

namespace load {

void colony(::game& ctx, const std::filesystem::path& path)
{
	// const std::string path_string = path.string();
	
	// debug::log_trace("Loading colony from \"{}\"...", path_string);
	// try
	// {
		// json* data = ctx.resource_manager->load<json>(path);
		
		// debug::log_trace("Loaded colony from \"{}\"", path_string);
	// }
	// catch (...)
	// {
		// debug::log_error("Failed to load colony from \"{}\"", path_string);
	// }
}

} // namespace load
