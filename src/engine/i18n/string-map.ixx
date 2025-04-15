// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.i18n.string_map;
export import engine.hash.fnv;
export import <string>;
export import <unordered_map>;

export namespace engine::i18n
{
	/// Maps 32-bit keys to strings.
	using string_map = std::unordered_map<hash::fnv32_t, std::string>;
}
