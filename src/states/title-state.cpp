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

#include "title-state.hpp"
#include "experiment-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include <iostream>
#include <SDL.h>

const float blankDuration = 0.0f;
const float fadeInDuration = 0.5f;
const float hangDuration = 1.0f;
const float fadeOutDuration = 0.5f;
const float titleDelay = 2.0f;
const float copyrightDelay = 3.0f;
const float pressAnyKeyDelay = 5.0f;

TitleState::TitleState(Application* application):
	ApplicationState(application)
{}

TitleState::~TitleState()
{}

void TitleState::enter()
{
	std::cout << "Entering TitleState..." << std::endl;
	
	// Setup screen fade-in transition
	fadeIn = false;
	fadeOut = false;
	
	application->antModelInstance->setModel(application->antModel);
	application->antModelInstance->setTransform(Transform::getIdentity());
	
	// BG
	application->bgBatch.resize(1);
	BillboardBatch::Range* bgRange = application->bgBatch.addRange();
	bgRange->start = 0;
	bgRange->length = 1;
	Billboard* bgBillboard = application->bgBatch.getBillboard(0);
	bgBillboard->setDimensions(Vector2(1.0f, 1.0f));
	bgBillboard->setTranslation(Vector3(0.5f, 0.5f, 0.0f));
	bgBillboard->setTintColor(Vector4(1, 1, 1, 1));
	application->bgBatch.update();
	
	application->vignettePass.setRenderTarget(&application->defaultRenderTarget);
	application->bgCompositor.addPass(&application->vignettePass);
	application->bgCompositor.load(nullptr);
	application->bgCamera.setOrthographic(0, 1.0f, 1.0f, 0, -1.0f, 1.0f);
	application->bgCamera.lookAt(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	application->bgCamera.setCompositor(&application->bgCompositor);
	application->bgCamera.setCompositeIndex(0);
	
	application->bgScene.addLayer();
	application->bgScene.getLayer(0)->addObject(&application->bgCamera);
	application->bgScene.getLayer(0)->addObject(&application->bgBatch);
	
	// Setup lighting
	application->sunlight.setColor(glm::vec3(1.0f));
	application->sunlight.setDirection(glm::normalize(glm::vec3(0.5, -1, -0.5)));
	
	// Setup soil pass
	application->soilPass.setRenderTarget(&application->defaultRenderTarget);
	application->defaultCompositor.addPass(&application->soilPass);
	
	// Create terrain
	application->terrain.create(255, 255, Vector3(50, 20, 50));
	application->terrain.getSurfaceModel()->getGroup(0)->material = application->materialLoader->load("data/materials/debug-terrain-surface.mtl");
	terrainSurface.setModel(application->terrain.getSurfaceModel());
	terrainSurface.setTranslation(Vector3(0, 0, 0));
	terrainSubsurface.setModel(application->terrain.getSubsurfaceModel());
	terrainSubsurface.setTranslation(Vector3(0, 0, 0));
	application->scene.getLayer(0)->addObject(&terrainSurface);
	application->scene.getLayer(0)->addObject(&terrainSubsurface);
	navmesh = application->terrain.getSurfaceNavmesh();
	
	// Load level
	application->loadLevel();
	
	// Setup lighting pass
	application->lightingPass.setRenderTarget(&application->defaultRenderTarget);
	application->lightingPass.setShadowMap(0);
	application->lightingPass.setShadowCamera(&application->camera);
	application->lightingPass.setModelLoader(application->modelLoader);
	application->defaultCompositor.addPass(&application->lightingPass);
	
	application->camera.lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	
	application->camera.setCompositor(&application->defaultCompositor);
	application->camera.setCompositeIndex(0);
	
	// Setup scene
	DirectionalLight* lightA = new DirectionalLight();
	DirectionalLight* lightB = new DirectionalLight();
	DirectionalLight* lightC = new DirectionalLight();

	
	lightA->setColor(glm::vec3(1.0f));
	lightB->setColor(glm::vec3(0.25f));
	lightC->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	lightA->setDirection(glm::normalize(glm::vec3(0.0, -0.8, -0.2)));
	lightB->setDirection(glm::normalize(glm::vec3(1.0, -.2, 0.0f)));
	lightC->setDirection(glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	
	//application->scene.addObject(&application->sunlight);

	application->scene.getLayer(0)->addObject(lightA);
	application->scene.getLayer(0)->addObject(lightB);
	application->scene.getLayer(0)->addObject(lightC);
	application->scene.getLayer(0)->addObject(&application->camera);
	
	// Load compositor
	RenderQueue renderQueue;
	const std::list<SceneObject*>* objects = application->scene.getLayer(0)->getObjects();
	for (const SceneObject* object: *objects)
		renderQueue.queue(object);
	RenderContext renderContext;
	renderContext.camera = nullptr;
	renderContext.layer = application->scene.getLayer(0);
	renderContext.queue = &renderQueue;
	application->defaultCompositor.load(&renderContext);
	
	// Setup fade-in
	application->blackoutImage->setVisible(true);
	application->fadeInTween->start();
	
	application->inputManager->addWindowObserver(this);
	application->mouse->addMouseButtonObserver(this);
	windowResized(application->width, application->height);
	
	// Setup camera controller
	application->surfaceCam->setCamera(&application->camera);
	application->surfaceCam->setFocalPoint(Vector3(0.0f));
	application->surfaceCam->setFocalDistance(300.0f);
	application->surfaceCam->setElevation(glm::radians(35.0f));
	application->surfaceCam->setAzimuth(glm::radians(-45.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	// Setup arcball
	dragging = false;
	wasDragging = dragging;
	application->arcball.setCenter(Vector2(application->width * 0.5f, application->height * 0.5f));
	application->arcball.setRadius(application->height * 0.5f);
			

	
	// Setup colony
	
	colony.setAntModel(application->antModel);
	for (int i = 0; i < 50; ++i)
	{
		Navmesh::Triangle* triangle = (*navmesh->getTriangles())[0];
		
		ant = colony.spawn(navmesh, triangle, normalize_barycentric(Vector3(0.5f)));
		
		Vector3 forward = glm::normalize(triangle->edge->vertex->position - triangle->edge->next->vertex->position);
		Vector3 up = triangle->normal;
		ant->setOrientation(forward, up);
		
		application->scene.getLayer(0)->addObject(ant->getModelInstance());
		ant->setState(Ant::State::WANDER);
	}
	
	ant->setState(Ant::State::IDLE);
	
	// Start timer
	stateTime = 0.0f;
	application->frameTimer.reset();
	application->frameTimer.start();
	substate = 0;
}

void TitleState::execute()
{
	// Calculate delta time (in seconds)
	float dt = static_cast<float>(application->frameTimer.microseconds().count()) / 1000000.0f;
	application->frameTimer.reset();
			
	// Add dt to state time
	stateTime += dt;
	
	if (substate == 0 || substate == 1)
	{
		if (stateTime >= titleDelay && !application->titleImage->isVisible())
		{
			application->titleImage->setVisible(true);
			application->titleFadeInTween->start();
		}
		
		if (stateTime >= pressAnyKeyDelay && !application->anyKeyLabel->isVisible())
		{
			application->anyKeyLabel->setVisible(true);
			application->anyKeyFadeInTween->start();
		}
	}
	
	if (substate == 0 && stateTime >= titleDelay && application->titleFadeInTween->isStopped())
	{
		substate = 1;
	}
	
	// Listen for fade-in skip and "press any key"
	if (substate < 2)
	{
		InputEvent event;
		application->inputManager->listen(&event);
		
		if (event.type != InputEvent::Type::NONE)
		{
			application->menuControlProfile->update();
			application->inputManager->update();
						
			// Check if application was closed
			if (application->escape.isTriggered())
			{
				application->close(EXIT_SUCCESS);
				return;
			}
			// Check if fullscreen was toggled
			else if (application->toggleFullscreen.isTriggered() && !application->toggleFullscreen.wasTriggered())
			{
				application->changeFullscreen();
			}
			else if (!application->menuCancel.isTriggered())
			{
				if (substate == 0)
				{
					// Remove fade-in
					substate = 1;
					
					application->fadeInTween->stop();
					application->blackoutImage->setTintColor(Vector4(0.0f));
					application->blackoutImage->setVisible(false);
					
					application->titleFadeInTween->stop();
					application->titleImage->setVisible(true);
					application->titleImage->setTintColor(Vector4(1.0f));
					
					application->anyKeyFadeInTween->start();
					application->anyKeyLabel->setVisible(true);
				}
				else if (substate == 1)
				{
					substate = 2;
					
					application->titleFadeInTween->stop();
					application->titleFadeOutTween->start();
					application->anyKeyFadeInTween->stop();
					application->anyKeyFadeOutTween->stop();
					application->anyKeyLabel->setVisible(false);
					
					application->enterMenu(0);
					
					application->menuSelectorLabel->setVisible(true);
					//enterMenu(application, &mainMenu);
				}
			}
		}
	}
	
	// Update menu controls
	application->menuControlProfile->update();
	application->gameControlProfile->update();
	
	// Update input
	application->inputManager->update();
	
	// Check state time
	if (!fadeIn && stateTime >= blankDuration)
	{
		// Begin fade-in
		fadeIn = true;
	}
	
	/*
	glm::ivec2 mousePosition = application->mouse->getCurrentPosition();
	mousePosition.y = application->height - mousePosition.y;
	if (dragging && !wasDragging)
	{
		dragStart = application->arcball.project(Vector2(mousePosition.x, mousePosition.y));
		dragStartRotation = application->displayModelInstance->getTransform().rotation;
	}
	else if (dragging && wasDragging)
	{
		Vector3 dragEnd = application->arcball.project(Vector2(mousePosition.x, mousePosition.y));
		Quaternion rotation = glm::normalize(glm::rotation(dragStart, dragEnd));
		
		// Update display model
		Transform transform = application->displayModelInstance->getTransform();
		transform.rotation = glm::normalize(rotation * dragStartRotation);
		application->displayModelInstance->setTransform(transform);
	}
	wasDragging = dragging;
	*/

	
	// Check if application was closed
	if (application->inputManager->wasClosed() || application->escape.isTriggered())
	{
		application->close(EXIT_SUCCESS);
		return;
	}
	
	// Check if fullscreen was toggled
	if (application->toggleFullscreen.isTriggered() && !application->toggleFullscreen.wasTriggered())
	{
		application->changeFullscreen();
	}
	
	float rotationSpeed = glm::radians(3.0f) * dt / (1.0f / 60.0f);
	if (application->cameraRotateCW.isTriggered())
		application->surfaceCam->rotate(-rotationSpeed);
	if (application->cameraRotateCCW.isTriggered())
		application->surfaceCam->rotate(rotationSpeed);
	
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
		movementVector *= 0.005f * application->surfaceCam->getFocalDistance() * dt / (1.0f / 60.0f);
		application->surfaceCam->move(movementVector);
	}
	
	// Zoom camera
	float zoomFactor = application->surfaceCam->getFocalDistance() / 5.0f * dt / (1.0f / 60.0f);
	if (application->cameraZoomIn.isTriggered())
		application->surfaceCam->zoom(zoomFactor * application->cameraZoomIn.getCurrentValue());
	if (application->cameraZoomOut.isTriggered())
		application->surfaceCam->zoom(-zoomFactor * application->cameraZoomOut.getCurrentValue());
	
	float minFocalDistance = 2.0f;
	float maxFocalDistance = 500.0f;
	application->surfaceCam->setTargetFocalDistance(std::min(std::max(application->surfaceCam->getTargetFocalDistance(), minFocalDistance), maxFocalDistance));
	
	application->surfaceCam->update(dt);
		
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
	
	
	float walkSpeed = 3.0f * dt;
	float turnSpeed = 4.0f * dt;
	Vector3 antVelocity = ant->getForward() * walkSpeed;
	
	if (application->walkForward.isTriggered())
	{
		ant->move(antVelocity);
	}
	if (application->walkBack.isTriggered())
	{
		ant->move(-antVelocity);
	}
	if (application->turnLeft.isTriggered())
	{
		ant->turn(turnSpeed);
	}
	if (application->turnRight.isTriggered())
	{
		ant->turn(-turnSpeed);
	}
	
	colony.update(dt);
	
	// Perform tweening
	application->tweener->update(dt);
	
	// Update UI
	application->uiRootElement->update();
	
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	// Render BG
	application->renderer.render(application->bgScene);
	
	// Render scene
	application->renderer.render(application->scene);
	
	// Form billboard batch for UI then render UI scene
	application->uiBatcher->batch(application->uiBatch, application->uiRootElement);
	application->renderer.render(application->uiScene);
	
	// Swap buffers
	SDL_GL_SwapWindow(application->window);
}

void TitleState::exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
	
	application->inputManager->removeWindowObserver(this);
	application->exitMenu(application->currentMenuIndex);
	application->menuSelectorLabel->setVisible(false);
	
	application->scene.removeLayers();
}

void TitleState::windowClosed()
{
	application->close(EXIT_SUCCESS);
}

void TitleState::windowResized(int width, int height)
{
	// Update application dimensions
	application->width = width;
	application->height = height;
	if (application->fullscreen)
	{
		application->fullscreenWidth = width;
		application->fullscreenHeight = height;
	}
	else
	{
		application->windowedWidth = width;
		application->windowedHeight = height;
	}
	
	// Setup default render target
	application->defaultRenderTarget.width = application->width;
	application->defaultRenderTarget.height = application->height;
	
	// Resize UI
	application->resizeUI();
	
	// 3D camera
	application->camera.setPerspective(
		glm::radians(25.0f),
		(float)application->width / (float)application->height,
		0.1f,
		1000.0f);
}

void TitleState::mouseButtonPressed(int button, int x, int y)
{
	dragging = true;
}

void TitleState::mouseButtonReleased(int button, int x, int y)
{
	dragging = false;
}
