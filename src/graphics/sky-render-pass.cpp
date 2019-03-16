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

#include "sky-render-pass.hpp"
#include "resources/resource-manager.hpp"

SkyRenderPass::SkyRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr)
{}

bool SkyRenderPass::load(const RenderContext* renderContext)
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

	// Load sky shader
	shader = resourceManager->load<Shader>("sky.glsl");
	
	// Generate shader permutation
	if (!shader->generatePermutation(0))
	{
		std::cerr << std::string("SkyRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	matrixParam.connect(shader->getInput("matrix"));
	sunDirectionParam.connect(shader->getInput("sunDirection"));
	sunAngularRadiusParam.connect(shader->getInput("sunAngularRadius"));
	skyGradientParam.connect(shader->getInput("skyGradient"));

	// Load sky gradient texture
	skyGradientTexture = resourceManager->load<Texture2D>("sky-gradient-noon.png");

	// Set sun angular radius
	sunAngularRadius = glm::radians(2.0f);
	
	return true;
}

void SkyRenderPass::unload()
{
	// Free quad geometry
	glDeleteBuffers(1, &quadIBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);

	matrixParam.disconnect();
	sunDirectionParam.disconnect();
	sunAngularRadiusParam.disconnect();
	skyGradientParam.disconnect();
	shader->deleteAllPermutations();
}

void SkyRenderPass::render(RenderContext* renderContext)
{
	const Camera& camera = *(renderContext->camera);

	// Get sun light
	const std::list<SceneObject*>* lights = renderContext->scene->getObjects(SceneObjectType::LIGHT);
	DirectionalLight* sun = nullptr;
	for (SceneObject* object: *lights)
	{
		Light* light = reinterpret_cast<Light*>(object);
		if (light->getLightType() == LightType::DIRECTIONAL)
		{
			sun = reinterpret_cast<DirectionalLight*>(light);
			break;
		}
	}
	
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);

	// Disable depth testing and writing
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable alpha blending
	glDisable(GL_BLEND);

	// Calculate matrix
	Matrix4 modelView = Matrix4(Matrix3(camera.getViewTween()->getSubstate()));
	const Matrix4& inverseProjection = camera.getInverseProjectionTween()->getSubstate();
	Matrix4 matrix = glm::inverse(modelView) * inverseProjection;

	// Get sun direction
	Vector3 sunDirection = Vector3(0, 0, 1);
	if (sun)
	{
		sunDirection = glm::normalize(sun->getDirectionTween()->getSubstate());
	}

	// Bind shader
	shader->activate(0);

	// Set shader param values
	matrixParam.setValue(matrix);
	sunDirectionParam.setValue(sunDirection);
	sunAngularRadiusParam.setValue(sunAngularRadius);
	skyGradientParam.setValue(skyGradientTexture);

	// Upload shader params
	matrixParam.upload();
	sunDirectionParam.upload();
	sunAngularRadiusParam.upload();
	skyGradientParam.upload();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
	// Draw quad
	glBindVertexArray(quadVAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT,	(void*)0, 0);
}

