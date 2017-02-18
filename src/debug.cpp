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

#include "debug.hpp"
#include <iostream>

LineBatcher::LineBatcher(std::size_t lineCount):
	lineCount(lineCount),
	currentLine(0),
	width(1.0f),
	color(1.0f)
{
	batch.resize(lineCount);
	range = batch.addRange();
	range->material = &material;
	
	material.albedo = Vector3(1.0f);
}

void LineBatcher::begin()
{
	currentLine = 0;
	range->start = 0;
	range->length = 0;
}

void LineBatcher::end()
{
	range->length = currentLine;
	batch.update();
}

void LineBatcher::draw(const Vector3& start, const Vector3& end)
{
	if (currentLine >= batch.getBillboardCount())
	{
		std::cout << "LineBatcher::draw(): maximum line count exceeded" << std::endl;
		return;
	}
	
	Vector3 center = (start + end) * 0.5f;
	float length = glm::length(end - start);
	
	Vector3 forward = glm::normalize(end - start);
	glm::quat rotation = glm::normalize(glm::rotation(Vector3(1, 0, 0), forward));
	
	Billboard* billboard = batch.getBillboard(currentLine);
	billboard->setTranslation(center);
	billboard->setDimensions(Vector2(length, width));
	billboard->setRotation(rotation);
	billboard->setTintColor(color);
	
	++currentLine;
}

void LineBatcher::setWidth(float width)
{
	this->width = width;
}
	
void LineBatcher::setColor(const Vector4& color)
{
	this->color = color;
}
