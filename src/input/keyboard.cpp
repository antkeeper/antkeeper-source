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

#include "keyboard.hpp"
#include "scancode.hpp"
#include "event/event-dispatcher.hpp"
#include "input-events.hpp"

const char* keyboard::get_scancode_name(scancode scancode)
{
	return scancode_names[static_cast<std::size_t>(scancode)];
};

scancode keyboard::get_scancode_from_name(const char* name)
{
	auto it = scancode_map.find(std::string(name));
	if (it == scancode_map.end())
	{
		return scancode::unknown;
	}

	return it->second;
}

keyboard::keyboard()
{}

keyboard::~keyboard()
{}

void keyboard::press(scancode scancode)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	key_pressed_event event;
	event.keyboard = this;
	event.scancode = scancode;

	input_device::event_dispatcher->queue(event);
}

void keyboard::release(scancode scancode)
{
	if (!input_device::event_dispatcher)
	{
		return;
	}

	key_released_event event;
	event.keyboard = this;
	event.scancode = scancode;

	input_device::event_dispatcher->queue(event);
}

std::map<std::string, scancode> keyboard::build_scancode_map()
{
	std::map<std::string, scancode> scancode_map;
	for (std::size_t i = 0; i <= static_cast<std::size_t>(scancode::audio_fast_forward); ++i)
	{
		if (scancode_names[i] != nullptr)
		{
			std::string scancode_name = scancode_names[i];
			scancode_map[scancode_name] = static_cast<scancode>(i);
		}
	}

	return scancode_map;
}

const char* keyboard::scancode_names[] =
{
	nullptr, // UNKNOWN
	"A", // A
	"B", // B
	"C", // C
	"D", // D
	"E", // E
	"F", // F
	"G", // G
	"H", // H
	"I", // I
	"J", // J
	"K", // K
	"L", // L
	"M", // M
	"N", // N
	"O", // O
	"P", // P
	"Q", // Q
	"R", // R
	"S", // S
	"T", // T
	"U", // U
	"V", // V
	"W", // W
	"X", // X
	"Y", // Y
	"Z", // Z
	"1", // ONE
	"2", // TWO
	"3", // THREE
	"4", // FOUR
	"5", // FIVE
	"6", // SIX
	"7", // SEVEN
	"8", // EIGHT
	"9", // NINE
	"0", // ZERO
	"Enter", // ENTER
	"Escape", // ESCAPE
	"Backspace", // BACKSPACE
	"Tab", // TAB
	"Space", // SPACE
	"-", // MINUS
	"=", // EQUALS
	"[", // LEFTBRACKET
	"]", // RIGHTBRACKET
	"\\", // BACKSLASH
	"#", // NONUSHASH
	";", // SEMICOLON
	"'", // APOSTROPHE
	"`", // GRAVE
	",", // COMMA
	".", // PERIOD
	"/", // SLASH
	"Caps Lock", // CAPSLOCK
	"F1", // F1
	"F2", // F2
	"F3", // F3
	"F4", // F4
	"F5", // F5
	"F6", // F6
	"F7", // F7
	"F8", // F8
	"F9", // F9
	"F10", // F10
	"F11", // F11
	"F12", // F12
	"Print Screen", // PRINTSCREEN
	"Scroll Lock", // SCROLLLOCK
	"Pause", // PAUSE
	"Insert", // INSERT
	"Home", // HOME
	"Page Up", // PAGEUP
	"Delete", // DELETE
	"End", // END
	"Page Down", // PAGEDOWN
	"Right", // RIGHT
	"Left", // LEFT
	"Down", // DOWN
	"Up", // UP
	"Num Lock", // NUMLOCKCLEAR
	"Keypad /", // KP_DIVIDE
	"Keypad *", // KP_MULTIPLY
	"Keypad -", // KP_MINUS
	"Keypad +", // KP_PLUS
	"Keypad Enter", // KP_ENTER
	"Keypad 1", // KP_1
	"Keypad 2", // KP_2
	"Keypad 3", // KP_3
	"Keypad 4", // KP_4
	"Keypad 5", // KP_5
	"Keypad 6", // KP_6
	"Keypad 7", // KP_7
	"Keypad 8", // KP_8
	"Keypad 9", // KP_9
	"Keypad 0", // KP_0
	"Keypad .", // KP_PERIOD
	nullptr, // NONUSBACKSLASH
	"Application", // APPLICATION
	"Power", // POWER
	"Keypad =", // KP_EQUALS
	"F13", // F13
	"F14", // F14
	"F15", // F15
	"F16", // F16
	"F17", // F17
	"F18", // F18
	"F19", // F19
	"F20", // F20
	"F21", // F21
	"F22", // F22
	"F23", // F23
	"F24", // F24
	"Execute", // EXECUTE
	"Help", // HELP
	"Menu", // MENU
	"Select", // SELECT
	"Stop", // STOP
	"Again", // AGAIN
	"Undo", // UNDO
	"Cut", // CUT
	"Copy", // COPY
	"Paste", // PASTE
	"Find", // FIND
	"Mute", // MUTE
	"Volume Up", // VOLUMEUP
	"Volume Down", // VOLUMEDOWN
	nullptr, // LOCKINGCAPSLOCK
	nullptr, // LOCKINGNUMLOCK
	nullptr, // LOCKINGSCROLLLOCK
	"Keypad ,", // KP_COMMA
	"Keypad = (AS400)", // KP_EQUALSAS400
	nullptr, // INTERNATIONAL1
	nullptr, // INTERNATIONAL2
	nullptr, // INTERNATIONAL3
	nullptr, // INTERNATIONAL4
	nullptr, // INTERNATIONAL5
	nullptr, // INTERNATIONAL6
	nullptr, // INTERNATIONAL7
	nullptr, // INTERNATIONAL8
	nullptr, // INTERNATIONAL9
	nullptr, // LANG1
	nullptr, // LANG2
	nullptr, // LANG3
	nullptr, // LANG4
	nullptr, // LANG5
	nullptr, // LANG6
	nullptr, // LANG7
	nullptr, // LANG8
	nullptr, // LANG9
	"Alt Erase", // ALTERASE
	"Sys Req", // SYSREQ
	"Cancel", // CANCEL
	"Clear", // CLEAR
	"Prior", // PRIOR
	"Return", // RETURN2
	"Separator", // SEPARATOR
	"Out", // OUT
	"Oper", // OPER
	"Clear/Again", // CLEARAGAIN
	"CrSel", // CRSEL
	"ExSel", // EXSEL
	"Keypad 00", // KP_00
	"Keypad 000", // KP_000
	"Thousands Separator", // THOUSANDSSEPARATOR
	"Decimal Separator", // DECIMALSEPARATOR
	"Currency Unit", // CURRENCYUNIT
	"Currency Sub-Unit", // CURRENCYSUBUNIT
	"Keypad (", // KP_LEFTPAREN
	"Keypad )", // KP_RIGHTPAREN
	"Keypad {", // KP_LEFTBRACE
	"Keypad }", // KP_RIGHTBRACE
	"Keypad Tab", // KP_TAB
	"Keypad Backspace", // KP_BACKSPACE
	"Keypad A", // KP_A
	"Keypad B", // KP_B
	"Keypad C", // KP_C
	"Keypad D", // KP_D
	"Keypad E", // KP_E
	"Keypad F", // KP_F
	"Keypad XOR", // KP_XOR
	"Keypad ^", // KP_POWER
	"Keypad %", // KP_PERCENT
	"Keypad <", // KP_LESS
	"Keypad >", // KP_GREATER
	"Keypad &", // KP_AMPERSAND
	"Keypad &&", // KP_DBLAMPERSAND
	"Keypad |", // KP_VERTICALBAR
	"Keypad ||", // KP_DBLVERTICALBAR
	"Keypad :", // KP_COLON
	"Keypad #", // KP_HASH
	"Keypad Space", // KP_SPACE
	"Keypad @", // KP_AT
	"Keypad !", // KP_EXCLAM
	"Keypad Mem Store", // KP_MEMSTORE
	"Keypad Mem Recall", // KP_MEMRECALL
	"Keypad Mem Clear", // KP_MEMCLEAR
	"Keypad Mem Add", // KP_MEMADD
	"Keypad Mem Subtract", // KP_MEMSUBTRACT
	"Keypad Mem Multiply", // KP_MEMMULTIPLY
	"Keypad Mem Divide", // KP_MEMDIVIDE
	"Keypad +/-", // KP_PLUSMINUS
	"Keypad Clear", // KP_CLEAR
	"Keypad Clear Entry", // KP_CLEARENTRY
	"Keypad Binary", // KP_BINARY
	"Keypad Octal", // KP_OCTAL
	"Keypad Decimal", // KP_DECIMAL
	"Keypad Hexadecimal", // KP_HEXADECIMAL
	"Left Ctrl", // LCTRL
	"Left Shift", // LSHIFT
	"Left Alt", // LALT
	"Left GUI", // LGUI
	"Right Ctrl", // RCTRL
	"Right Shift", // RSHIFT
	"Right Alt", // RALT
	"Right GUI", // RGUI
	"Mode Switch", // MODE
	"Audio Next", // AUDIONEXT
	"Audio Prev", // AUDIOPREV
	"Audio Stop", // AUDIOSTOP
	"Audio Play", // AUDIOPLAY
	"Audio Mute", // AUDIOMUTE
	"Media Select", // MEDIASELECT
	"WWW", // WWW
	"Mail", // MAIL
	"Calculator", // CALCULATOR
	"Computer", // COMPUTER
	"AC Search", // AC_SEARCH
	"AC Home", // AC_HOME
	"AC Back", // AC_BACK
	"AC Forward", // AC_FORWARD
	"AC Stop", // AC_STOP
	"AC Refresh", // AC_REFRESH
	"AC Bookmarks", // AC_BOOKMARKS
	"Brightness Down", // BRIGHTNESSDOWN
	"Brightness Up", // BRIGHTNESSUP
	"Display Switch", // DISPLAYSWITCH
	"KBD Illum Toggle", // KBDILLUMTOGGLE
	"KBD Illum Down", // KBDILLUMDOWN
	"KBD Illum Up", // KBDILLUMUP
	"Eject", // EJECT
	"Sleep", // SLEEP
	"App 1", // APP1
	"App 2", // APP2
	"Audio Rewind", // AUDIOREWIND
	"Audio Fast-Forward", // AUDIOFASTFORWARD
};

std::map<std::string, scancode> keyboard::scancode_map = keyboard::build_scancode_map();

