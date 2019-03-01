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

#include "sandbox-state.hpp"
#include "game.hpp"
#include "ui/ui.hpp"
#include "graphics/lighting-render-pass.hpp"
#include "game/camera-rig.hpp"
#include "game/tool.hpp"
#include "game/lens.hpp"
#include "game/forceps.hpp"
#include "game/brush.hpp"
#include <cmath>

SandboxState::SandboxState(Game* game):
	GameState(game)
{}

SandboxState::~SandboxState()
{}

void SandboxState::enter()
{
	// Subscribe this state to input events
	game->getEventDispatcher()->subscribe<MouseMovedEvent>(this);
	game->getEventDispatcher()->subscribe<MouseButtonPressedEvent>(this);
	game->getEventDispatcher()->subscribe<MouseButtonReleasedEvent>(this);

	// Make HUD visible
	//game->hudContainer->setVisible(true);
	game->radialMenuContainer->setVisible(false);

	// Show mouse
	game->mouse->setVisible(false);

	game->fadeIn(1.0f, Vector3(0.0f), nullptr);

	Vector3 focalPoint = Vector3(0.0f);
	float focalDistance = 5.0f;
	float elevation = glm::radians(30.0f);
	float azimuth = glm::radians(-45.0f);

	game->cameraRig = game->orbitCam;
	game->orbitCam->setFocalPoint(focalPoint);
	game->orbitCam->setTargetFocalPoint(focalPoint);
	game->orbitCam->setFocalDistance(focalDistance);
	game->orbitCam->setTargetFocalDistance(focalDistance);
	game->orbitCam->setElevation(elevation);
	game->orbitCam->setTargetElevation(elevation);
	game->orbitCam->setAzimuth(azimuth);
	game->orbitCam->setTargetAzimuth(azimuth);

	game->freeCam->setTranslation(Vector3(-5, 5.0f, -5.0f));
	game->cameraRig = game->freeCam;
	game->mouse->setRelativeMode(true);

	toolIndex = 0;
	game->selectTool(toolIndex);
	//game->currentTool->setActive(false);
	game->mouse->warp(game->window, game->w / 2, game->h / 2);

	zoom = 0.5f;
	noPick = false;

	//game->boxSelect(100, 100, 500, 300);
}

void SandboxState::execute()
{
	game->lightingPass->setTime(game->time);
	bool menuClosed = false;

	if (game->openRadialMenuControl.isActive() && !game->openRadialMenuControl.wasActive())
	{
		game->radialMenuContainer->setVisible(true);
		game->hudContainer->setVisible(false);

		savedMousePosition = game->mouse->getCurrentPosition();
		selectorVector = Vector2(0.0f);
		game->mouse->setRelativeMode(true);
	}
	else if (!game->openRadialMenuControl.isActive() && game->openRadialMenuControl.wasActive())
	{
		game->radialMenuContainer->setVisible(false);
		//game->hudContainer->setVisible(true);
		game->mouse->setRelativeMode(false);
		game->mouse->warp(game->window, std::get<0>(savedMousePosition), std::get<1>(savedMousePosition));
		menuClosed = true;

		game->selectTool(toolIndex);
	}

	float speed = 3.0f * game->timestep;
	Vector2 forward = Vector2(0, -1);
	Vector2 right = Vector2(1, 0);
	Vector2 direction = Vector2(0);
	if (game->moveForwardControl.isActive())
	{
		direction += forward;
	}
	if (game->moveBackControl.isActive())
	{
		direction -= forward;
	}
	if (game->moveLeftControl.isActive())
	{
		direction -= right;
	}
	if (game->moveRightControl.isActive())
	{
		direction += right;
	}
	if (glm::length2(direction) != 0.0f)
	{
		direction = glm::normalize(direction);
	}

	float rotationAngle = glm::radians(180.0f) * game->timestep;
	if (game->rotateCCWControl.isActive())
	{
		game->orbitCam->rotate(-rotationAngle);
	}
	if (game->rotateCWControl.isActive())
	{
		game->orbitCam->rotate(rotationAngle);
	}
	
	float zoomSpeed = 3.0f * game->timestep;
	if (game->zoomInControl.isActive())
	{
		zoom += zoomSpeed * game->zoomInControl.getCurrentValue();
	}
	if (game->zoomOutControl.isActive())
	{
		zoom -= zoomSpeed * game->zoomOutControl.getCurrentValue();
	}
	zoom = std::max<float>(0.0f, std::min<float>(1.0f, zoom));

	float minFocalDistance = 5.0f;
	float maxFocalDistance = 70.0f;
	float logMinFocalDistance = std::log(minFocalDistance);
	float logMaxFocalDistance = std::log(maxFocalDistance);
	float logFocalDistance = lerp(logMinFocalDistance, logMaxFocalDistance, 1.0f - zoom);
	float focalDistance = std::exp(logFocalDistance);

	float minFOV = glm::radians(30.0f);
	float maxFOV = glm::radians(60.0f);
	float logMinFOV = std::log(minFOV);
	float logMaxFOV = std::log(maxFOV);
	float logFOV = lerp(logMinFOV, logMaxFOV, 1.0f - zoom);
	float fov = std::exp(logFOV);

	float minClipNear = 1.0f;
	float maxClipNear = 10.0f;
	float minClipFar = 80.0f;
	float maxClipFar = 350.0f;
	float logMinClipNear = std::log(minClipNear);
	float logMaxClipNear = std::log(maxClipNear);
	float logMinClipFar = std::log(minClipFar);
	float logMaxClipFar = std::log(maxClipFar);
	float logClipNear = lerp(logMinClipNear, logMaxClipNear, 1.0f - zoom);
	float logClipFar = lerp(logMinClipFar, logMaxClipFar, 1.0f - zoom);
	float clipNear = std::exp(logClipNear);
	float clipFar = std::exp(logClipFar);

	float nearElevation = glm::radians(40.0f);
	float farElevation = glm::radians(80.0f);
	float logNearElevation = std::log(nearElevation);
	float logFarElevation = std::log(farElevation);
	float logElevation = lerp(logNearElevation, logFarElevation, 1.0f - zoom);
	float elevation = std::exp(logElevation);

	float minMovementSpeed = 2.5f * game->timestep;
	float maxMovementSpeed = 40.0f * game->timestep;
	float logMinMovementSpeed = std::log(minMovementSpeed);
	float logMaxMovementSpeed = std::log(maxMovementSpeed);
	float logMovementSpeed = lerp(logMinMovementSpeed, logMaxMovementSpeed, 1.0f - zoom);
	float movementSpeed = std::exp(logMovementSpeed);

	game->orbitCam->setTargetFocalDistance(focalDistance);
	game->orbitCam->getCamera()->setPerspective(fov, (float)game->w / (float)game->h, clipNear, clipFar);
	game->orbitCam->move(direction * movementSpeed);
	game->orbitCam->setFocalPoint(game->orbitCam->getTargetFocalPoint());
	//game->orbitCam->setTargetElevation(elevation);
	//
	
	if (game->cameraRig == game->freeCam)
	{
		game->freeCam->move(direction * movementSpeed);
	}
	
	if (game->cameraRig)
	{
		game->cameraRig->update(game->timestep);
	}

	float nearLabelDistance = 0.25f;
	float farLabelDistance = 1.0f;
	float logNearLabelDistance = std::log(nearLabelDistance);
	float logFarLabelDistance = std::log(farLabelDistance);
	float logLabelDistance = lerp(logNearLabelDistance, logFarLabelDistance, 1.0f - zoom);
	float labelDistance = std::exp(logLabelDistance);

	if (!game->radialMenuContainer->isVisible() && !menuClosed)
	{
		// Picking
		Vector3 pick;
		std::tuple<int, int> mousePosition = game->mouse->getCurrentPosition();

		std::get<1>(mousePosition) = game->h - std::get<1>(mousePosition);
		Vector4 viewport(0.0f, 0.0f, game->w, game->h);
		Vector3 mouseNear = game->cameraRig->getCamera()->unproject(Vector3(std::get<0>(mousePosition), std::get<1>(mousePosition), 0.0f), viewport);
		Vector3 mouseFar = game->cameraRig->getCamera()->unproject(Vector3(std::get<0>(mousePosition), std::get<1>(mousePosition), 1.0f), viewport);
		
		Ray pickingRay;
		pickingRay.origin = mouseNear;
		pickingRay.direction = glm::normalize(mouseFar - mouseNear);

		Plane plane(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f));
		auto pickingIntersection = pickingRay.intersects(plane);
		bool picked = std::get<0>(pickingIntersection);
		if (picked)
		{
			pick = pickingRay.extrapolate(std::get<1>(pickingIntersection));
		}

		if (picked)
		{
			if (game->currentTool)
			{
				if (!noPick)
				{
					game->currentTool->setPick(pick);
				}
				game->currentTool->update(game->timestep);
			}

			if (game->adjustCameraControl.isActive() || game->dragCameraControl.isActive())
			{
				noPick = true;
			}
			else
			{
				noPick = false;
			}



			if (game->adjustCameraControl.isActive() && !game->adjustCameraControl.wasActive())
			{
				Vector3 focalPoint = pick;
				game->orbitCam->setTargetFocalPoint(focalPoint);
				savedMousePosition = game->mouse->getCurrentPosition();
				game->mouse->setRelativeMode(true);
			}
			else if (game->dragCameraControl.isActive())
			{
				if (!game->dragCameraControl.wasActive())
				{
					dragMousePosition = mousePosition;
					dragReferencePoint = pick;
					dragOffset = dragReferencePoint;
				}
				else
				{
					Vector4 viewport(0.0f, 0.0f, game->w, game->h);
					Vector3 dragMouseNear = game->cameraRig->getCamera()->unproject(Vector3(std::get<0>(dragMousePosition), std::get<1>(dragMousePosition), 0.0f), viewport);
					Vector3 dragMouseFar = game->cameraRig->getCamera()->unproject(Vector3(std::get<0>(dragMousePosition), std::get<1>(dragMousePosition), 1.0f), viewport);
					
					Ray dragRay;
					dragRay.origin = dragMouseNear;
					dragRay.direction = glm::normalize(dragMouseFar - dragMouseNear);
					auto dragPickResult = pickingRay.intersects(plane);

					Vector3 dragPick;
					if (std::get<0>(dragPickResult))
					{
						dragPick = dragRay.extrapolate(std::get<1>(dragPickResult));


						game->orbitCam->setTargetFocalPoint(game->orbitCam->getTargetFocalPoint() + dragOffset - pick);

						game->orbitCam->setFocalPoint(game->orbitCam->getTargetFocalPoint());
					}


				}
			}
		}
	}

	if (!game->adjustCameraControl.isActive() && game->adjustCameraControl.wasActive())
	{
		game->mouse->setRelativeMode(false);
		game->mouse->warp(game->window, game->w / 2, game->h / 2);
		noPick = false;
	}

	if (game->toggleNestViewControl.isActive() && !game->toggleNestViewControl.wasActive())
	{
		game->fadeOut(0.5f, Vector3(0.0f), std::bind(&Game::fadeIn, game, 0.5f, Vector3(0.0f), nullptr));
	}
}

void SandboxState::exit()
{
	// Unsubscribe this state from input events
	game->getEventDispatcher()->unsubscribe<MouseMovedEvent>(this);
	game->getEventDispatcher()->unsubscribe<MouseButtonPressedEvent>(this);
	game->getEventDispatcher()->unsubscribe<MouseButtonReleasedEvent>(this);

	// Make HUD invisible
	game->hudContainer->setVisible(false);
}

void SandboxState::handleEvent(const MouseMovedEvent& event)
{
	float dx = event.dx;
	float dy = event.dy;
	if (game->radialMenuContainer->isVisible())
	{
		selectorVector.x += event.dx * 0.5f;
		selectorVector.y += event.dy * 0.5f;

		float lengthSquared = glm::length2(selectorVector);
		if (lengthSquared > 0.0f)
		{
			// Limit selector vector magnitude
			float maxLength = 100.0f;
			if (lengthSquared > maxLength * maxLength)
			{
				selectorVector = (selectorVector / std::sqrt(lengthSquared)) * maxLength;
				lengthSquared = maxLength * maxLength;
			}

			float minLength = 3.0f;
			if (lengthSquared >= minLength * minLength)
			{
				Vector2 selectorDirection = selectorVector / std::sqrt(lengthSquared);

				float angle = std::atan2(-selectorDirection.y, selectorDirection.x) + twoPi<float>();
				float sectorAngle = twoPi<float>() / 8.0f;
				int sector = static_cast<int>((angle + sectorAngle * 0.5f) / sectorAngle);
				game->radialMenuSelectorImage->setRotation(static_cast<float>(sector) * sectorAngle);
				game->radialMenuImage->setRotation(static_cast<float>(sector) * sectorAngle);
				toolIndex = (8 - ((sector - 4) % 8)) % 8;
			}
		}
	}
	else if (game->adjustCameraControl.isActive())
	{
		bool invertX = true;
		bool invertY = false;

		float rotationFactor = event.dx;
		float elevationFactor = event.dy;

		if (invertX)
		{
			rotationFactor *= -1.0f;
		}
		if (invertY)
		{
			elevationFactor *= -1.0f;
		}

		float rotation = glm::radians(22.5f) * rotationFactor * game->timestep;

		float minElevation = glm::radians(-80.0f);
		float maxElevation = glm::radians(80.0f);
		float elevation = game->orbitCam->getTargetElevation() + elevationFactor * 0.25f * game->timestep;
		elevation = std::min<float>(maxElevation, std::max<float>(minElevation, elevation));


		game->orbitCam->rotate(rotation);
		game->orbitCam->setTargetElevation(elevation);
	}
	
	if (game->cameraRig == game->freeCam)
	{
		float angle = glm::radians(0.5f);
		bool invertX = false;
		bool invertY = false;

		float pan = -dx * angle;
		float tilt = -dy * angle;

		if (invertX)
			pan = -pan;
		if (invertY)
			tilt = -tilt;
		game->freeCam->rotate(pan, tilt);
	}

	game->brush->setTiltParams(Vector2(event.x, event.y), Vector2(game->w, game->h));
}

void SandboxState::handleEvent(const MouseButtonPressedEvent& event)
{
	if (event.button == 1)
	{
		game->lens->focus();
		game->forceps->pinch();
		game->brush->press();
	}
}

void SandboxState::handleEvent(const MouseButtonReleasedEvent& event)
{
	if (event.button == 1)
	{
		game->lens->unfocus();
		game->forceps->release();
		game->brush->release();
	}
}

