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
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../game/colony.hpp"
#include "../game/ant.hpp"

PlayState::PlayState(Application* application):
	ApplicationState(application)
{}

PlayState::~PlayState()
{}

void PlayState::enter()
{
	// Hide level selector
	application->levelSelectorContainer->setVisible(false);
	application->levelSelectorContainer->setActive(false);
	
	// Setup HUD
	application->pauseButtonImage->setVisible(false);
	application->pauseButtonImage->setActive(false);
	application->playButtonImage->setVisible(false);
	application->playButtonImage->setActive(false);
	
	// Create terrain model instances
	application->terrain.getSurfaceModel()->getGroup(0)->material = application->materialLoader->load("data/materials/debug-terrain-surface.mtl");
	terrainSurface.setModel(application->terrain.getSurfaceModel());
	terrainSurface.setTranslation(Vector3(0, 0, 0));
	terrainSubsurface.setModel(application->terrain.getSubsurfaceModel());
	terrainSubsurface.setTranslation(Vector3(0, 0, 0));
	
	// Add terrain to scene
	application->defaultLayer->addObject(&terrainSurface);
	application->defaultLayer->addObject(&terrainSubsurface);
	
	// Spawn ants
	Navmesh* navmesh = application->terrain.getSurfaceNavmesh();
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
	application->surfaceCam->setFocalPoint(Vector3(0.0f));
	application->surfaceCam->setFocalDistance(250.0f);
	application->surfaceCam->setElevation(glm::radians(35.0f));
	application->surfaceCam->setAzimuth(glm::radians(-45.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	application->simulationPaused = false;
}

void PlayState::execute()
{
	// Update colony
	if (!application->simulationPaused)
	{
		application->colony->update(application->dt);
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
	float zoomFactor = application->surfaceCam->getFocalDistance() / 20.0f * application->dt / (1.0f / 60.0f);
	if (application->cameraZoomIn.isTriggered())
		application->surfaceCam->zoom(zoomFactor * application->cameraZoomIn.getCurrentValue());
	if (application->cameraZoomOut.isTriggered())
		application->surfaceCam->zoom(-zoomFactor * application->cameraZoomOut.getCurrentValue());
	application->surfaceCam->update(application->dt);
	
	// Update camera
	application->surfaceCam->update(application->dt);
	
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
}

void PlayState::mouseButtonPressed(int button, int x, int y)
{
}

void PlayState::mouseButtonReleased(int button, int x, int y)
{
}
