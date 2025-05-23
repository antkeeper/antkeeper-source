// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.debug.contract;

namespace engine::debug
{
	const char* contract_error::what() const noexcept
	{
		return "Contract violated";
	}

	const char* precondition_error::what() const noexcept
	{
		return "Precondition violated";
	}

	const char* postcondition_error::what() const noexcept
	{
		return "Postcondition violated";
	}

	const char* invariant_error::what() const noexcept
	{
		return "Invariant violated";
	}
}
