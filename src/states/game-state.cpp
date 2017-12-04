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

#include "game-state.hpp"
#include "title-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../game/colony.hpp"
#include "../game/ant.hpp"
#include "../game/tool.hpp"
#include "../game/pheromone-matrix.hpp"
#include "../ui/toolbar.hpp"
#include "../ui/menu.hpp"
#include "../ui/pie-menu.hpp"
#include <cmath>

inline void cmykToRGB(const float* cmyk, float* rgb)
{
	rgb[0] = -((cmyk[0] * (1.0f - cmyk[3])) + cmyk[3] - 1.0f);
	rgb[1] = -((cmyk[1] * (1.0f - cmyk[3])) + cmyk[3] - 1.0f);
	rgb[2] = -((cmyk[2] * (1.0f - cmyk[3])) + cmyk[3] - 1.0f);
}

GameState::GameState(Application* application):
	ApplicationState(application)
{}

GameState::~GameState()
{}

void GameState::enter()
{
	int continueWorld = -1;
	int continueLevel = -1;
	application->settings.get("continue_world", &continueWorld);
	application->settings.get("continue_level", &continueLevel);
	
	if (continueWorld != application->currentWorldIndex || continueLevel != application->currentLevelIndex)
	{
		// Save continue world and level indices
		application->settings.set("continue_world", application->currentWorldIndex);
		application->settings.set("continue_level", application->currentLevelIndex);
		application->saveUserSettings();
	}
	
	// Setup HUD
	application->rectangularPaletteImage->setVisible(true);
	application->rectangularPaletteImage->setActive(true);
	application->toolbar->getContainer()->setVisible(true);
	application->toolbar->getContainer()->setActive(true);
	
	Navmesh* navmesh = application->currentLevel->terrain.getSurfaceNavmesh();
	
	// Setup tools
	application->forceps->setColony(application->colony);
	application->forceps->setNavmesh(navmesh);
	
	// Add tools to scene
	application->defaultLayer->addObject(application->forceps->getModelInstance());
	application->defaultLayer->addObject(application->lens->getModelInstance());
	application->defaultLayer->addObject(application->lens->getSpotlight());
	application->defaultLayer->addObject(application->brush->getModelInstance());

	
	// Add terrain to scene
	application->defaultLayer->addObject(&application->currentLevel->terrainSurface);
	//application->defaultLayer->addObject(&application->currentLevel->terrainSubsurface);
	//application->defaultLayer->addObject(&application->biomeFloorModelInstance);
		
	// Spawn ants
	for (int i = 0; i < 200; ++i)
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
	
	// Position options menu
	application->optionsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.5f));
	application->controlsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.5f));
	application->levelsMenu->getUIContainer()->setAnchor(Vector2(0.5f, 0.5f));
	
	// Show level name
	application->levelNameLabel->setText(application->getLevelName(application->currentWorldIndex, application->currentLevelIndex));
	//application->levelNameLabel->setVisible(true);
	
	// Begin fade-in
	application->fadeInTween->start();
	
	application->mouse->addMouseButtonObserver(this);
}

void GameState::execute()
{
	// Pause simulation
	if (application->escape.isTriggered() && !application->escape.wasTriggered())
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
	
	// Navigate menu
	if (application->activeMenu != nullptr)
	{
		MenuItem* selectedItem = application->activeMenu->getSelectedItem();
		
		if (application->menuDown.isTriggered() && !application->menuDown.wasTriggered())
		{
			if (selectedItem != nullptr)
			{
				if (selectedItem->getItemIndex() < application->activeMenu->getItemCount() - 1)
				{
					application->selectMenuItem(selectedItem->getItemIndex() + 1);
				}
				else
				{
					application->selectMenuItem(0);
				}
			}
			else
			{
				application->selectMenuItem(0);
			}
		}
		else if (application->menuUp.isTriggered() && !application->menuUp.wasTriggered())
		{
			if (selectedItem != nullptr)
			{
				if (selectedItem->getItemIndex() > 0)
				{
					application->selectMenuItem(selectedItem->getItemIndex() - 1);
				}
				else
				{
					application->selectMenuItem(application->activeMenu->getItemCount() - 1);
				}
			}
			else
			{
				application->selectMenuItem(application->activeMenu->getItemCount() - 1);
			}
		}
		
		if (application->menuLeft.isTriggered() && !application->menuLeft.wasTriggered())
		{
			application->decrementMenuItem();
		}
		else if (application->menuRight.isTriggered() && !application->menuRight.wasTriggered())
		{
			application->incrementMenuItem();
		}
		
		if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
		{
			application->activateMenuItem();
		}
	} 
	else
	{
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
	}
	


	/*
	else
	{
		Plane plane;
		plane.set(Vector3(0, 1, 0), Vector3(0.0f));
		auto result = pickingRay.intersects(plane);
		pick = pickingRay.extrapolate(std::get<1>(result));
	}
	*/
	static bool bla = false;
	bla = !bla;
	// Update pheromone texture
	if (bla)
	{
		float cmyk[4];
		float rgb[3];
		const float* bufferH = application->colony->getHomingMatrix()->getActiveBuffer();
		const float* bufferR = application->colony->getRecruitmentMatrix()->getActiveBuffer();
		
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, application->pheromonePBO);
		GLubyte* data = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		GLubyte* channel = data;
		
		std::size_t index = 0;
		for (int y = 0; y < application->pheromoneTexture.getHeight(); ++y)
		{
			for (int x = 0; x < application->pheromoneTexture.getWidth(); ++x)
			{
				float concentrationH = std::min<float>(1.0f, bufferH[index]) * 0.35f;
				float concentrationR = std::min<float>(1.0f, bufferR[index]) * 0.35f;

				cmyk[0] = std::min<float>(1.0f, concentrationH * HOMING_PHEROMONE_COLOR[0] + concentrationR * RECRUITMENT_PHEROMONE_COLOR[0]);
				cmyk[1] = std::min<float>(1.0f, concentrationH * HOMING_PHEROMONE_COLOR[1] + concentrationR * RECRUITMENT_PHEROMONE_COLOR[1]);
				cmyk[2] = std::min<float>(1.0f, concentrationH * HOMING_PHEROMONE_COLOR[2] + concentrationR * RECRUITMENT_PHEROMONE_COLOR[2]);
				cmyk[3] = 0.35f;
				
				cmykToRGB(cmyk, rgb);
				
				GLubyte b = static_cast<GLubyte>(std::min<float>(255.0f, rgb[2] * 255.0f));
				GLubyte g = static_cast<GLubyte>(std::min<float>(255.0f, rgb[1] * 255.0f));
				GLubyte r = static_cast<GLubyte>(std::min<float>(255.0f, rgb[0] * 255.0f));
				
				*(channel++) = b;
				*(channel++) = g;
				*(channel++) = r;
				*(channel++) = 255;
				
				++index;
			}
		}
		
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		
		glBindTexture(GL_TEXTURE_2D, application->pheromoneTextureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, application->pheromoneTexture.getWidth(), application->pheromoneTexture.getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	
	application->colony->getHomingMatrix()->evaporate();
	application->colony->getRecruitmentMatrix()->evaporate();
	static int frame = 0;
	if (frame++ % DIFFUSION_FRAME == 0)
	{
		application->colony->getHomingMatrix()->diffuse();
		application->colony->getRecruitmentMatrix()->diffuse();
	}
	

	
	// Update camera
	application->surfaceCam->update(application->dt);
	
	// Picking
	if (!application->simulationPaused)
	{
		glm::ivec2 mousePosition = application->mouse->getCurrentPosition();
		mousePosition.y = application->resolution.y - mousePosition.y;
		Vector4 viewport(0.0f, 0.0f, application->resolution.x, application->resolution.y);
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
		
		application->colony->update(application->dt);
	}
}

void GameState::exit()
{
	// Remove input observers
	application->mouse->removeMouseButtonObserver(this);
	
	// Clear scene
	application->defaultLayer->removeObject(&application->currentLevel->terrainSurface);
	application->defaultLayer->removeObject(&application->currentLevel->terrainSubsurface);
	application->defaultLayer->removeObject(&application->biomeFloorModelInstance);
	application->defaultLayer->removeObject(application->forceps->getModelInstance());
	application->defaultLayer->removeObject(application->lens->getModelInstance());
	application->defaultLayer->removeObject(application->lens->getSpotlight());
	application->defaultLayer->removeObject(application->brush->getModelInstance());
	for (std::size_t i = 0; i < application->colony->getAntCount(); ++i)
	{
		Ant* ant = application->colony->getAnt(i);
		application->defaultLayer->removeObject(ant->getModelInstance());
	}
	
	// Kill all ants
	application->colony->killAll();
	
	// Hide HUD
	application->rectangularPaletteImage->setVisible(false);
	application->rectangularPaletteImage->setActive(false);
	application->toolbar->getContainer()->setVisible(false);
	application->toolbar->getContainer()->setActive(false);
}

void GameState::mouseButtonPressed(int button, int x, int y)
{
	if (button == 1)
	{
		if (application->forceps->isActive())
		{
			application->forceps->pinch();
		}
		else if (application->brush->isActive())
		{
			application->brush->press();
		}
		else if (application->lens->isActive())
		{
			application->lens->focus();
		}
	}
	
}

void GameState::mouseButtonReleased(int button, int x, int y)
{
	if (button == 1)
	{
		if (application->forceps->isActive())
		{
			application->forceps->release();
		}
		else if (application->brush->isActive())
		{
			application->brush->release();
		}
		else if (application->lens->isActive())
		{
			application->lens->unfocus();
		}
	}
}
