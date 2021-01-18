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

#ifndef ANTKEEPER_ECS_EBT_HPP
#define ANTKEEPER_ECS_EBT_HPP

#include "ai/behavior-tree.hpp"
#include "ecs/entity.hpp"
#include "ecs/registry.hpp"

namespace ecs {

/// Entity behavior tree (EBT) nodes and context.
namespace ebt {

/**
 * EBT context which references an entity and its registry.
 */
struct context
{
	ecs::registry* registry;
	ecs::entity entity;
};

typedef ai::bt::status status;
typedef ai::bt::node<context> node;
typedef ai::bt::leaf_node<context> leaf_node;
typedef ai::bt::decorator_node<context> decorator_node;
typedef ai::bt::composite_node<context> composite_node;
typedef ai::bt::action<context> action;
typedef ai::bt::condition<context> condition;
typedef ai::bt::inverter<context> inverter;
typedef ai::bt::repeater<context> repeater;
typedef ai::bt::succeeder<context> succeeder;
typedef ai::bt::sequence<context> sequence;
typedef ai::bt::selector<context> selector;

// Actions
status print(context& context, const std::string& text);
status print_eid(context& context);
status warp_to(context& context, float x, float y, float z);

// Conditions
bool is_carrying_food(const context& context);

} // namespace ebt
} // namespace ecs

#endif // ANTKEEPER_ECS_EBT_HPP

