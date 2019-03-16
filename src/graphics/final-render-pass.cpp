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

#include "final-render-pass.hpp"
#include "resources/resource-manager.hpp"

FinalRenderPass::FinalRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr)
{}

bool FinalRenderPass::load(const RenderContext* renderContext)
{
	const float quadVertexData[] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	const std::uint32_t quadIndexData[] =
	{
		0, 1, 3,
		3, 1, 2
	};

	quadVertexCount = 4;
	quadIndexCount = 6;
	
	// Create quad geometry
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * quadVertexCount, quadVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(EMERGENT_VERTEX_POSITION);
	glVertexAttribPointer(EMERGENT_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (char*)0 + 0*sizeof(float));
	glGenBuffers(1, &quadIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * quadIndexCount, quadIndexData, GL_STATIC_DRAW);

	// Load shader
	shader = resourceManager->load<Shader>("final.glsl");
	
	// Generate shader permutation
	if (!shader->generatePermutation(0))
	{
		std::cerr << std::string("FinalRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}

	// Connect shader parameters
	silhouetteTextureParam.connect(shader->getInput("silhouetteTexture"));
	
	return true;
}

void FinalRenderPass::unload()
{
	// Free quad geometry
	glDeleteBuffers(1, &quadIBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	silhouetteTextureParam.disconnect();

	shader->deleteAllPermutations();
}

void FinalRenderPass::render(RenderContext* renderContext)
{
	const Camera& camera = *(renderContext->camera);

	// Determine scissor box coordinates using AABBs projected onto screen coordinates
	for (const RenderOperation& operation: *renderContext->queue->getOperations())
	{
		// Only use operations with the outline material.
		if (operation.material != nullptr)
		{
			if (operation.material->getFlags() & 256)
			{

			}
		}
	}

	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);

	//glScissor(0,0,renderTarget->width * 0.5f,renderTarget->height);
	//glEnable(GL_SCISSOR_TEST);

	// Disable depth testing and writing
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable alpha blending
	glDisable(GL_BLEND);

	// Bind shader
	shader->activate(0);

	// Upload shader parameters
	silhouetteTextureParam.setValue(&silhouetteTexture);
	silhouetteTextureParam.upload();

	// Draw quad
	glBindVertexArray(quadVAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT,	(void*)0, 0);

	//glDisable(GL_SCISSOR_TEST);
}

void FinalRenderPass::setSilhouetteRenderTarget(const RenderTarget* renderTarget)
{
	silhouetteRenderTarget = renderTarget;
	silhouetteTexture.setWidth(renderTarget->width);
	silhouetteTexture.setHeight(renderTarget->height);
	silhouetteTexture.setTextureID(renderTarget->texture);
}
