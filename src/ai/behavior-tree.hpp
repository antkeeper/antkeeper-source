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

#ifndef ANTKEEPER_AI_BEHAVIOR_TREE_HPP
#define ANTKEEPER_AI_BEHAVIOR_TREE_HPP

#include <functional>
#include <list>

namespace ai {

/// Behavior tree (BT)
namespace bt {

/// Behavior tree node return status enumerations.
enum class status
{
	failure, ///< Indicates a node's execution failed.
	success, ///< Indicates a node's execution succeed.
	running  ///< Indicates a node's execution has not finished.
};

/**
 * Abstract base class for behavior tree nodes.
 *
 * @tparam T Data type on which nodes operate.
 */
template <class T>
struct node
{
	/// Data type on which nodes operate.
	typedef T context_type;

	/**
	 * Executes a node's functionality and returns its status.
	 *
	 * @param context Context data on which the node will operate.
	 */
	virtual status execute(context_type& context) const = 0;
};

/// A node with no children.
template <class T>
using leaf_node = node<T>;

/// A node with exactly one child.
template <class T>
struct decorator_node: node<T>
{
	node* child;
};

/// A node that can have one or more children.
template <class T>
struct composite_node: node<T>
{
	std::list<node*> children;
};

/// Executes a function on a context and returns the status.
template <class T>
struct action: leaf_node<T>
{
	virtual status execute(context_type& context) const final;
	typedef std::function<status(context_type&)> function_type;
	function_type function;
};

/// Evaluates a boolean condition (predicate) and returns either `status::success` or `status::failure`.
template <class T>
struct condition: leaf_node<T>
{
	virtual status execute(context_type& context) const final;
	typedef std::function<status(const context_type&)> predicate_type;
	predicate_type predicate;
};

/// Executes a child node and returns its inverted status. If the child returns `status::success`, then `status::failure` will be returned. Otherwise if the child returns `status::failure`, then `status::success` will be returned.
template <class T>
struct inverter: decorator_node<T>
{
	virtual status execute(context_type& context) const final;
};

/// Attempts to execute a child node `n` times or until the child fails.
template <class T>
struct repeater: decorator_node<T>
{
	virtual status execute(context_type& context) const final;
	int n;
};

/// Executes a child node and returns `status::success` regardless of the child node status.
template <class T>
struct succeeder: decorator_node<T>
{
	virtual status execute(context_type& context) const final;
};

/// Attempts to execute each child node sequentially until one fails. If all children are executed successfully, `status::success` will be returned. Otherwise if any children fail, `status::failure` will be returned.
template <class T>
struct sequence: composite_node<T>
{
	virtual status execute(context_type& context) const final;
};

/// Attempts to execute each child node sequentially until one succeeds. If a child succeeds, `status::success` will be returned. Otherwise if all children fail, `status::failure` will be returned.
template <class T>
struct selector: composite_node<T>
{
	virtual status execute(context_type& context) const final;
};

template <class T>
status action<T>::execute(context_type& context) const
{
	return function(context);
}

template <class T>
status condition<T>::execute(context_type& context) const
{
	return (predicate(context)) ? status::success : status::failure;
}

template <class T>
status inverter<T>::execute(context_type& context) const
{
	status child_status = child->execute(context);
	return (child_status == status::success) ? status::failure : (child_status == status::failure) ? status::success : child_status;
}

template <class T>
status repeater<T>::execute(context_type& context) const
{
	status child_status;
	for (int i = 0; i < n; ++i)
	{
		child_status = child->execute(context);
		if (child_status == status::failure)
			break;
	}
	return child_status;
}

template <class T>
status succeeder<T>::execute(context_type& context) const
{
	child->execute(context);
	return status::success;
}

template <class T>
status sequence<T>::execute(context_type& context) const
{
	for (const node* child: children)
	{
		status child_status = child->execute(context);
		if (child_status != status::success)
			return child_status;
	}
	return status::success;
}

template <class T>
status selector<T>::execute(context_type& context) const
{
	for (const node* child: children)
	{
		status child_status = child->execute(context);
		if (child_status != status::failure)
			return child_status;
	}
	return status::failure;
}

} // namespace bt
} // namespace ai

#endif // ANTKEEPER_AI_BEHAVIOR_TREE_HPP
