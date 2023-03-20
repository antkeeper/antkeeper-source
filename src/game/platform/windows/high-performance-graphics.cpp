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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C"
{
	/**
	 * Requests NVIDIA Optimus to use high performance graphics.
	 *
	 * @see https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
	 */
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	
	/**
	 * Requests AMD PowerXpress to use high performance graphics.
	 *
	 * @see https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
	 */
	_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
