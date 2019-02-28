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

#include "splash-state.hpp"
#include "game.hpp"
#include "sandbox-state.hpp"
#include "ui/ui.hpp"

SplashState::SplashState(Game* game):
	GameState(game)
{}

SplashState::~SplashState()
{}

void SplashState::enter()
{
	AnimationChannel<float>* channel;

	// Construct fade-in animation clip
	fadeInClip.setInterpolator(lerp<float>);
	channel = fadeInClip.addChannel(0);
	channel->insertKeyframe(0.0f, 0.0f);
	channel->insertKeyframe(0.5f, 0.0f);
	channel->insertKeyframe(1.25f, 1.0f);
	channel->insertKeyframe(5.25f, 1.0f);

	// Construct fade-out animation clip
	fadeOutClip.setInterpolator(lerp<float>);
	channel = fadeOutClip.addChannel(0);
	channel->insertKeyframe(0.0f, 1.0f);
	channel->insertKeyframe(0.75f, 0.0f);
	channel->insertKeyframe(1.25f, 0.0f);

	// Setup animate callback to change splash screen opacity
	fadeAnimation.setAnimateCallback
	(
		[this](std::size_t id, float opacity)
		{
			this->game->splashImage->setTintColor(Vector4(1.0f, 1.0f, 1.0f, opacity));
		}
	);

	// Setup end callback to fade-out after fading in
	fadeAnimation.setEndCallback
	(
		[this]()
		{
			// Change clip to fade-out
			this->fadeAnimation.setClip(&fadeOutClip);
			this->fadeAnimation.setTimeFrame(this->fadeOutClip.getTimeFrame());
			this->fadeAnimation.rewind();
			this->fadeAnimation.play();

			// Setup end callback to change to title state after fading out
			this->fadeAnimation.setEndCallback
			(
				[this]()
				{
					//this->game->changeState(this->game->titleState);
					this->game->changeState(this->game->sandboxState);
				}
			);
		}
	);

	// Setup fade animation to fade-in
	fadeAnimation.setSpeed(1.0f);
	fadeAnimation.setLoop(false);
	fadeAnimation.setClip(&fadeInClip);
	fadeAnimation.setTimeFrame(fadeInClip.getTimeFrame());
	
	// Play the fade animation
	fadeAnimation.play();

	// Add fade animation to the animator
	game->getAnimator()->addAnimation(&fadeAnimation);

	// Subscribe this state to input events
	game->getEventDispatcher()->subscribe<KeyPressedEvent>(this);
	game->getEventDispatcher()->subscribe<MouseButtonPressedEvent>(this);
	game->getEventDispatcher()->subscribe<GamepadButtonPressedEvent>(this);

	// Make splash screen visible
	game->splashBackgroundImage->setVisible(true);
	game->splashImage->setVisible(true);
	game->splashImage->setTintColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	game->splashBackgroundImage->setTintColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	game->splashImage->resetTweens();
	game->splashBackgroundImage->resetTweens();
	game->uiRootElement->update();

	// Hide mouse
	game->mouse->setVisible(false);
}

void SplashState::execute()
{}

void SplashState::exit()
{
	// Remove fade animation from the animator
	game->getAnimator()->removeAnimation(&fadeAnimation);

	// Unsubscribe this state from input events
	game->getEventDispatcher()->unsubscribe<KeyPressedEvent>(this);
	game->getEventDispatcher()->unsubscribe<MouseButtonPressedEvent>(this);
	game->getEventDispatcher()->unsubscribe<GamepadButtonPressedEvent>(this);

	// Make splash screen invisible
	game->splashBackgroundImage->setVisible(false);
	game->splashImage->setVisible(false);
}

void SplashState::handleEvent(const KeyPressedEvent& event)
{
	skip();
}

void SplashState::handleEvent(const MouseButtonPressedEvent& event)
{
	skip();
}

void SplashState::handleEvent(const GamepadButtonPressedEvent& event)
{
	skip();
}

void SplashState::skip()
{
	game->splashImage->setVisible(false);
	game->changeState(game->sandboxState);
}

