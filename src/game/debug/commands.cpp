// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/debug/commands.hpp"
#include "game/debug/shell-buffer.hpp"
#include "game/world.hpp"
#include <engine/config.hpp>
#include "game/systems/astronomy-system.hpp"
#include <engine/physics/time/constants.hpp>
#include <engine/debug/log.hpp>

namespace {
	
	int command_clear([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, [[maybe_unused]] std::ostream& cerr, ::game* ctx)
	{
		ctx->shell_buffer->str("");
		return 0;
	}
	
	int command_exit([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, [[maybe_unused]] std::ostream& cerr, ::game* ctx)
	{
		ctx->closed = true;
		return 0;
	}
	
	/** Prints a string in the current language. */
	int command_string(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr, const ::game* ctx)
	{
		if (arguments.size() != 2)
		{
			return 1;
		}
		
		if (auto it = ctx->string_map->find(arguments[1]); it != ctx->string_map->end())
		{
			cout << it->get_ref<const std::string&>() << '\n';
			return 0;
		}
		
		return 404;
	}
	
	/** Copies text to or pastes text from the clipboard. */
	int command_clipboard(std::span<const std::string> arguments, std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr, ::game* ctx)
	{
		if (arguments.size() == 2)
		{
			if (arguments[1] == "copy")
			{
				std::ostringstream contents;
				std::string line;
				if (std::getline(cin, line))
				{
					contents << line;
					while (std::getline(cin, line))
					{
						contents << '\n' << line;
					}
				}
				
				ctx->input_manager->set_clipboard_text(contents.str());
				return 0;
			}
			else if (arguments[1] == "paste")
			{
				cout << ctx->input_manager->get_clipboard_text() << '\n';
				return 0;
			}
		}
		
		return 1;
	}
	
	int command_time(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr, ::game* ctx)
	{
		if (arguments.size() == 1)
		{
			cout << std::format("{}\n", ctx->astronomy_system->get_time());
			return 0;
		}
		
		if (arguments.size() == 2)
		{
			double time;
			if (!(std::istringstream(arguments[1]) >> time))
			{
				return 1;
			}
			
			world::set_time(*ctx, time);
			return 0;
		}
		
		return 1;
	}
	
	int command_timescale(std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, std::ostream& cout, [[maybe_unused]] std::ostream& cerr, ::game* ctx)
	{
		if (arguments.size() == 1)
		{
			cout << std::format("{}\n", ctx->astronomy_system->get_time_scale() * physics::time::seconds_per_day<double>);
			return 0;
		}
		
		if (arguments.size() == 2)
		{
			double timescale;
			if (!(std::istringstream(arguments[1]) >> timescale))
			{
				return 1;
			}
			
			world::set_time_scale(*ctx, timescale);
			return 0;
		}
		
		return 1;
	}
	
	int command_sound([[maybe_unused]] std::span<const std::string> arguments, [[maybe_unused]] std::istream& cin, [[maybe_unused]] std::ostream& cout, [[maybe_unused]] std::ostream& cerr, [[maybe_unused]] ::game* ctx)
	{
		// ctx->test_sound->play();
		return 0;
	}
}

void register_commands(::shell& shell, ::game& ctx)
{
	shell.set_command("clear", std::bind_back(command_clear, &ctx));
	shell.set_command("clipboard", std::bind_back(command_clipboard, &ctx));
	shell.set_command("exit", std::bind_back(command_exit, &ctx));
	shell.set_command("string", std::bind_back(command_string, &ctx));
	shell.set_command("time", std::bind_back(command_time, &ctx));
	shell.set_command("timescale", std::bind_back(command_timescale, &ctx));
	shell.set_command("sound", std::bind_back(command_sound, &ctx));
}
