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

#ifndef ANTKEEPER_EVENT_SIGNAL_HPP
#define ANTKEEPER_EVENT_SIGNAL_HPP

#include <algorithm>
#include <execution>
#include <functional>
#include <list>
#include <memory>
#include <type_traits>

//namespace event {

template <class T>
class signal;

template <class T>
class connector;

/**
 * Manages a connection between a signal and handler. A signal will be disconnected from a handler when the connection is destructed or is disconnected manually via connection::disconnect().
 */
class connection
{
public:
	/// Signal handler disconnect function type.
	typedef std::function<void(std::weak_ptr<void>)> disconnector_type;
	
	/**
	 * Constructs a connection between a signal and a handler.
	 *
	 * @param handler Weak pointer to a signal handler.
	 * @param disconnector Signal handler disconnect function.
	 */
	connection(std::weak_ptr<void> handler, disconnector_type disconnector);
	
	/**
	 * Destructs a connection between a signal and a handler.
	 */
	~connection();
	
	/**
	 * Returns `true` if the signal and handler are connected, `false` otherwise.
	 */
	bool connected() const noexcept;
	
	/**
	 * Disconnects the signal from the handler.
	 */
	void disconnect();
	
private:
	template <class T>
	friend class signal;
	
	std::weak_ptr<void> handler;
	disconnector_type disconnector;
};

/**
 * Creates connections between a signal and signal handlers.
 *
 * @tparam T Signal response type.
 * @tparam Args Signal argument types.
 */
template <class T, class... Args>
class connector<T(Args...)>
{
public:
	/// Signal response type.
	typedef T response_type;
	
	/// Signal type.
	typedef signal<T(Args...)> signal_type;
	
	/**
	 * Constructs a signal connector.
	 *
	 * @param signal Signal to which handlers may be connected.
	 */
	connector(signal_type& signal):
		signal(&signal)
	{}
	
	/// @copydoc signal::connect(handler_type)
	std::shared_ptr<connection> connect(typename signal_type::handler_type handler)
	{
		return signal->connect(handler);
	}
	
private:
	signal_type* signal;
};

/**
 * Emits signals to signal handlers.
 *
 * @tparam T Signal response type.
 * @tparam Args Signal argument types.
 */
template <class T, class... Args>
class signal<T(Args...)>
{
public:
	/// Signal response type.
	typedef T response_type;
	
	/// Signal handler type.
	typedef std::function<T(Args...)> handler_type;
	
	/// Signal connector type.
	typedef connector<T(Args...)> connector_type;
	
	/**
	 * Constructs a signal.
	 */
	signal():
		signal_connector(*this)
	{}
	
	/**
	 * Returns the connector for this signal.
	 */
	connector_type& connector() noexcept
	{
		return signal_connector;
	}
	
	/**
	 * Connects the signal to a handler.
	 *
	 * @param handler Signal handler to connect.
	 *
	 * @return Connection between the signal and handler.
	 */
	std::shared_ptr<connection> connect(handler_type handler)
	{
		// Allocate shared pointer to handler
		std::shared_ptr<handler_type> shared_handler = std::make_shared<handler_type>(handler);
		
		// Add handler to list of connected handlers
		connections.push_back(shared_handler);
		
		// Return a shared pointer to the connection between the signal and handler
		return std::make_shared<connection>
		(
			std::static_pointer_cast<void>(shared_handler),
			[this](std::weak_ptr<void> handler)
			{
				this->connections.remove(std::static_pointer_cast<handler_type>(handler.lock()));
			}
		);
	}
	
	/**
	 * Disconnects the signal from all connected handlers.
	 */
	void disconnect()
	{
		connections.clear();
	}
	
	/**
	 * Emits a signal to all connected handlers.
	 *
	 * @tparam ExecutionPolicy Execution policy type.
	 *
	 * @param policy Execution policy to use.
	 * @param args Signal arguments.
	 */
	/// @{
	template <class ExecutionPolicy>
	void emit(ExecutionPolicy&& policy, Args... args) const
	{
		std::for_each
		(
			policy,
			std::begin(connections),
			std::end(connections),
			[&](const auto& handler)
			{
				(*handler)(args...);
			}
		);
	}
	
	void emit(Args... args) const
	{
		emit(std::execution::seq, args...);
	}
	/// @}
	
	/**
	 * Emits a signal to all connected handlers and relays their responses to a listener.
	 *
	 * @tparam ExecutionPolicy Execution policy type.
	 * @tparam UnaryFunction Listener function object type.
	 *
	 * @param policy Execution policy to use.
	 * @param listener Listener function object.
	 * @param args Signal arguments.
	 */
	/// @{
	template <class ExecutionPolicy, class UnaryFunction>
	void ping(ExecutionPolicy&& policy, UnaryFunction listener, Args... args) const
	{
		std::for_each
		(
			policy,
			std::begin(connections),
			std::end(connections),
			[&](const auto& handler)
			{
				if constexpr(std::is_void_v<T>)
				{
					(*handler)(args...);
					listener();
				}
				else
				{
					listener((*handler)(args...));
				}
			}
		);
	}
	
	template <class UnaryFunction>
	void ping(UnaryFunction listener, Args... args) const
	{
		ping(std::execution::seq, listener, args...);
	}
	/// @}
	
private:
	/// List of connected signal handlers.
	std::list<std::shared_ptr<handler_type>> connections;
	
	/// Signal connector.
	connector_type signal_connector;
};

//} // namespace event

#endif // ANTKEEPER_EVENT_SIGNAL_HPP
