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

#include "particle-system.hpp"
#include "../../game/curl-noise.hpp"

ParticleSystem::ParticleSystem(ComponentManager* componentManager):
	System(componentManager),
	range(nullptr),
	material(nullptr)
{
	batch.setTransform(Transform::getIdentity());
	batch.setCullingEnabled(false);
}

ParticleSystem::~ParticleSystem()
{}

void ParticleSystem::resize(std::size_t count)
{
	particles.resize(count);
	batch.resize(count);
	range = batch.addRange();
	range->start = 0;
	range->length = particles.size();
	range->material = material;

	while (!stack.empty())
		stack.pop();

	for (int i = 0; i < particles.size(); ++i)
	{
		Particle& particle = particles[i];
		particle.life = 0.0f;
		particle.size = 0.0f;

		Billboard* billboard = batch.getBillboard(i);
		billboard->setDimensions(Vector2(particle.size));
		billboard->resetTweens();

		stack.push(i);
	}
}

void ParticleSystem::emit(const Vector3& position)
{
	if (!stack.empty())
	{
		std::size_t index = stack.top();
		stack.pop();

		Particle& particle = particles[index];
		particle.life = frand(1.0f, 5.0f);
		particle.translation = position;
		particle.size = frand(0.01f, 0.2f);
		particle.speed = frand(2.0f, 3.0f);
		particle.direction = direction + Vector3(frand(-1, 1), 0, frand(-1, 1)) * 0.1f;
		particle.direction = glm::normalize(particle.direction);

		Billboard* billboard = batch.getBillboard(index);
		billboard->setTranslation(particle.translation);
		billboard->setRotation(Quaternion(1, 0, 0, 0));
		billboard->setDimensions(Vector2(particle.size));
		billboard->setTintColor(Vector4(1.0f));
		billboard->resetTweens();
	}
}

void ParticleSystem::update(float t, float dt)
{
	if (stack.size() == particles.size())
	{
		// Inactive
		return;
	}

	batch.reset();

	const Vector3 wind = glm::normalize(Vector3(1.0f, 0.0f, -1.0f)) * 1.5f * dt;
	float frequency = 0.4f;
	Vector3 noiseOffset = Vector3(77.7f, 33.3f, 11.1f) * t * 0.01f;

	for (std::size_t i = 0; i < particles.size(); ++i)
	{
		Particle& particle = particles[i];
		if (particle.life <= 0.0f)
		{
			continue;
		}


		Billboard* billboard = batch.getBillboard(i);

		bool reset = false;
		
		Vector3 smoke = curl(particle.translation, noiseOffset, frequency) * 8.0f;


		particle.translation += particle.direction * particle.speed * dt + smoke * dt + wind;
		particle.size += 0.1f * dt;
		particle.life -= dt;
		if (particle.life <= 0.0f)
		{
			particle.size = 0.0f;
			reset = true;

			stack.push(i);
		}

		billboard->setTranslation(particle.translation);
		billboard->setRotation(Quaternion(1, 0, 0, 0));
		billboard->setDimensions(Vector2(particle.size));
		billboard->setTintColor(Vector4(0.5f));

		if (reset)
		{
			billboard->resetTweens();
		}
	}
}

void ParticleSystem::setMaterial(Material* material)
{
	this->material = material;
	if (range)
	{
		range->material = material;
	}
}

void ParticleSystem::setDirection(const Vector3& direction)
{
	this->direction = direction;
}

void ParticleSystem::setLifeTime(float time)
{
	this->lifeTime = time;
}

void ParticleSystem::setEmissionRate(float frequency)
{
	this->emissionRate = frequency;
}

