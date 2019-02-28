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

#include "silhouette-render-pass.hpp"
#include "resources/resource-manager.hpp"

SilhouetteRenderPass::SilhouetteRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr)
{}

bool SilhouetteRenderPass::load(const RenderContext* renderContext)
{
	// Load shader
	shader = resourceManager->load<Shader>("silhouette.glsl");
	
	// Setup permutation values
	unskinnedPermutation = 0;
	skinnedPermutation = 1;
	
	// Generate unskinned and skinned permutations
	if (!shader->generatePermutation(unskinnedPermutation) || !shader->generatePermutation(skinnedPermutation))
	{
		std::cerr << std::string("SilhouetteRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}

	maxBoneCount = 64;
	matrixPaletteParam = new ShaderMatrix4(maxBoneCount);

	// Connect shader parameters
	matrixPaletteParam->connect(shader->getInput("matrixPalette"));
	modelViewProjectionMatrixParam.connect(shader->getInput("modelViewProjectionMatrix"));
	
	return true;
}

void SilhouetteRenderPass::unload()
{

	// Disconnect shader parameters
	matrixPaletteParam->disconnect();
	modelViewProjectionMatrixParam.disconnect();

	delete matrixPaletteParam;

	// Delete shader permutations
	shader->deleteAllPermutations();
}

void SilhouetteRenderPass::render(RenderContext* renderContext)
{
	const Camera& camera = *(renderContext->camera);

	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);

	// Disable depth testing and writing
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	// Disable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable alpha blending
	glDisable(GL_BLEND);

	// Get view-projection matrix
	const Matrix4& viewProjection = renderContext->camera->getViewProjectionTween()->getSubstate();

	// Filter render operations
	std::list<RenderOperation> operations;
	for (const RenderOperation& operation: *renderContext->queue->getOperations())
	{
		// Only use operations with the outline material.
		if (operation.material != nullptr)
		{
			if (operation.material->getFlags() & 256)
			{
				operations.push_back(operation);
			}
		}
	}

	// Sort render operations
	operations.sort(RenderOpCompare());

	std::uint32_t permutation = 0xDEADBEEF;
	GLuint boundVAO = 0;

	for (const RenderOperation& operation: operations)
	{
		std::uint32_t targetPermutation = (operation.pose != nullptr) ? skinnedPermutation : unskinnedPermutation;
		if (permutation != targetPermutation)
		{
			permutation = targetPermutation;
			shader->activate(permutation);
		}


		Matrix4 modelViewProjectionMatrix = viewProjection * operation.transform;	

		modelViewProjectionMatrixParam.setValue(modelViewProjectionMatrix);
		modelViewProjectionMatrixParam.upload();

		// Pass matrix palette
		if (operation.pose != nullptr)
		{
			matrixPaletteParam->getConnectedInput()->upload(0, operation.pose->getMatrixPalette(), operation.pose->getSkeleton()->getBoneCount());
		}

		if (boundVAO != operation.vao)
		{
			glBindVertexArray(operation.vao);
			boundVAO = operation.vao;
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

bool SilhouetteRenderPass::RenderOpCompare::operator()(const RenderOperation& opA, const RenderOperation& opB) const
{
	if (opA.pose)
	{
		if (opB.pose)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

