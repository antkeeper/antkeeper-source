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

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <emergent/emergent.hpp>
#include "materials.hpp"

using namespace Emergent;

class LineBatcher
{
public:
	LineBatcher(std::size_t lineCount);
	~LineBatcher();
	
	void setWidth(float width);
	void setColor(const Vector4& color);
	
	void begin();
	void end();
	
	void draw(const Vector3& start, const Vector3& end);
	
	const BillboardBatch* getBatch() const;
	BillboardBatch* getBatch();
	
private:
	std::size_t lineCount;
	std::size_t currentLine;
	BillboardBatch batch;
	BillboardBatch::Range* range;
	float width;
	Vector4 color;
	PhysicalMaterial material;
};

inline const BillboardBatch* LineBatcher::getBatch() const
{
	return &batch;
}

inline BillboardBatch* LineBatcher::getBatch()
{
	return &batch;
}

#endif // DEBUG_HPP
