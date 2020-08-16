/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "application.hpp"
#include "game/bootloader.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
	auto wrapped_bootloader = std::bind(bootloader, std::placeholders::_1, argc, argv);
	
	try
	{
		return application().execute(wrapped_bootloader);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unhandled exception: \"" << e.what() << "\"" << std::endl;
	}
	
	return EXIT_FAILURE;
}
