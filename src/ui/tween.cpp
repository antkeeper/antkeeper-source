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

#include "tween.hpp"
#include <cmath>

const TweenBase::EaseFunctionPointer TweenBase::easeFunctionPointers[] =
{
	&TweenBase::easeLinear,
	&TweenBase::easeInSine,
	&TweenBase::easeOutSine,
	&TweenBase::easeInOutSine,
	&TweenBase::easeInQuad,
	&TweenBase::easeOutQuad,
	&TweenBase::easeInOutQuad,
	&TweenBase::easeInCubic,
	&TweenBase::easeOutCubic,
	&TweenBase::easeInOutCubic,
	&TweenBase::easeInQuart,
	&TweenBase::easeOutQuart,
	&TweenBase::easeInOutQuart,
	&TweenBase::easeInQuint,
	&TweenBase::easeOutQuint,
	&TweenBase::easeInOutQuint,
	&TweenBase::easeInExpo,
	&TweenBase::easeOutExpo,
	&TweenBase::easeInOutExpo,
	&TweenBase::easeInCirc,
	&TweenBase::easeOutCirc,
	&TweenBase::easeInOutCirc,
	&TweenBase::easeInBack,
	&TweenBase::easeOutBack,
	&TweenBase::easeInOutBack,
	&TweenBase::easeInBounce,
	&TweenBase::easeOutBounce,
	&TweenBase::easeInOutBounce,
};

TweenBase::TweenBase(EaseFunction function, float time, float duration):
	time(time),
	duration(duration),
	stopped(true),
	oldStopped(true),
	paused(false)
{
	setEaseFunction(function);
}

TweenBase::TweenBase():
	time(0.0f),
	duration(0.0f),
	stopped(true),
	oldStopped(true),
	paused(false)
{
	setEaseFunction(EaseFunction::LINEAR);
}

TweenBase::~TweenBase()
{}

void TweenBase::start()
{
	if (stopped)
	{
		stopped = false;
		oldStopped = true;
		time = 0.0f;
	}
	else if (paused)
	{
		paused = false;
	}
}

void TweenBase::stop()
{
	if (!stopped)
	{
		stopped = true;
		oldStopped = false;
		paused = false;
	}
}

void TweenBase::pause()
{
	if (!stopped && !paused)
	{
		paused = true;
		oldStopped = false;
	}
}

void TweenBase::reset()
{
	time = 0.0f;
}

inline void TweenBase::setEaseFunction(EaseFunction function)
{
	easeFunction = function;
	easeFunctionPointer = easeFunctionPointers[static_cast<std::size_t>(easeFunction)];
}

void TweenBase::setTime(float time)
{
	this->time = time;
}

void TweenBase::setDuration(float duration)
{
	this->duration = duration;
}

float TweenBase::easeLinear(float t, float b, float c, float d)
{
	return c * t / d + b;
}

float TweenBase::easeInSine(float t, float b, float c, float d)
{
	return -c * std::cos(t / d * glm::half_pi<float>()) + c + b;
}

float TweenBase::easeOutSine(float t, float b, float c, float d)
{
	return c * std::sin(t / d * glm::half_pi<float>()) + b;
}

float TweenBase::easeInOutSine(float t, float b, float c, float d)
{
	return -c * 0.5f * (std::cos(glm::pi<float>() * t / d) - 1.0f) + b;
}

float TweenBase::easeInQuad(float t, float b, float c, float d)
{
	t /= d;
	return c * t * t + b;
}

float TweenBase::easeOutQuad(float t, float b, float c, float d)
{
	t /= d;
	return -c * t * (t - 2.0f) + b;
}

float TweenBase::easeInOutQuad(float t, float b, float c, float d)
{
	t /= d;
	if ((t * 0.5f) < 1.0f)
	{
		return c * 0.5f * t * t + b;
	}
	
	t -= 1.0f;
	return -c * 0.5f * (t * (t - 2.0f) - 1.0f) + b;
}

float TweenBase::easeInCubic(float t, float b, float c, float d)
{
	t /= d;
	return c * t * t * t + b;
}

float TweenBase::easeOutCubic(float t, float b, float c, float d)
{
	t = t / d - 1.0f;
	return c * (t * t * t + 1.0f) + b;
}

float TweenBase::easeInOutCubic(float t, float b, float c, float d)
{
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		return c * 0.5f * t * t * t + b;
	}
	
	t -= 2.0f;
	return c * 0.5f * (t * t * t + 2.0f) + b;
}

float TweenBase::easeInQuart(float t, float b, float c, float d)
{
	t /= d;
	return c * t * t * t * t + b;
}

float TweenBase::easeOutQuart(float t, float b, float c, float d)
{
	t = t / d - 1.0f;
	return -c * (t * t * t * t - 1.0f) + b;
}

float TweenBase::easeInOutQuart(float t, float b, float c, float d)
{
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		return c * 0.5f * t * t * t * t + b;
	}
	
	t -= 2.0f;
	return -c * 0.5f * (t * t * t * t - 2.0f) + b;
}

float TweenBase::easeInQuint(float t, float b, float c, float d)
{
	t /= d;
	return c * t * t * t * t * t + b;
}

float TweenBase::easeOutQuint(float t, float b, float c, float d)
{
	t = t / d - 1.0f;
	return c * (t * t * t * t * t + 1.0f) + b;
}

float TweenBase::easeInOutQuint(float t, float b, float c, float d)
{
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		return c * 0.5f * t * t * t * t * t + b;
	}
	
	t -= 2.0f;
	return c * 0.5f * (t * t * t * t * t + 2.0f) + b;
}

float TweenBase::easeInExpo(float t, float b, float c, float d)
{
	return (t == 0.0f) ? b : c * std::pow(2.0f, 10.0f * (t / d - 1.0f)) + b - c * 0.001f;
}

float TweenBase::easeOutExpo(float t, float b, float c, float d)
{
	return (t == d) ? b + c : c * 1.001f * (-std::pow(2.0f, -10.0f * t / d) + 1.0f) + b;
}

float TweenBase::easeInOutExpo(float t, float b, float c, float d)
{
	if (t == 0.0f)
	{
		return b;
	}
	
	if (t == d)
	{
		return b + c;
	}
	
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		return c * 0.5f * std::pow(2.0f, 10.0f * (t - 1.0f)) + b - c * 0.0005f;
	}
	
	t -= 1.0f;
	return c * 0.5f * 1.0005f * (-std::pow(2.0f, -10.0f * t) + 2.0f) + b;
}

float TweenBase::easeInCirc(float t, float b, float c, float d)
{
	t /= d;
	return -c * (std::sqrt(1.0f - t * t) - 1.0f) + b;
}
float TweenBase::easeOutCirc(float t, float b, float c, float d)
{
	t = t / d - 1.0f;
	return c * std::sqrt(1.0f - t * t) + b;
}

float TweenBase::easeInOutCirc(float t, float b, float c, float d)
{
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		return -c * 0.5f * (std::sqrt(1.0f - t * t) - 1.0f) + b;
	}
	
	t -= 2.0f;
	return c * 0.5f * (std::sqrt(1.0f - t * t) + 1.0f) + b;
}

float TweenBase::easeInBack(float t, float b, float c, float d)
{
	float s = 1.70158f;
	t /= d;
	return c * t * t * ((s + 1.0f) * t - s) + b;
}

float TweenBase::easeOutBack(float t, float b, float c, float d)
{
	float s = 1.70158f;
	t = t / d - 1.0f;
	return c * (t * t * ((s + 1.0f) * t + s) + 1.0f) + b;
}

float TweenBase::easeInOutBack(float t, float b, float c, float d)
{
	float s =  1.70158f;
	t /= d * 0.5f;
	if (t < 1.0f)
	{
		s *= 1.525f;
		return c * 0.5f * (t * t * ((s + 1.0f) * t - s)) + b;
	}
	
	t -= 2.0f;
	s *= 1.525f;
	return c * 0.5f * (t * t * ((s + 1.0f) * t + s) + 2.0f) + b;
}

float TweenBase::easeInBounce(float t, float b, float c, float d)
{
	return c - easeOutBounce(d - t, 0.0f, c, d) + b;
}

float TweenBase::easeOutBounce(float t, float b, float c, float d)
{
	t /= d;
	if (t < (1.0f / 2.75f))
	{
		return c * (7.5625f * t * t) + b;
	}
	else if (t < (2.0f / 2.75f))
	{
		t -= (1.5f / 2.75f);
		return c * (7.5625f * t * t + 0.75f) + b;
	}
	else if (t < (2.5f / 2.75f))
	{
		t -= (2.25f / 2.75f);
		return c * (7.5625f * t * t + 0.9375f) + b;
	}
	
	t -= (2.625f / 2.75f);
	return c * (7.5625f * t * t + 0.984375f) + b;
}

float TweenBase::easeInOutBounce(float t, float b, float c, float d)
{
	if (t < d * 0.5f)
	{
		return easeInBounce(t * 2.0f, 0.0f, c, d) * 0.5f + b;
	}
	
	return easeOutBounce(t * 2.0f - d, 0.0f, c, d) * 0.5f + c * 0.5f + b;
}

void Tweener::update(float dt)
{
	for (TweenBase* tween: tweens)
	{
		tween->update(dt);
	}
}

void Tweener::addTween(TweenBase* tween)
{
	tweens.push_back(tween);
}

void Tweener::removeTween(TweenBase* tween)
{
	tweens.remove(tween);
}

void Tweener::removeTweens()
{
	tweens.clear();
}
