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
	
	/*
	// Load tunnel texture
	GLuint tunnelTexture;
	loadTexture("data/textures/soil-01.png", &tunnelTexture);
	
	// Setup triplanar texturing
	Material* material = (Material*)application->displayModel->getMaterial(0);
	material->setRoughness(0.75f);
	material->setDiffuseColor(glm::vec3(1.0f));
	material->setSpecularColor(glm::vec3(0.001f));
	material->setOpacity(0.999f);
	
	Texture* texture = material->createTexture();
	texture->setTextureID(tunnelTexture);
	texture->setCoordinateSource(TextureCoordinateSource::TRIPLANAR_PROJECTION);
	texture->setCoordinateScale(glm::vec3(1.0f / 2.0f));
	texture->setDiffuseInfluence(1.0f);
	*/
	
	
	application->displayModelInstance->setModel(application->displayModel);
	application->displayModelInstance->setTransform(Transform::getIdentity());
	
	application->antModelInstance->setModel(application->antModel);
	application->antModelInstance->setTransform(Transform::getIdentity());
	
	// Setup lighting
	application->sunlight.setColor(glm::vec3(1.0f));
	application->sunlight.setDirection(glm::normalize(glm::vec3(0.5, -1, -0.5)));
	
	
	// Setup lighting pass
	application->lightingPass.setRenderTarget(&application->defaultRenderTarget);
	application->lightingPass.setShadowMap(0);
	application->lightingPass.setShadowCamera(&application->camera);
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

	application->scene.getLayer(0)->addObject(application->displayModelInstance);
	application->scene.getLayer(0)->addObject(application->antModelInstance);
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
	windowResized(application->width, application->height);
	
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
		
		if (stateTime >= copyrightDelay && !application->copyrightImage->isVisible())
		{
			//application->copyrightImage->setVisible(true);
			//application->copyrightFadeInTween->start();
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
					//application->copyrightFadeInTween->stop();
					//application->copyrightFadeOutTween->start();
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
	
	// Check state time
	if (!fadeIn && stateTime >= blankDuration)
	{
		// Begin fade-in
		fadeIn = true;
	}
	
	// Update display model
	Transform transform = application->displayModelInstance->getTransform();
	transform.translation = Vector3(0, 0.0f, 0);
	transform.scale = Vector3(0.75f);
	transform.rotation = glm::angleAxis(stateTime * glm::radians(360.0f) / 60.0f, glm::vec3(0, 1, 0));
	application->displayModelInstance->setTransform(transform);
	
	transform.scale = Vector3(0.25f);
	transform.translation.y = 0.0f;
	//application->antModelInstance->setTransform(transform);

	
	// Update menu controls
	application->menuControlProfile->update();
	
	// Update input
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
	
	// Perform tweening
	application->tweener->update(dt);
	
	// Update UI
	application->uiRootElement->update();
	
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
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