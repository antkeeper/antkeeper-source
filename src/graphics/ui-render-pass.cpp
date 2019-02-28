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

#include "ui-render-pass.hpp"
#include "ui/ui.hpp"
#include "resources/resource-manager.hpp"

UIRenderPass::UIRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr)
{}

bool UIRenderPass::load(const RenderContext* renderContext)
{
	shader = resourceManager->load<Shader>("ui.glsl");
	
	// Set permutation values
	int gammaFlag = 0x2;
	untexturedPermutation = 0 | gammaFlag;
	texturedPermutation = 1 | gammaFlag;
	
	// Generate shader permutations
	if (!shader->generatePermutation(untexturedPermutation) || !shader->generatePermutation(texturedPermutation))
	{
		std::cerr << std::string("UIRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	modelViewProjectionMatrixParam.connect(shader->getInput("modelViewProjectionMatrix"));
	textureParam.connect(shader->getInput("tex"));
	textureOffsetParam.connect(shader->getInput("texcoordOffset"));
	textureScaleParam.connect(shader->getInput("texcoordScale"));
	if (!modelViewProjectionMatrixParam.isConnected() ||
		!textureParam.isConnected() ||
		!textureOffsetParam.isConnected() ||
		!textureScaleParam.isConnected())
	{
		std::cerr << std::string("UIRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
		return false;
	}
	
	return true;
}

void UIRenderPass::unload()
{
	modelViewProjectionMatrixParam.disconnect();
	textureParam.disconnect();
	textureOffsetParam.disconnect();
	textureScaleParam.disconnect();
	shader->deleteAllPermutations();
}

void UIRenderPass::render(RenderContext* renderContext)
{
	const Camera& camera = *(renderContext->camera);
	const Matrix4& viewProjection = camera.getViewProjectionTween()->getSubstate();
	
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	//glDepthFunc(GL_LESS);
	
	// Disable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 

	// Render operations
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	for (const RenderOperation& operation: *operations)
	{
		// Skip render operations with unsupported materials
		/*
		if (operation.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::UI))
		{
			continue;
		}
		*/
		
		const UIMaterial* material = static_cast<const UIMaterial*>(operation.material);
		
		if (material->texture->getValue() != nullptr)
		{
			shader->activate(texturedPermutation);
			
			textureParam.setValue(material->texture->getValue());
			textureOffsetParam.setValue(material->textureOffset->getValue());
			textureScaleParam.setValue(material->textureScale->getValue());
			
			textureParam.upload();
			textureOffsetParam.upload();
			textureScaleParam.upload();
		}
		else
		{
			shader->activate(untexturedPermutation);
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = viewProjection * modelMatrix;
		
		// Upload matrix parameters
		modelViewProjectionMatrixParam.setValue(modelViewProjectionMatrix);
		modelViewProjectionMatrixParam.upload();
		
		// Upload material parameters
		//operation.material->upload();
		
		// Draw geometry
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

