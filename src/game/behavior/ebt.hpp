/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_EBT_HPP
#define ANTKEEPER_EBT_HPP

#include "game/behavior/behavior-tree.hpp"
#include <entt/entt.hpp>

/// Entity Behavior Tree

/**
 * The `ebt` namespace defines Entity Behavior Tree (EBT) nodes and an EBT context, on which EBT nodes operate.
 */
namespace ebt {

/**
 * EBT context which references an entity and its registry.
 */
struct context
{
	entt::registry* registry;
	entt::entity entity;
};

typedef behavior_tree::status status;
typedef behavior_tree::node<context> node;
typedef behavior_tree::leaf_node<context> leaf_node;
typedef behavior_tree::decorator_node<context> decorator_node;
typedef behavior_tree::composite_node<context> composite_node;
typedef behavior_tree::action<context> action;
typedef behavior_tree::condition<context> condition;
typedef behavior_tree::inverter<context> inverter;
typedef behavior_tree::repeater<context> repeater;
typedef behavior_tree::succeeder<context> succeeder;
typedef behavior_tree::sequence<context> sequence;
typedef behavior_tree::selector<context> selector;

// Actions
status print(context& context, const std::string& text);
status print_eid(context& context);
status warp_to(context& context, float x, float y, float z);

// Conditions
bool is_carrying_food(const context& context);

} // namespace ebt

#endif // ANTKEEPER_EBT_HPP

