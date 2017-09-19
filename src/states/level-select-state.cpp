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

#include "level-select-state.hpp"
#include "main-menu-state.hpp"
#include "../application.hpp"
#include "../configuration.hpp"

LevelSelectState::LevelSelectState(Application* application):
	ApplicationState(application)
{}

LevelSelectState::~LevelSelectState()
{}

void LevelSelectState::enter()
{
	levelRotation = 0.0f;
	for (int i = 0; i < 5; ++i)
	{
		ModelInstance* surfaceInstance = &application->previewLevelSurfaces[i];
		ModelInstance* subsurfaceInstance = &application->previewLevelSubsurfaces[i];
		
		Quaternion rotation = glm::angleAxis(levelRotation, Vector3(0, 1, 0));
		surfaceInstance->setRotation(rotation);
		subsurfaceInstance->setRotation(rotation);
		
		application->defaultLayer->addObject(surfaceInstance);
		application->defaultLayer->addObject(subsurfaceInstance);
	}
	application->defaultLayer->addObject(&application->biomeFloorModelInstance);
	//application->biomeFloorModelInstance.setTranslation(Vector3(0.0f, -ANTKEEPER_TERRAIN_BASE_HEIGHT, 0.0f));
	
	application->levelIDLabel->setVisible(true);
	application->levelNameLabel->setVisible(true);
	
	application->selectWorld(0);
	application->selectLevel(0);
	
	application->camera.lookAt(Vector3(0, 150, 200), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void LevelSelectState::execute()
{
	// Navigate menu
	if (application->menuLeft.isTriggered() && !application->menuLeft.wasTriggered())
	{
		application->selectPreviousLevel();
	}
	else if (application->menuRight.isTriggered() && !application->menuRight.wasTriggered())
	{
		application->selectNextLevel();
	}
	
	if (application->menuDown.isTriggered() && !application->menuDown.wasTriggered())
	{
		application->selectPreviousWorld();
	}
	else if (application->menuUp.isTriggered() && !application->menuUp.wasTriggered())
	{
		application->selectNextWorld();
	}
	
	if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
	{
		application->enterSelectedLevel();
	}
	else if (application->menuCancel.isTriggered() && !application->menuCancel.wasTriggered())
	{
		application->changeState(application->mainMenuState);
	}
	
	// Rotate levels
	levelRotation += glm::radians(5.0f) * application->dt;
	for (int i = 0; i < 5; ++i)
	{
		ModelInstance* surfaceInstance = &application->previewLevelSurfaces[i];
		ModelInstance* subsurfaceInstance = &application->previewLevelSubsurfaces[i];
		
		Quaternion rotation = glm::angleAxis(levelRotation, Vector3(0, 1, 0));
		//surfaceInstance->setRotation(rotation);
		//subsurfaceInstance->setRotation(rotation);
	}
}

void LevelSelectState::exit()
{
	for (int i = 0; i < 5; ++i)
	{
		ModelInstance* surfaceInstance = &application->previewLevelSurfaces[i];
		ModelInstance* subsurfaceInstance = &application->previewLevelSubsurfaces[i];
		
		application->defaultLayer->removeObject(surfaceInstance);
		application->defaultLayer->removeObject(subsurfaceInstance);
	}
	application->defaultLayer->removeObject(&application->biomeFloorModelInstance);
	
	application->levelIDLabel->setVisible(false);
	application->levelNameLabel->setVisible(false);
}
