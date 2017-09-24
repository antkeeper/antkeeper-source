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

#include "play-state.hpp"
#include "level-select-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../game/colony.hpp"
#include "../game/ant.hpp"
#include "../game/tool.hpp"
#include "../ui/toolbar.hpp"
#include "../ui/pie-menu.hpp"
#include <cmath>

PlayState::PlayState(Application* application):
	ApplicationState(application)
{}

PlayState::~PlayState()
{}

void PlayState::enter()
{
	// Setup HUD
	application->pauseButtonImage->setVisible(false);
	application->pauseButtonImage->setActive(false);
	application->playButtonImage->setVisible(false);
	application->playButtonImage->setActive(false);
	application->rectangularPaletteImage->setVisible(true);
	application->rectangularPaletteImage->setActive(true);
	application->toolbar->getContainer()->setVisible(true);
	application->toolbar->getContainer()->setActive(true);
	
	// Setup tools
	application->forceps->setColony(application->colony);
	
	// Add tools to scene
	application->defaultLayer->addObject(application->forceps->getModelInstance());
	application->defaultLayer->addObject(application->lens->getModelInstance());
	application->defaultLayer->addObject(application->brush->getModelInstance());

	
	// Add terrain to scene
	application->defaultLayer->addObject(&application->currentLevel->terrainSurface);
	application->defaultLayer->addObject(&application->currentLevel->terrainSubsurface);
	application->defaultLayer->addObject(&application->biomeFloorModelInstance);
	

	
	// Spawn ants
	Navmesh* navmesh = application->currentLevel->terrain.getSurfaceNavmesh();
	for (int i = 0; i < 50; ++i)
	{
		Navmesh::Triangle* triangle = (*navmesh->getTriangles())[0];
		
		Ant* ant = application->colony->spawn(navmesh, triangle, normalize_barycentric(Vector3(0.5f)));
		
		Vector3 forward = glm::normalize(triangle->edge->vertex->position - triangle->edge->next->vertex->position);
		Vector3 up = triangle->normal;
		ant->setOrientation(forward, up);
		
		application->defaultLayer->addObject(ant->getModelInstance());
		ant->setState(Ant::State::WANDER);
	}
	
	// Setup camera controller
	application->surfaceCam->setCamera(&application->camera);
	//application->surfaceCam->setFocalPoint(Vector3(0.0f));
	//application->surfaceCam->setFocalDistance(250.0f);
	//application->surfaceCam->setElevation(glm::radians(35.0f));
	//application->surfaceCam->setAzimuth(glm::radians(-45.0f));
	application->surfaceCam->setTargetFocalPoint(Vector3(0.0f));
	application->surfaceCam->setTargetFocalDistance(250.0f);
	application->surfaceCam->setTargetElevation(glm::radians(35.0f));
	//application->surfaceCam->setTargetAzimuth(glm::radians(-45.0f));
	application->surfaceCam->update(0.0f);
	
	application->simulationPaused = false;
	
	// Select forceps tool
	//application->deselectTool(application->currentTool);
	//application->selectTool(application->forceps);
	application->pieMenu->select(1);
	
	application->mouse->addMouseButtonObserver(this);
}

void PlayState::execute()
{

	


	/*
	else
	{
		Plane plane;
		plane.set(Vector3(0, 1, 0), Vector3(0.0f));
		auto result = pickingRay.intersects(plane);
		pick = pickingRay.extrapolate(std::get<1>(result));
	}
	*/
	
	static float rotationTime = 0.0f;
	float iconRotation = std::sin(rotationTime * 3.0f) * glm::radians(10.0f);
	rotationTime += application->dt;
	
	
	//application->blaImage->setRotation(iconRotation);
	
	// Return to level select
	if (application->menuCancel.isTriggered() && !application->menuCancel.wasTriggered())
	{
		application->changeState(application->levelSelectState);
	}
	
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
		application->surfaceCam->move(movementVector);
		
		Vector3 focal = application->surfaceCam->getFocalPoint();
	}
	
	// Zoom camera
	float zoomFactor = application->surfaceCam->getFocalDistance() / 10.0f * application->dt / (1.0f / 60.0f);
	if (application->cameraZoomIn.isTriggered())
		application->surfaceCam->zoom(zoomFactor * application->cameraZoomIn.getCurrentValue());
	if (application->cameraZoomOut.isTriggered())
		application->surfaceCam->zoom(-zoomFactor * application->cameraZoomOut.getCurrentValue());
	
	// Rotate camera
	if (application->cameraRotateCW.isTriggered() && !application->cameraRotateCW.wasTriggered())
	{
		application->surfaceCam->rotate(glm::radians(-45.0f));
	}
	if (application->cameraRotateCCW.isTriggered() && !application->cameraRotateCCW.wasTriggered())
	{
		application->surfaceCam->rotate(glm::radians(45.0f));
	}
	
	// Update camera
	application->surfaceCam->update(application->dt);
	
	// Picking
	glm::ivec2 mousePosition = application->mouse->getCurrentPosition();
	mousePosition.y = application->height - mousePosition.y;
	Vector4 viewport(0.0f, 0.0f, application->width, application->height);
	Vector3 mouseNear = application->camera.unproject(Vector3(mousePosition.x, mousePosition.y, 0.0f), viewport);
	Vector3 mouseFar = application->camera.unproject(Vector3(mousePosition.x, mousePosition.y, 1.0f), viewport);
	
	pickingRay.origin = mouseNear;
	pickingRay.direction = glm::normalize(mouseFar - mouseNear);
	
	std::list<Navmesh::Triangle*> triangles;
	application->currentLevel->terrain.getSurfaceOctree()->query(pickingRay, &triangles);
	
	auto result = intersects(pickingRay, triangles);
	if (std::get<0>(result))
	{
		pick = pickingRay.extrapolate(std::get<1>(result));
		
		std::size_t triangleIndex = std::get<3>(result);
		pickTriangle = (*application->currentLevel->terrain.getSurfaceNavmesh()->getTriangles())[triangleIndex];
		
		/*
		float forcepsDistance = application->forcepsSwoopTween->getTweenValue();
		
		//Quaternion rotation = glm::rotation(Vector3(0, 1, 0), triangle->normal);
		Quaternion rotation = glm::angleAxis(application->surfaceCam->getAzimuth(), Vector3(0, 1, 0)) *
			glm::angleAxis(glm::radians(15.0f), Vector3(0, 0, -1));
		
		Vector3 translation = pick + rotation * Vector3(0, forcepsDistance, 0);
		
		// Set tool position
		application->forcepsModelInstance.setTranslation(translation);
		application->forcepsModelInstance.setRotation(rotation);
		*/
	}
	
	// Update tools
	if (application->currentTool != nullptr)
	{
		application->currentTool->setPick(pick);
		application->currentTool->update(application->dt);
	}
	
	// Update colony
	if (!application->simulationPaused)
	{
		application->colony->update(application->dt);
	}
	
	// Pause simulation
	if (application->togglePause.isTriggered() && !application->togglePause.wasTriggered())
	{
		if (application->simulationPaused)
		{
			application->unpauseSimulation();
		}
		else
		{
			application->pauseSimulation();
		}		
	}
}

void PlayState::exit()
{
	// Remove input observers
	application->mouse->removeMouseButtonObserver(this);
	
	// Clear scene
	application->defaultLayer->removeObject(&application->currentLevel->terrainSurface);
	application->defaultLayer->removeObject(&application->currentLevel->terrainSubsurface);
	application->defaultLayer->removeObject(&application->biomeFloorModelInstance);
	application->defaultLayer->removeObject(application->forceps->getModelInstance());
	application->defaultLayer->removeObject(application->lens->getModelInstance());
	for (std::size_t i = 0; i < application->colony->getAntCount(); ++i)
	{
		Ant* ant = application->colony->getAnt(i);
		application->defaultLayer->removeObject(ant->getModelInstance());
	}
	
	// Kill all ants
	application->colony->killAll();
	
	// Hide HUD
	application->pauseButtonImage->setVisible(false);
	application->pauseButtonImage->setActive(false);
	application->playButtonImage->setVisible(false);
	application->playButtonImage->setActive(false);
	application->rectangularPaletteImage->setVisible(false);
	application->rectangularPaletteImage->setActive(false);
	application->toolbar->getContainer()->setVisible(false);
	application->toolbar->getContainer()->setActive(false);
}

void PlayState::mouseButtonPressed(int button, int x, int y)
{
	if (button == 1 && application->forceps->isActive())
	{
		application->forceps->pinch();
	}
}

void PlayState::mouseButtonReleased(int button, int x, int y)
{
	if (button == 1 && application->forceps->isActive())
	{
		application->forceps->release();
	}
}
