// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C"
{
	/// Requests NVIDIA Optimus to use high performance graphics.
	/// @see https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	
	/// Requests AMD PowerXpress to use high performance graphics.
	/// @see https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
	_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
