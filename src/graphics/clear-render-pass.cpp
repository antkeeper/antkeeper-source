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

#include "clear-render-pass.hpp"

ClearRenderPass::ClearRenderPass():
	clearColor(true),
	clearDepth(true),
	clearStencil(true),
	color(0.0f),
	depth(1.0f),
	index(0)
{}

bool ClearRenderPass::load(const RenderContext* renderContext)
{
	return true;
}

void ClearRenderPass::unload()
{}

void ClearRenderPass::render(RenderContext* renderContext)
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	
	GLbitfield mask = 0;
	if (clearColor)
	{
		mask |= GL_COLOR_BUFFER_BIT;
		glClearColor(color[0], color[1], color[2], color[3]);
	}
	
	if (clearDepth)
	{
		mask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(depth);
	}
	
	if (clearStencil)
	{
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(index);
	}
	
	glClear(mask);
}

void ClearRenderPass::setClear(bool color, bool depth, bool stencil)
{
	clearColor = color;
	clearDepth = depth;
	clearStencil = stencil;
}

void ClearRenderPass::setClearColor(const Vector4& color)
{
	this->color = color;
}

void ClearRenderPass::setClearDepth(float depth)
{
	this->depth = depth;
}

void ClearRenderPass::setClearStencil(int index)
{
	this->index = index;
}

