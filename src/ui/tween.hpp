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

#ifndef TWEEN_HPP
#define TWEEN_HPP

#include <algorithm>
#include <functional>
#include <list>
#include <iostream>
#include <emergent/emergent.hpp>

using namespace Emergent;

/// @see http://easings.net/
/// @see http://wiki.unity3d.com/index.php?title=Tween
enum class EaseFunction
{
	LINEAR,
	IN_SINE,
	OUT_SINE,
	IN_OUT_SINE,
	IN_QUAD,
	OUT_QUAD,
	IN_OUT_QUAD,
	IN_CUBIC,
	OUT_CUBIC,
	IN_OUT_CUBIC,
	IN_QUART,
	OUT_QUART,
	IN_OUT_QUART,
	IN_QUINT,
	OUT_QUINT,
	IN_OUT_QUINT,
	IN_EXPO,
	OUT_EXPO,
	IN_OUT_EXPO,
	IN_CIRC,
	OUT_CIRC,
	IN_OUT_CIRC,
	IN_BACK,
	OUT_BACK,
	IN_OUT_BACK,
	IN_BOUNCE,
	OUT_BOUNCE,
	IN_OUT_BOUNCE
};

class TweenBase
{
public:
	TweenBase(EaseFunction function, float time, float duration);
	TweenBase();
	virtual ~TweenBase();
	
	void start();
	void stop();
	void pause();
	void reset();
	
	void setEaseFunction(EaseFunction function);
	void setTime(float time);
	void setDuration(float duration);
	
	EaseFunction getEaseFunction() const;
	float getTime() const;
	float getDuration() const;
	bool isStopped() const;
	bool wasStopped() const;
	bool isPaused() const;
	
protected:
	typedef float (*EaseFunctionPointer)(float, float, float, float);
	
	EaseFunction easeFunction;
	EaseFunctionPointer easeFunctionPointer;
	float time;
	float duration;
	bool stopped;
	bool oldStopped;
	bool paused;
	
private:
	friend class Tweener;
	
	static const EaseFunctionPointer easeFunctionPointers[28];
	
	virtual void update(float dt) = 0;
	
	static float easeLinear(float t, float b, float c, float d);
	static float easeInSine(float t, float b, float c, float d);
	static float easeOutSine(float t, float b, float c, float d);
	static float easeInOutSine(float t, float b, float c, float d);
	static float easeInQuad(float t, float b, float c, float d);
	static float easeOutQuad(float t, float b, float c, float d);
	static float easeInOutQuad(float t, float b, float c, float d);
	static float easeInCubic(float t, float b, float c, float d);
	static float easeOutCubic(float t, float b, float c, float d);
	static float easeInOutCubic(float t, float b, float c, float d);
	static float easeInQuart(float t, float b, float c, float d);
	static float easeOutQuart(float t, float b, float c, float d);
	static float easeInOutQuart(float t, float b, float c, float d);
	static float easeInQuint(float t, float b, float c, float d);
	static float easeOutQuint(float t, float b, float c, float d);
	static float easeInOutQuint(float t, float b, float c, float d);
	static float easeInExpo(float t, float b, float c, float d);
	static float easeOutExpo(float t, float b, float c, float d);
	static float easeInOutExpo(float t, float b, float c, float d);
	static float easeInCirc(float t, float b, float c, float d);
	static float easeOutCirc(float t, float b, float c, float d);
	static float easeInOutCirc(float t, float b, float c, float d);
	static float easeInBack(float t, float b, float c, float d);
	static float easeOutBack(float t, float b, float c, float d);
	static float easeInOutBack(float t, float b, float c, float d);
	static float easeInBounce(float t, float b, float c, float d);
	static float easeOutBounce(float t, float b, float c, float d);
	static float easeInOutBounce(float t, float b, float c, float d);
};

inline EaseFunction TweenBase::getEaseFunction() const
{
	return easeFunction;
}

inline float TweenBase::getTime() const
{
	return time;
}

inline float TweenBase::getDuration() const
{
	return duration;
}

inline bool TweenBase::isStopped() const
{
	return stopped;
}

inline bool TweenBase::wasStopped() const
{
	return oldStopped;
}

inline bool TweenBase::isPaused() const
{
	return paused;
}

template <typename T>
class Tween: public TweenBase
{
public:
	Tween(EaseFunction function, float time, float duration, const T& startValue, const T& deltaValue);
	Tween();
	virtual ~Tween();
	
	void setStartValue(const T& startValue);
	void setDeltaValue(const T& deltaValue);
	void setStartCallback(std::function<void(const T&)> callback);
	void setUpdateCallback(std::function<void(const T&)> callback);
	void setEndCallback(std::function<void(const T&)> callback);
	
	const T& getStartValue() const;
	const T& getDeltaValue() const;
	const T& getTweenValue() const;
	
	std::function<void(const T&)> getStartCallback() const;
	std::function<void(const T&)> getUpdateCallback() const;
	std::function<void(const T&)> getEndCallback() const;
	
private:
	virtual void update(float dt);
	
	void calculateTweenValue();
	
	T startValue;
	T deltaValue;
	T tweenValue;
	std::function<void(const T&)> startCallback;
	std::function<void(const T&)> updateCallback;
	std::function<void(const T&)> endCallback;
};

template <typename T>
Tween<T>::Tween(EaseFunction function, float time, float duration, const T& startValue, const T& deltaValue):
	TweenBase(function, time, duration),
	startValue(startValue),
	deltaValue(deltaValue),
	tweenValue(startValue),
	startCallback(nullptr),
	updateCallback(nullptr),
	endCallback(nullptr)
{}

template <typename T>
Tween<T>::Tween():
	startCallback(nullptr),
	updateCallback(nullptr),
	endCallback(nullptr)
{}

template <typename T>
Tween<T>::~Tween()
{}

template <typename T>
void Tween<T>::update(float dt)
{
	if (isStopped() || isPaused())
	{
		return;
	}
	
	// Check if tween was just started
	if (!isStopped() && wasStopped())
	{
		// Execute start callback
		if (startCallback != nullptr)
		{
			startCallback(startValue);
		}
	}
	oldStopped = stopped;
	
	// Add delta time to time and calculate tween value
	time = std::min<float>(duration, time + dt);
	calculateTweenValue();
	
	// Execute update callback
	if (updateCallback != nullptr)
	{
		updateCallback(tweenValue);
	}
	
	// Check if tween has ended
	if (time >= duration)
	{
		if (!isStopped())
		{
			// Stop tween
			stop();
			
			// Execute end callback
			if (endCallback != nullptr)
			{
				endCallback(tweenValue);
			}
		}
	}
}

template <typename T>
inline void Tween<T>::setStartValue(const T& startValue)
{
	this->startValue = startValue;
}

template <typename T>
inline void Tween<T>::setDeltaValue(const T& deltaValue)
{
	this->deltaValue = deltaValue;
}

template <typename T>
inline void Tween<T>::setStartCallback(std::function<void(const T&)> callback)
{
	this->startCallback = callback;
}

template <typename T>
inline void Tween<T>::setUpdateCallback(std::function<void(const T&)> callback)
{
	this->updateCallback = callback;
}

template <typename T>
inline void Tween<T>::setEndCallback(std::function<void(const T&)> callback)
{
	this->endCallback = callback;
}

template <typename T>
inline const T& Tween<T>::getStartValue() const
{
	return startValue;
}

template <typename T>
inline const T& Tween<T>::getDeltaValue() const
{
	return deltaValue;
}

template <typename T>
inline const T& Tween<T>::getTweenValue() const
{
	return tweenValue;
}

template <typename T>
inline std::function<void(const T&)> Tween<T>::getStartCallback() const
{
	return startCallback;
}

template <typename T>
inline std::function<void(const T&)> Tween<T>::getUpdateCallback() const
{
	return updateCallback;
}

template <typename T>
inline std::function<void(const T&)> Tween<T>::getEndCallback() const
{
	return endCallback;
}

template <typename T>
inline void Tween<T>::calculateTweenValue()
{
	tweenValue = easeFunctionPointer(time, startValue, deltaValue, duration);
}

template <>
inline void Tween<Vector2>::calculateTweenValue()
{
	tweenValue.x = easeFunctionPointer(time, startValue.x, deltaValue.x, duration);
	tweenValue.y = easeFunctionPointer(time, startValue.y, deltaValue.y, duration);
}

template <>
inline void Tween<Vector3>::calculateTweenValue()
{
	tweenValue.x = easeFunctionPointer(time, startValue.x, deltaValue.x, duration);
	tweenValue.y = easeFunctionPointer(time, startValue.y, deltaValue.y, duration);
	tweenValue.z = easeFunctionPointer(time, startValue.z, deltaValue.z, duration);

}

template <>
inline void Tween<Vector4>::calculateTweenValue()
{
	tweenValue.x = easeFunctionPointer(time, startValue.x, deltaValue.x, duration);
	tweenValue.y = easeFunctionPointer(time, startValue.y, deltaValue.y, duration);
	tweenValue.z = easeFunctionPointer(time, startValue.z, deltaValue.z, duration);
	tweenValue.w = easeFunctionPointer(time, startValue.w, deltaValue.w, duration);
}

class Tweener
{
public:
	void update(float dt);
	
	void addTween(TweenBase* tween);
	void removeTween(TweenBase* tween);
	void removeTweens();
	
private:
	std::list<TweenBase*> tweens;
};

#endif // TWEEN_HPP