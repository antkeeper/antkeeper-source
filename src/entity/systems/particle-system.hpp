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

#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

#include "../components/model-component.hpp"
#include "../components/transform-component.hpp"
#include "../entity-group.hpp"
#include "../system.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

#include <stack>

class ParticleSystem:
	public System
{
public:
	ParticleSystem(ComponentManager* componentManager);
	~ParticleSystem();

	void setMaterial(Material* material);

	void setParticleCount(std::size_t count);
	void setDirection(const Vector3& direction);
	void setLifeTime(float time);
	void setEmissionRate(float frequency);

	const BillboardBatch* getBillboardBatch() const;
	BillboardBatch* getBillboardBatch();

	void resize(std::size_t count);
	virtual void update(float t, float dt);

	void emit(const Vector3& position);

private:
	struct Particle
	{
		Vector3 translation;
		float size;
		float life;
		float speed;
		Vector3 direction;
	};

	BillboardBatch batch;
	BillboardBatch::Range* range;

	Material* material;
	std::vector<Particle> particles;
	Vector3 direction;
	float lifeTime;
	float emissionRate;
	std::stack<std::size_t> stack;
};

inline const BillboardBatch* ParticleSystem::getBillboardBatch() const
{
	return &batch;
}

inline BillboardBatch* ParticleSystem::getBillboardBatch()
{
	return &batch;
}

#endif // PARTICLE_SYSTEM_HPP
