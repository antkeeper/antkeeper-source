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

#include "experiment-state.hpp"
#include "../application.hpp"
#include "../camera-controller.hpp"
#include "../ui/ui.hpp"
#include <iostream>
#include <SDL.h>

ExperimentState::ExperimentState(Application* application):
	ApplicationState(application)
{}

ExperimentState::~ExperimentState()
{}

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


void ExperimentState::generateNest()
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

void ExperimentState::enter()
{
	std::cout << "Entering ExperimentState..." << std::endl;
	
	std::srand(std::time(0));
	
	// BG
	application->bgBatch.resize(1);
	BillboardBatch::Range* bgRange = application->bgBatch.addRange();
	bgRange->start = 0;
	bgRange->length = 1;
	Billboard* bgBillboard = application->bgBatch.getBillboard(0);
	bgBillboard->setDimensions(Vector2(1.0f, 1.0f));
	bgBillboard->setTranslation(Vector3(0.5f, 0.5f, 0.0f));
	bgBillboard->setTintColor(Vector4(1, 0, 0, 1));
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

	SceneLayer* terrainLayer = application->scene.addLayer();
	SceneLayer* objectsLayer = application->scene.addLayer();
	
	terrainLayer->addObject(&application->camera);

	
	objectsLayer->addObject(&application->camera);
	objectsLayer->addObject(application->displayModelInstance);
	objectsLayer->addObject(application->antModelInstance);
	objectsLayer->addObject(application->lineBatcher->getBatch());
	
	// Create terrain
	terrain.create(16, 16, Vector3(150.0f));
	terrainLayer->addObject(terrain.getSurfaceModel()->createInstance());
	terrainLayer->addObject(terrain.getSubsurfaceModel()->createInstance());
	
	DirectionalLight* lightA = new DirectionalLight();
	DirectionalLight* lightB = new DirectionalLight();
	DirectionalLight* lightC = new DirectionalLight();
	lightA->setColor(glm::vec3(1.0f));
	lightB->setColor(glm::vec3(0.25f));
	lightC->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
	lightA->setDirection(glm::normalize(glm::vec3(0.0, -0.8, -0.2)));
	lightB->setDirection(glm::normalize(glm::vec3(1.0, -.2, 0.0f)));
	lightC->setDirection(glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
	terrainLayer->addObject(lightA);
	terrainLayer->addObject(lightB);
	terrainLayer->addObject(lightC);
	objectsLayer->addObject(lightA);
	objectsLayer->addObject(lightB);
	objectsLayer->addObject(lightC);
	
	
	// Load compositor
	application->defaultCompositor.unload();
	RenderQueue renderQueue;
	const std::list<SceneObject*>* objects = terrainLayer->getObjects();
	for (const SceneObject* object: *objects)
		renderQueue.queue(object);
	objects = objectsLayer->getObjects();
	for (const SceneObject* object: *objects)
		renderQueue.queue(object);
	RenderContext renderContext;
	renderContext.camera = nullptr;
	renderContext.layer = objectsLayer;
	renderContext.queue = &renderQueue;
	application->defaultCompositor.load(&renderContext);
	
	application->camera.setPerspective(
		glm::radians(25.0f),
		(float)application->width / (float)application->height,
		0.5f,
		2000.0f);
	
	// Setup camera controller
	application->surfaceCam->setCamera(&application->camera);
	application->surfaceCam->setFocalPoint(Vector3(0.0f));
	application->surfaceCam->setFocalDistance(10.0f);
	application->surfaceCam->setElevation(glm::radians(90.0f * (3.0f / 4.0f)));
	application->surfaceCam->setAzimuth(glm::radians(45.0f));
	application->surfaceCam->setTargetFocalPoint(application->surfaceCam->getFocalPoint());
	application->surfaceCam->setTargetFocalDistance(application->surfaceCam->getFocalDistance());
	application->surfaceCam->setTargetElevation(application->surfaceCam->getElevation());
	application->surfaceCam->setTargetAzimuth(application->surfaceCam->getAzimuth());
	application->surfaceCam->update(0.0f);
	
	application->pauseMenuContainer->setVisible(false);
	application->pauseMenuContainer->setActive(false);
	
	// Generate nest
	generateNest();
	
	dragging = oldDragging = false;
	
	application->inputManager->addWindowObserver(this);
	application->mouse->addMouseButtonObserver(this);
	windowResized(application->width, application->height);
	
	// Start timer
	timer.start();
}

void ExperimentState::execute()
{	
	// Calculate delta time (in seconds)
	float dt = static_cast<float>(timer.microseconds().count()) / 1000000.0f;
	timer.reset();
	
	// Update controls
	application->menuControlProfile->update();
	application->gameControlProfile->update();
	
	// Update input
	oldDragging = dragging;
	application->inputManager->update();
	
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
	
	// Rotate camera
	/*
	if (application->cameraRotateCW.isTriggered() && !application->cameraRotateCW.wasTriggered())
		application->surfaceCam->rotate(glm::radians(-90.0f));
	if (application->cameraRotateCCW.isTriggered() && !application->cameraRotateCCW.wasTriggered())
		application->surfaceCam->rotate(glm::radians(90.0f));
	*/
	
	float rotationSpeed = glm::radians(3.0f) * dt / (1.0f / 60.0f);
	if (application->cameraRotateCW.isTriggered())
		application->surfaceCam->rotate(-rotationSpeed);
	if (application->cameraRotateCCW.isTriggered())
		application->surfaceCam->rotate(rotationSpeed);
	
	
	// Zoom camera
	float zoomFactor = application->surfaceCam->getFocalDistance() / 20.0f * dt / (1.0f / 60.0f);
	if (application->cameraZoomIn.isTriggered())
		application->surfaceCam->zoom(zoomFactor * application->cameraZoomIn.getCurrentValue());
	if (application->cameraZoomOut.isTriggered())
		application->surfaceCam->zoom(-zoomFactor * application->cameraZoomOut.getCurrentValue());
	
	// Enforce camera focal distance constraints
	float minFocalDistance = 2.5f;
	float maxFocalDistance = 1000.0f;
	if (application->surfaceCam->getTargetFocalDistance() > maxFocalDistance)
	{
		application->surfaceCam->setTargetFocalDistance(maxFocalDistance);
	}
	else if (application->surfaceCam->getTargetFocalDistance() < minFocalDistance)
	{
		application->surfaceCam->setTargetFocalDistance(minFocalDistance);
	}
	
	// Enforce camera focal point constraints
	float worldSize = 150.0f;
	Vector3 boundsMin = Vector3(-worldSize * 0.5f, 0.0f, -worldSize * 0.5f);
	Vector3 boundsMax = Vector3(worldSize * 0.5f, 0.0f, worldSize * 0.5f);
	Vector3 targetFocalPoint = application->surfaceCam->getTargetFocalPoint();
	targetFocalPoint.x = std::max(boundsMin.x, std::min(boundsMax.x, targetFocalPoint.x));
	targetFocalPoint.z = std::max(boundsMin.z, std::min(boundsMax.z, targetFocalPoint.z));
	application->surfaceCam->setTargetFocalPoint(targetFocalPoint);
	
	// Fixed camera angles
	float overheadViewElevation = glm::radians(90.0f * (3.0f / 4.0f));
	float tiltedViewElevation = glm::radians(30.0f);
	float nestViewElevation = glm::radians(0.0f);
	
	// Toggle overhead view
	if (!application->cameraNestView)
	{
		if (application->cameraToggleOverheadView.isTriggered() && !application->cameraToggleOverheadView.wasTriggered())
		{
			application->cameraOverheadView = !application->cameraOverheadView;
			float elevation = (application->cameraOverheadView) ? overheadViewElevation : tiltedViewElevation;
			application->surfaceCam->setTargetElevation(elevation);
		}
	}
	
	// Toggle nest view
	if (application->cameraToggleNestView.isTriggered() && !application->cameraToggleNestView.wasTriggered())
	{
		application->cameraNestView = !application->cameraNestView;
		float elevation = (application->cameraNestView) ? nestViewElevation : (application->cameraOverheadView) ? overheadViewElevation : tiltedViewElevation;
		application->surfaceCam->setTargetElevation(elevation);
	}
	
	if (application->menuSelect.isTriggered() && !application->menuSelect.wasTriggered())
	{
		generateNest();
	}
	
	application->surfaceCam->update(dt);
	
	// Picking
// Pick!!!
	glm::ivec2 mousePosition = application->mouse->getCurrentPosition();
	mousePosition.y = application->height - mousePosition.y;
	Vector4 viewport(0.0f, 0.0f, application->width, application->height);
	Vector3 mouseNear = application->camera.unproject(Vector3(mousePosition.x, mousePosition.y, 0.0f), viewport);
	Vector3 mouseFar = application->camera.unproject(Vector3(mousePosition.x, mousePosition.y, 1.0f), viewport);
	
	Ray pickingRay;
	pickingRay.origin = mouseNear;
	pickingRay.direction = glm::normalize(mouseFar - mouseNear);
	Vector3 pick;

	
	if (dragging)
	{
		auto result = pickingRay.intersects(*terrain.getSurfaceMesh());
		if (std::get<0>(result))
		{
			pick = pickingRay.extrapolate(std::get<1>(result));
		}
		else
		{
			Plane plane;
			plane.set(Vector3(0, 1, 0), Vector3(0.0f));
			auto result = pickingRay.intersects(plane);
			pick = pickingRay.extrapolate(std::get<1>(result));
		}
		
		Transform xf = Transform::getIdentity();
		xf.translation = pick;
		application->antModelInstance->setTransform(xf);
		
		if (!oldDragging)
		{
			dragStart = pick;
		}
		
		dragEnd = pick;
		
		Vector3 dragMin;
		dragMin.x = std::min(dragStart.x, dragEnd.x);
		dragMin.y = std::min(dragStart.y, dragEnd.y);
		dragMin.z = std::min(dragStart.z, dragEnd.z);
		
		Vector3 dragMax;
		dragMax.x = std::max(dragStart.x, dragEnd.x);
		dragMax.y = std::max(dragStart.y, dragEnd.y);
		dragMax.z = std::max(dragStart.z, dragEnd.z);
		
		float halfWorldSize = worldSize * 0.5f;
		application->clippingPlaneOffsets[0] = Vector3(dragMax.x, -halfWorldSize, 0.0f);
		application->clippingPlaneOffsets[1] = Vector3(0.0f, -halfWorldSize, dragMin.z);
		application->clippingPlaneOffsets[2] = Vector3(dragMin.x, -halfWorldSize, 0.0f);
		application->clippingPlaneOffsets[3] = Vector3(0.0f, -halfWorldSize, dragMax.z);
	}
	
	
	
	// Calculate clipping planes
	
	float halfWorldSize = worldSize * 0.5f;
	
	// E, N, W, S, B
	//application->clippingPlaneOffsets[0] = Vector3(halfWorldSize * 0.5f, -halfWorldSize, 0.0f);
	//application->clippingPlaneOffsets[1] = Vector3(0.0f, -halfWorldSize, -halfWorldSize * 0.5f);
	//application->clippingPlaneOffsets[2] = Vector3(-halfWorldSize * 0.5f, -halfWorldSize, 0.0f);
	//application->clippingPlaneOffsets[3] = Vector3(0.0f, -halfWorldSize, halfWorldSize * 0.5f);
	application->clippingPlaneOffsets[4] = Vector3(0.0f, -worldSize * 2.0f, 0.0f);
	application->clippingPlaneNormals[0] = Vector3(1.0f, 0.0f, 0.0f);
	application->clippingPlaneNormals[1] = Vector3(0.0f, 0.0f, -1.0f);
	application->clippingPlaneNormals[2] = Vector3(-1.0f, 0.0f, 0.0f);
	application->clippingPlaneNormals[3] = Vector3(0.0f, 0.0f, 1.0f);
	application->clippingPlaneNormals[4] = Vector3(0.0f, -1.0f, 0.0f);
	
	for (int i = 0; i < 5; ++i)
	{
		application->clippingPlanes[i].set(application->clippingPlaneNormals[i], application->clippingPlaneOffsets[i]);
	}
	application->lightingPass.setClippingPlanes(&application->clippingPlanes[0]);
	
	
	application->lineBatcher->getBatch()->update();
	
	// Perform tweening
	application->tweener->update(dt);
	
	// Update UI
	application->uiRootElement->update();
	
	// Clear to black
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	// Render background
	application->renderer.render(application->bgScene);
	
	// Render scene
	application->renderer.render(application->scene);
	
	// Form billboard batch for UI then render UI scene
	application->uiBatcher->batch(application->uiBatch, application->uiRootElement);
	application->renderer.render(application->uiScene);
	
	// Swap buffers
	SDL_GL_SwapWindow(application->window);
}

void ExperimentState::exit()
{
	std::cout << "Exiting ExperimentState..." << std::endl;
	application->inputManager->removeWindowObserver(this);
}

void ExperimentState::windowClosed()
{
	application->close(EXIT_SUCCESS);
}

void ExperimentState::windowResized(int width, int height)
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
	
	// UI camera
	application->uiCamera.setOrthographic(0, application->width, application->height, 0, -1.0f, 1.0f);
	
	// 3D camera
	application->camera.setPerspective(
		glm::radians(25.0f),
		(float)application->width / (float)application->height,
		0.5f,
		2000.0f);
}

void ExperimentState::mouseButtonPressed(int button, int x, int y)
{
	dragging = true;
}

void ExperimentState::mouseButtonReleased(int button, int x, int y)
{
	dragging = false;
}