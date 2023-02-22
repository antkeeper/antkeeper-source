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

#ifndef ANTKEEPER_AI_BT_NODE_HPP
#define ANTKEEPER_AI_BT_NODE_HPP

#include <engine/ai/bt/status.hpp>

namespace ai {
namespace bt {

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
	 * Executes a node's function and returns its status.
	 *
	 * @param context Context data on which the node will operate.
	 */
	virtual status execute(context_type& context) const = 0;
};

/// Behavior tree node with no children.
template <class T>
using leaf_node = node<T>;


/// A node with exactly one child.
template <class T>
struct decorator_node: public node<T>
{
	node<T>* child;
};

/// A node that can have one or more children.
template <class T>
struct composite_node: public node<T>
{
	std::list<node<T>*> children;
};

/// Executes a function on a context and returns the status.
template <class T>
struct action: public leaf_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
	typedef std::function<status(node<T>::context_type&)> function_type;
	function_type function;
};

/// Evaluates a boolean condition (predicate) and returns either `status::success` or `status::failure`.
template <class T>
struct condition: public leaf_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
	typedef std::function<status(const node<T>::context_type&)> predicate_type;
	predicate_type predicate;
};

/// Executes a child node and returns its inverted status. If the child returns `status::success`, then `status::failure` will be returned. Otherwise if the child returns `status::failure`, then `status::success` will be returned.
template <class T>
struct inverter: public decorator_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
};

/// Attempts to execute a child node `n` times or until the child fails.
template <class T>
struct repeater: public decorator_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
	int n;
};

/// Executes a child node and returns `status::success` regardless of the child node status.
template <class T>
struct succeeder: public decorator_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
};

/// Attempts to execute each child node sequentially until one fails. If all children are executed successfully, `status::success` will be returned. Otherwise if any children fail, `status::failure` will be returned.
template <class T>
struct sequence: public composite_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
};

/// Attempts to execute each child node sequentially until one succeeds. If a child succeeds, `status::success` will be returned. Otherwise if all children fail, `status::failure` will be returned.
template <class T>
struct selector: public composite_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
};

template <class T>
status action<T>::execute(node<T>::context_type& context) const
{
	return function(context);
}

template <class T>
status condition<T>::execute(node<T>::context_type& context) const
{
	return (predicate(context)) ? status::success : status::failure;
}

template <class T>
status inverter<T>::execute(node<T>::context_type& context) const
{
	status child_status = decorator_node<T>::child->execute(context);
	return (child_status == status::success) ? status::failure : (child_status == status::failure) ? status::success : child_status;
}

template <class T>
status repeater<T>::execute(node<T>::context_type& context) const
{
	status child_status;
	for (int i = 0; i < n; ++i)
	{
		child_status = decorator_node<T>::child->execute(context);
		if (child_status == status::failure)
			break;
	}
	return child_status;
}

template <class T>
status succeeder<T>::execute(node<T>::context_type& context) const
{
	decorator_node<T>::child->execute(context);
	return status::success;
}

template <class T>
status sequence<T>::execute(node<T>::context_type& context) const
{
	for (const node<T>* child: composite_node<T>::children)
	{
		status child_status = child->execute(context);
		if (child_status != status::success)
			return child_status;
	}
	return status::success;
}

template <class T>
status selector<T>::execute(node<T>::context_type& context) const
{
	for (const node<T>* child: composite_node<T>::children)
	{
		status child_status = child->execute(context);
		if (child_status != status::failure)
			return child_status;
	}
	return status::failure;
}

} // namespace bt
} // namespace ai

#endif // ANTKEEPER_AI_BT_NODE_HPP
