/*
 * Copyright (C) 2017-2019  Christopher J. Howard
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

#include "game.hpp"
#include "ui/ui.hpp"
#include "menu.hpp"
#include "debug/command-interpreter.hpp"
#include "game/camera-rig.hpp"

void Game::enterTitleState()
{
	// Setup scene
	Vector3 antHillTranslation = {0, 0, 0};
	EntityID antHill = createInstanceOf("ant-hill");
	setTranslation(antHill, antHillTranslation);

	// Setup camera
	cameraRig = orbitCam;
	orbitCam->setTargetFocalPoint(antHillTranslation);
	orbitCam->setTargetFocalDistance(0.0f);
	orbitCam->setTargetElevation(glm::radians(80.0f));
	orbitCam->setTargetAzimuth(0.0f);
	orbitCam->setFocalPoint(orbitCam->getTargetFocalPoint());
	orbitCam->setFocalDistance(orbitCam->getTargetFocalDistance());
	orbitCam->setElevation(orbitCam->getTargetElevation());
	orbitCam->setAzimuth(orbitCam->getTargetAzimuth());

	float fov = glm::radians(30.0f);
	orbitCam->getCamera()->setPerspective(fov, (float)w / (float)h, 1.0f, 1000.0f);

	// Begin fade-in
	fadeIn(6.0f, {0, 0, 0}, nullptr);

	//
	animator.addAnimation(&antHillZoomAnimation);
	antHillZoomAnimation.rewind();
	antHillZoomAnimation.play();	

	menuFadeAnimation.rewind();
	menuFadeAnimation.play();
	menuFadeAnimation.setEndCallback(nullptr);

	// Disable non-system controls
	disableNonSystemControls();

	// Enable menu controls
	menuControls.setCallbacksEnabled(true);
	
	// Enable UI callbacks
	uiRootElement->setCallbacksEnabled(true);

	// Change setting menu's back item to return to the main menu
	settingsMenuBackItem->setActivatedCallback(std::bind(&Game::openMenu, this, mainMenu, 3));

	#if defined(DEBUG)
		// Add important entity IDs to CLI variables
		cli->set("anthill", std::to_string(antHill));
	#endif

	// Open the main menu and select the first menu item
	openMenu(mainMenu, 0);
}

void Game::exitTitleState()
{
	animator.removeAnimation(&antHillZoomAnimation);
}

