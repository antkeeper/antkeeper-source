// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_BT_NODE_HPP
#define ANTKEEPER_AI_BT_NODE_HPP

#include <engine/ai/bt/status.hpp>

namespace ai {
namespace bt {

/// Abstract base class for behavior tree nodes.
/// @tparam T Data type on which nodes operate.
template <class T>
struct node
{
	/// Data type on which nodes operate.
	using context_type = T;

	/// Executes a node's function and returns its status.
	/// @param context Context data on which the node will operate.
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
	using function_type = std::function<status(node<T>::context_type&)>;
	function_type function;
};

/// Evaluates a boolean condition (predicate) and returns either `status::success` or `status::failure`.
template <class T>
struct condition: public leaf_node<T>
{
	virtual status execute(node<T>::context_type& context) const final;
	using predicate_type = std::function<status(const node<T>::context_type&)>;
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
