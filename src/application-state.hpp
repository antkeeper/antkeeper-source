/*
 * Copyright (C) 2017  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef APPLICATION_STATE_HPP
#define APPLICATION_STATE_HPP

class Application;

/**
 * Abstract base class for application states.
 */
class ApplicationState
{
public:
	ApplicationState(Application* application);
	
	virtual ~ApplicationState();
	
	// Run once when the state is initially entered
	virtual void enter() = 0;
	
	// Run continually while the state is valid
	virtual void execute() = 0;
	
	// Run once when the state is exited
	virtual void exit() = 0;

protected:
	Application* application;
};

#endif // APPLICATION_STATE_HPP