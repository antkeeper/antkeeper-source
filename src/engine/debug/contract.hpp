// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <exception>

namespace engine::debug
{
	/// @name Contracts
	/// @{

	/// Thrown when a contract is violated.
	class contract_error: public std::exception
	{
	public:
		~contract_error() override = default;
		[[nodiscard]] const char* what() const noexcept override;
	};

	/// Thrown when a precondition is violated.
	class precondition_error: public contract_error
	{
	public:
		~precondition_error() override = default;
		[[nodiscard]] const char* what() const noexcept override;
	};

	/// Thrown when a postcondition is violated.
	class postcondition_error: public contract_error
	{
	public:
		~postcondition_error() override = default;
		[[nodiscard]] const char* what() const noexcept override;
	};

	///	Thrown when an invariant is violated.
	class invariant_error: public contract_error
	{
	public:
		~invariant_error() override = default;
		[[nodiscard]] const char* what() const noexcept override;
	};

	/// Enforces a precondition contract.
	/// @param condition Contract condition.
	/// @exception precondition_error Precondition violated.
	inline void precondition([[maybe_unused]] bool condition)
	{
		#if !defined(NDEBUG)
			if (!condition)
			{
				throw precondition_error{};
			}
		#endif
	}

	/// Enforces a postcondition contract.
	/// @param condition Contract condition.
	/// @exception postcondition_error Postcondition violated.
	inline void postcondition([[maybe_unused]] bool condition)
	{
		#if !defined(NDEBUG)
			if (!condition)
			{
				throw postcondition_error{};
			}
		#endif
	}

	/// Enforces an invariant contract.
	/// @param condition Contract condition.
	/// @exception invariant_error Invariant violated.
	inline void invariant([[maybe_unused]] bool condition)
	{
		#if !defined(NDEBUG)
			if (!condition)
			{
				throw invariant_error{};
			}
		#endif
	}

	/// @}
}
