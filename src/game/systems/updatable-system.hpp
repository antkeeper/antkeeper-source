// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_UPDATABLE_SYSTEM_HPP
#define ANTKEEPER_GAME_UPDATABLE_SYSTEM_HPP

#include <engine/entity/registry.hpp>


/**
 * Abstract base class for updatable systems.
 */
class updatable_system
{
public:
	/**
	 * Creates an updatable system.
	 *
	 * @param registry Reference to the registry on which the system will operate.
	 */
	explicit updatable_system(entity::registry& registry);
	
	/** Destructs an updatable system. */
	virtual ~updatable_system() = default;

	/**
	 * Perform's a system's update() function.
	 *
	 * @param t Total elapsed time, in seconds.
	 * @param dt Delta time, in seconds.
	 */
	virtual void update(float t, float dt) = 0;
	
protected:
	/** Registry on which the system operates. */
	entity::registry& m_registry;
};


#endif // ANTKEEPER_GAME_UPDATABLE_SYSTEM_HPP
