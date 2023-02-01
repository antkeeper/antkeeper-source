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

#include "resource-loader.hpp"
#include "resource-manager.hpp"
#include "entity/ebt.hpp"
#include <nlohmann/json.hpp>
#include <functional>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <physfs.h>

/*

template <class T>
void parse_argument(T& value, const std::string& string)
{
	std::istringstream stream(string);
	stream >> value;
}

template <>
void parse_argument(std::string& value, const std::string& string)
{
	value = string;
}

template <class T, class... Args>
std::function<entity::ebt::status(entity::ebt::context&)> pack_function(T (*function)(entity::ebt::context&, Args...), std::list<std::string> argv)
{
	//if (argv.size() != sizeof...(Args))

	// Parse list of strings into tuple of arguments
	std::tuple<std::decay_t<Args>...> arguments;
	if constexpr (sizeof...(Args) > 0)
	{
		std::apply([&argv](auto& element, auto&... elements)
		{
			parse_argument(element, argv.front());
			argv.pop_front();
		},
		arguments);
	}

	return std::bind(
		[function, arguments](entity::ebt::context& context) -> entity::ebt::status
		{
			return std::apply(function, std::tuple_cat(std::make_tuple(context), arguments));
		},
		std::placeholders::_1);
}

static entity::ebt::node* load_node(const nlohmann::json::const_iterator& json, resource_manager* resource_manager);
static void load_node_child(entity::ebt::decorator_node* node, const nlohmann::json& json, resource_manager* resource_manager);
static void load_node_children(entity::ebt::composite_node* node, const nlohmann::json& json, resource_manager* resource_manager);

static entity::ebt::node* load_action_node(const nlohmann::json& json, resource_manager* resource_manager)
{
	// Get function name
	auto function_it = json.find("function");
	if (function_it == json.end())
		throw std::runtime_error("load_action_node(): Action node has no function.");
	std::string function_name = function_it.value().get<std::string>();

	// Get argument vector
	std::list<std::string> arguments;
	auto arguments_it = json.find("arguments");
	for (auto it = arguments_it.value().cbegin(); it != arguments_it.value().cend(); ++it)
		arguments.push_back(it.value().get<std::string>());
	
	entity::ebt::action* action_node = new entity::ebt::action();
	if (function_name == "print") action_node->function = pack_function(entity::ebt::print, arguments);
	else if (function_name == "print_eid") action_node->function = pack_function(entity::ebt::print_eid, arguments);
	else if (function_name == "warp_to") action_node->function = pack_function(entity::ebt::warp_to, arguments);
	
	return action_node;
}

static entity::ebt::node* load_selector_node(const nlohmann::json& json, resource_manager* resource_manager)
{
	entity::ebt::selector* selector_node = new entity::ebt::selector();
	load_node_children(selector_node, json, resource_manager);
	return selector_node;
}

static entity::ebt::node* load_sequence_node(const nlohmann::json& json, resource_manager* resource_manager)
{
	entity::ebt::sequence* sequence_node = new entity::ebt::sequence();
	load_node_children(sequence_node, json, resource_manager);
	return sequence_node;
}

static entity::ebt::node* load_node(const nlohmann::json::const_iterator& json, resource_manager* resource_manager)
{
	static const std::map<std::string, std::function<entity::ebt::node*(const nlohmann::json&, ::resource_manager*)>> node_loaders =
	{
		{"action", &load_action_node},
		{"selector", &load_selector_node},
		{"sequence", &load_sequence_node}
	};

	auto node_loader = node_loaders.find(json.key());
	if (node_loader == node_loaders.end())
	{
		throw std::runtime_error("load_node(): Unknown behavior tree node type \"" + json.key() + "\"");
	}
	
	return node_loader->second(json.value(), resource_manager);
}

static void load_node_child(entity::ebt::decorator_node* node, const nlohmann::json& json, resource_manager* resource_manager)
{
	auto it = json.find("child");
	node->child = load_node(it.value().cbegin(), resource_manager);
}

static void load_node_children(entity::ebt::composite_node* node, const nlohmann::json& json, resource_manager* resource_manager)
{
	auto children_it = json.find("children");
	for (auto it = children_it.value().cbegin(); it != children_it.value().cend(); ++it)
	{
		entity::ebt::node* child = load_node(it.value().begin(), resource_manager);
		node->children.push_back(child);
	}
}

template <>
entity::ebt::node* resource_loader<entity::ebt::node>::load(resource_manager* resource_manager, PHYSFS_File* file, const std::filesystem::path& path)
{
	// Read file into buffer
	std::size_t size = static_cast<int>(PHYSFS_fileLength(file));
	std::string buffer;
	buffer.resize(size);
	PHYSFS_readBytes(file, &buffer[0], size);
	
	// Parse json from file buffer
	nlohmann::json json = nlohmann::json::parse(buffer);

	if (json.size() != 1)
	{
		throw std::runtime_error("resource_loader<entity::ebt::node>::load(): Behavior tree must have exactly one root node.");
	}

	return load_node(json.cbegin(), resource_manager);
}
*/
