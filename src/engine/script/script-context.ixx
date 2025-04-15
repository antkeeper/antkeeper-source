// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.script.context;
export import <string_view>;

/// @private
export struct lua_State;

export namespace engine
{
	/// Context for executing Lua scripts.
	class script_context
	{
	public:
		/// Constructs a script context.
		script_context();

		/// Move-constructs a script context.
		script_context(script_context&& other) noexcept;

		/// Destructs a script context.
		~script_context();

		/// Move-assigns a script context.
		auto operator=(script_context&&) noexcept -> script_context&;

		script_context(const script_context&) = delete;
		auto operator=(const script_context&) -> script_context & = delete;

		/// Executes a script.
		/// @param input String containing a script to execute.
		auto execute(std::string_view input) -> void;

		/// @private
		[[nodiscard]] inline constexpr auto state() noexcept -> lua_State*
		{
			return m_state;
		}

		/// @private
		[[nodiscard]] inline constexpr auto state() const noexcept -> const lua_State*
		{
			return m_state;
		}

	private:
		lua_State* m_state{};
	};
}
