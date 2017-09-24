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

#include "loading-state.hpp"
#include "../application.hpp"
#include "splash-state.hpp"
#include "title-state.hpp"
#include "main-menu-state.hpp"

LoadingState::LoadingState(Application* application):
	ApplicationState(application)
{}

LoadingState::~LoadingState()
{}

void LoadingState::enter()
{		
	bool failure = false;
	
	std::cout << "Loading controls... ";
	if (!application->loadControls())
	{
		std::cout << "failed" << std::endl;
		failure = true;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	std::cout << "Loading scene... ";
	if (!application->loadScene())
	{
		std::cout << "failed" << std::endl;
		failure = true;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	std::cout << "Loading models... ";
	if (!application->loadModels())
	{
		std::cout << "failed" << std::endl;
		failure = true;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	std::cout << "Loading game... ";
	if (!application->loadGame())
	{
		std::cout << "failed" << std::endl;
		failure = true;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	std::cout << "Loading UI... ";
	if (!application->loadUI())
	{
		std::cout << "failed" << std::endl;
		failure = true;
	}
	else
	{
		std::cout << "success" << std::endl;
	}
	
	if (failure)
	{
		application->close(EXIT_FAILURE);
	}
}

void LoadingState::execute()
{
	// Check for splash screen and title skip settings
	bool skipSplash = false;
	bool skipTitle = false;
	application->settings.get("skip_splash", &skipSplash);
	application->settings.get("skip_title", &skipTitle);
	
	// Determine next state
	ApplicationState* nextState = application->splashState;
	if (skipSplash)
	{
		nextState = application->titleState;
		
		if (skipTitle)
		{
			nextState = application->mainMenuState;
		}
	}
	
	// Change state
	application->changeState(nextState);
}

void LoadingState::exit()
{}
