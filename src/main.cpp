/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "game/context.hpp"
#include "game/state/boot.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
	try
	{
		// Allocate game context
		game::context ctx;
		
		// Enter boot state
		ctx.state_machine.emplace(new game::state::boot(ctx, argc, argv));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unhandled exception: \"" << e.what() << "\"" << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
