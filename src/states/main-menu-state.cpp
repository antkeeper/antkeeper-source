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

#include "main-menu-state.hpp"
#include "../application.hpp"
#include "../debug.hpp"
#include "../camera-controller.hpp"

void drawShaft(LineBatcher* lineBatcher, const Shaft* shaft);

void drawChamber(LineBatcher* lineBatcher, const Chamber* chamber)
{
	float helixAngle = chamber->parent->getHelixAngle(chamber->relativeDepth);
	float minAngle = helixAngle - chamber->centralAngle * 0.5f;
	float maxAngle = helixAngle + chamber->centralAngle * 0.5f;
		
	// Find position on helix 
	Vector3 helixPosition = chamber->parent->getHelixPosition(chamber->relativeDepth);
	helixPosition.y = -helixPosition.y;
	
	// Move annulus toward helix by the inner radius
	Vector3 helixDirection = glm::normalize(Vector3(std::cos(helixAngle), 0.0f, std::sin(helixAngle)));
	Vector3 offset = helixPosition - helixDirection * (chamber->innerRadius - chamber->parent->shaftRadius);
		
	int stepCount = 10;
	float angleStep = chamber->centralAngle / (float)stepCount;
	for (int i = 0; i < stepCount; ++i)
	{
		float angle0 = minAngle + angleStep * (float)i;
		float angle1 = minAngle + angleStep * (float)(i + 1);
		
		float x0 = std::cos(angle0);
		float z0 = std::sin(angle0);
		float x1 = std::cos(angle1);
		float z1 = std::sin(angle1);
		
		Vector3 innerStart;
		innerStart.x = x0 * chamber->innerRadius;
		innerStart.y = 0.0f;
		innerStart.z = z0 * chamber->innerRadius;
		
		Vector3 outerStart;
		outerStart.x = x0 * chamber->outerRadius;
		outerStart.y = 0.0f;
		outerStart.z = z0 * chamber->outerRadius;
		
		Vector3 innerEnd;
		innerEnd.x = x1 * chamber->innerRadius;
		innerEnd.y = 0.0f;
		innerEnd.z = z1 * chamber->innerRadius;
		
		Vector3 outerEnd;
		outerEnd.x = x1 * chamber->outerRadius;
		outerEnd.y = 0.0f;
		outerEnd.z = z1 * chamber->outerRadius;
		
		lineBatcher->draw(offset + innerStart, offset + innerEnd);
		lineBatcher->draw(offset + outerStart, offset + outerEnd);
	}
	
	Vector3 leftWallStart;
	leftWallStart.x = std::cos(minAngle) * chamber->innerRadius;
	leftWallStart.y = 0.0f;
	leftWallStart.z = std::sin(minAngle) * chamber->innerRadius;
	
	Vector3 leftWallEnd;
	leftWallEnd.x = std::cos(minAngle) * chamber->outerRadius;
	leftWallEnd.y = 0.0f;
	leftWallEnd.z = std::sin(minAngle) * chamber->outerRadius;
	
	Vector3 rightWallStart;
	rightWallStart.x = std::cos(maxAngle) * chamber->innerRadius;
	rightWallStart.y = 0.0f;
	rightWallStart.z = std::sin(maxAngle) * chamber->innerRadius;
	
	Vector3 rightWallEnd;
	rightWallEnd.x = std::cos(maxAngle) * chamber->outerRadius;
	rightWallEnd.y = 0.0f;
	rightWallEnd.z = std::sin(maxAngle) * chamber->outerRadius;
	
	lineBatcher->draw(offset + leftWallStart, offset + leftWallEnd);
	lineBatcher->draw(offset + rightWallStart, offset + rightWallEnd);
	
	if (chamber->child != nullptr)
	{
		drawShaft(lineBatcher, chamber->child);
	}
}

void drawShaft(LineBatcher* lineBatcher, const Shaft* shaft)
{
	// Draw helix
	int stepCount = 50;
	float depthStep = shaft->shaftDepth / (float)stepCount;
	for (int i = 0; i < stepCount; ++i)
	{
		Vector3 start = shaft->getHelixPosition((float)i * depthStep);
		Vector3 end = shaft->getHelixPosition((float)(i + 1) * depthStep);
		
		start.y = -start.y;
		end.y = -end.y;
		
		lineBatcher->draw(start, end);
	}
	
	// Draw children
	for (const Chamber* chamber: shaft->children)
	{
		drawChamber(lineBatcher, chamber);
	}
}

void MainMenuState::generateNest()
{
	NestParameters params;
	
	params.randomSeed = std::rand();
	
	params.maxShaftGeneration = 2;
	params.minShaftRadius = 0.0f;
	params.maxShaftRadius = 0.0f;
	params.minShaftDepth = 4.0f;
	params.maxShaftDepth = 6.0f;
	params.minShaftHelixRadius = 0.1f;
	params.maxShaftHelixRadius = 1.0f;
	params.minShaftHelixPitch = 0.25f;
	params.maxShaftHelixPitch = 0.75f;
	params.minShaftChamberCount = 1;
	params.maxShaftChamberCount = 5;
	params.minShaftChamberPitch = 0.5f;
	params.maxShaftChamberPitch = 2.0f;
	
	params.minChamberInnerRadius = 0.2f;
	params.maxChamberInnerRadius = 0.2f;
	params.minChamberOuterRadius = 0.5f;
	params.maxChamberOuterRadius = 0.5f;
	params.minChamberCentralAngle = glm::radians(240.0f);
	params.maxChamberCentralAngle = glm::radians(240.0f);
	
	nest.setParameters(params);
	nest.generate();
	
	// Draw nest
	application->lineBatcher->setColor(Vector4(1.0f));
	application->lineBatcher->setWidth(0.015f);
	application->lineBatcher->begin();
	drawShaft(application->lineBatcher, nest.getRootShaft());
	application->lineBatcher->end();
}

MainMenuState::MainMenuState(Application* application):
	ApplicationState(application)
{}

MainMenuState::~MainMenuState()
{}

void MainMenuState::enter()
{
	// Open main menu
	application->enterMenu(0);					
	application->menuSelectorLabel->setVisible(true);
	
	// Start fade-in
	application->blackoutImage->setVisible(true);
	application->fadeInTween->start();
	
	// Add nest
	application->defaultLayer->addObject(&application->nestModelInstance);
	
	application->surfaceCam->setCamera(&application->camera);
	application->surfaceCam->setFocalPoint(Vector3(-10.0f, -13.3f, 0.0f));
	application->surfaceCam->setFocalDistance(89.5f);
	application->surfaceCam->setElevation(glm::radians(15.0f));
	application->surfaceCam->setAzimuth(glm::radians(0.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	// 3D camera
	application->camera.setPerspective(
		glm::radians(25.0f),
		(float)application->width / (float)application->height,
		0.1f,
		1000.0f);
}

void MainMenuState::execute()
{
	// Navigate menu
	if (application->menuDown.isTriggered() && !application->menuDown.wasTriggered())
	{
		if (application->selectedMenuItemIndex < application->currentMenu->getItemCount() - 1)
		{
			application->selectMenuItem(application->selectedMenuItemIndex + 1);
		}
		else
		{
			application->selectMenuItem(0);
		}
	}
	else if (application->menuUp.isTriggered() && !application->menuUp.wasTriggered())
	{
		if (application->selectedMenuItemIndex > 0)
		{
			application->selectMenuItem(application->selectedMenuItemIndex - 1);
		}
		else
		{
			application->selectMenuItem(application->currentMenu->getItemCount() - 1);
		}
	}
	
	if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
	{
		application->activateMenuItem(application->selectedMenuItemIndex);
	}
	else if (application->menuCancel.isTriggered() && !application->menuCancel.wasTriggered())
	{
		
	}
	
	float lineHeight = application->menuFont->getMetrics().getHeight();
	const UIContainer* container = application->menuContainers[application->currentMenuIndex];
	application->menuSelectorLabel->setTranslation(
		Vector2(container->getPosition().x - application->menuSelectorLabel->getDimensions().x * 1.5f,
			container->getPosition().y + lineHeight * 0.5f - application->menuSelectorLabel->getDimensions().y * 0.5f + lineHeight * application->selectedMenuItemIndex));
	
	// Move camera
	Vector2 movementVector(0.0f);
	if (application->cameraMoveLeft.isTriggered())
		movementVector.x -= application->cameraMoveLeft.getCurrentValue();
	if (application->cameraMoveRight.isTriggered())
		movementVector.x += application->cameraMoveRight.getCurrentValue();
	if (application->cameraMoveForward.isTriggered())
		movementVector.y -= application->cameraMoveForward.getCurrentValue();
	if (application->cameraMoveBack.isTriggered())
		movementVector.y += application->cameraMoveBack.getCurrentValue();
	if (movementVector.x != 0.0f || movementVector.y != 0.0f)
	{
		movementVector *= 0.005f * application->surfaceCam->getFocalDistance() * application->dt / (1.0f / 60.0f);
		application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getTargetFocalPoint() + Vector3(movementVector.x, -movementVector.y, 0.0f));
		
		Vector3 focal = application->surfaceCam->getFocalPoint();
		std::cout << focal.x << ", " << focal.y << ", " << focal.z << "; " << application->surfaceCam->getFocalDistance() << std::endl;
	}
	
	// Zoom camera
	float zoomFactor = application->surfaceCam->getFocalDistance() / 20.0f * application->dt / (1.0f / 60.0f);
	if (application->cameraZoomIn.isTriggered())
		application->surfaceCam->zoom(zoomFactor * application->cameraZoomIn.getCurrentValue());
	if (application->cameraZoomOut.isTriggered())
		application->surfaceCam->zoom(-zoomFactor * application->cameraZoomOut.getCurrentValue());
	application->surfaceCam->update(application->dt);
	
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getTargetAzimuth() + glm::radians(2.0f) * application->dt);
	application->surfaceCam->update(application->dt);
}

void MainMenuState::exit()
{
	// Remove nest
	application->defaultLayer->removeObject(&application->nestModelInstance);
}

void MainMenuState::mouseButtonPressed(int button, int x, int y)
{
}

void MainMenuState::mouseButtonReleased(int button, int x, int y)
{
}
