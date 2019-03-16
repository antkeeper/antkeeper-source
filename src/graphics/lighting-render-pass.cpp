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

#include "lighting-render-pass.hpp"
#include "resources/resource-manager.hpp"
#include "shadow-map-render-pass.hpp"

const std::uint64_t MATERIAL_FLAG_RIGGED                 = 0x0000000001;
const std::uint64_t MATERIAL_FLAG_TRANSLUCENT            = 0x0000000002;
const std::uint64_t MATERIAL_FLAG_DISABLE_SHADOW_CASTING = 0x0000000004;

LightingRenderPass::LightingRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr)
{}

bool LightingRenderPass::load(const RenderContext* renderContext)
{
	// Setup constants
	maxBoneCount = 64;
	frustumSplitCount = 4;
	wireframeLineWidth = 0.0f;

	// Create bias matrix
	biasMatrix = glm::translate(Vector3(0.5f)) * glm::scale(Vector3(0.5f));
	
	// Setup permutation values
	unskinnedPermutation = 0;
	skinnedPermutation = 1;
	
	// Load shader
	shader = resourceManager->load<Shader>("lighting.glsl");
	
	// Generate unskinned and skinned permutations
	if (!shader->generatePermutation(unskinnedPermutation) || !shader->generatePermutation(skinnedPermutation))
	{
		std::cerr << std::string("LightingRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}

	// Allocate array shader parameters
	matrixPaletteParam = new ShaderMatrix4(maxBoneCount);
	lightViewProjectionMatricesParam = new ShaderMatrix4(frustumSplitCount);

	// Connect shader parameters
	modelMatrixParam.connect(shader->getInput("modelMatrix"));
	modelViewMatrixParam.connect(shader->getInput("modelViewMatrix"));
	modelViewProjectionMatrixParam.connect(shader->getInput("modelViewProjectionMatrix"));
	normalModelMatrixParam.connect(shader->getInput("normalModelMatrix"));
	normalModelViewMatrixParam.connect(shader->getInput("normalModelViewMatrix"));
	matrixPaletteParam->connect(shader->getInput("matrixPalette"));
	lightViewProjectionMatricesParam->connect(shader->getInput("lightViewProjectionMatrices"));
	splitDistancesParam.connect(shader->getInput("splitDistances"));
	shadowMapParam.connect(shader->getInput("shadowMap"));
	cameraPositionParam.connect(shader->getInput("cameraPosition"));
	timeParam.connect(shader->getInput("time"));
	directionalLightCountParam.connect(shader->getInput("directionalLightCount"));
	directionalLightColorsParam.connect(shader->getInput("directionalLightColors"));
	directionalLightDirectionsParam.connect(shader->getInput("directionalLightDirections"));
	spotlightCountParam.connect(shader->getInput("spotlightCount"));
	spotlightColorsParam.connect(shader->getInput("spotlightColors"));
	spotlightPositionsParam.connect(shader->getInput("spotlightPositions"));
	spotlightAttenuationsParam.connect(shader->getInput("spotlightAttenuations"));
	spotlightDirectionsParam.connect(shader->getInput("spotlightDirections"));
	spotlightCutoffsParam.connect(shader->getInput("spotlightCutoffs"));
	spotlightExponentsParam.connect(shader->getInput("spotlightExponents"));

	clipNearParam.connect(shader->getInput("clipNear"));
	clipFarParam.connect(shader->getInput("clipFar"));

	#if defined(DEBUG)
		wireframeLineWidthParam.connect(shader->getInput("wireframeLineWidth"));
	#endif

	// Load tree shadow
	time = 0.0f;
	
	return true;
}

void LightingRenderPass::unload()
{
}

void LightingRenderPass::render(RenderContext* renderContext)
{
	// Get camera parameters
	const Camera& camera = *(renderContext->camera);
	const Matrix4& view = camera.getViewTween()->getSubstate();
	const Matrix4& projection = camera.getProjectionTween()->getSubstate();
	const Vector3& cameraPosition = camera.getTransformTween()->getSubstate().translation;

	// Gather lights
	const std::list<SceneObject*>* lights = renderContext->scene->getObjects(SceneObjectType::LIGHT);

	// Gather ambient cubes
	AmbientCube* ambientCube = nullptr;
	for (SceneObject* object: *lights)
	{
		Light* light = reinterpret_cast<Light*>(object);
		if (light->getLightType() == LightType::AMBIENT_CUBE)
		{
			ambientCube = reinterpret_cast<AmbientCube*>(light);
			break;
		}
	}

	// Gather directional lights
	const int MAX_DIRECTIONAL_LIGHT_COUNT = 3;
	int directionalLightCount = 0;
	Vector3 directionalLightColors[MAX_DIRECTIONAL_LIGHT_COUNT];
	Vector3 directionalLightDirections[MAX_DIRECTIONAL_LIGHT_COUNT];
	if (lights != nullptr)
	{
		for (auto object: *lights)
		{
			const Light* light = static_cast<const Light*>(object);
			LightType lightType = light->getLightType();
			
			if (lightType == LightType::DIRECTIONAL && light->isActive())
			{
				const DirectionalLight* directionalLight = static_cast<const DirectionalLight*>(light);
				
				directionalLightColors[directionalLightCount] = directionalLight->getColorTween()->getSubstate() * directionalLight->getIntensityTween()->getSubstate();
				directionalLightDirections[directionalLightCount] = glm::normalize(Vector3(view * Vector4(-directionalLight->getDirectionTween()->getSubstate(), 0.0f)));
				++directionalLightCount;
			}
		}
	}

	directionalLightCountParam.setValue(directionalLightCount);
	directionalLightColorsParam.setValues(0, directionalLightColors, directionalLightCount);
	directionalLightDirectionsParam.setValues(0, directionalLightDirections, directionalLightCount);

	// Gather spotlights
	const int MAX_SPOTLIGHT_COUNT = 3;
	int spotlightCount = 0;
	Vector3 spotlightColors[MAX_SPOTLIGHT_COUNT];
	Vector3 spotlightPositions[MAX_SPOTLIGHT_COUNT];
	Vector3 spotlightAttenuations[MAX_SPOTLIGHT_COUNT];
	Vector3 spotlightDirections[MAX_SPOTLIGHT_COUNT];
	float spotlightCutoffs[MAX_SPOTLIGHT_COUNT];
	float spotlightExponents[MAX_SPOTLIGHT_COUNT];
	if (lights != nullptr)
	{
		for (auto object: *lights)
		{
			const Light* light = static_cast<const Light*>(object);
			LightType lightType = light->getLightType();
			
			if (lightType == LightType::SPOTLIGHT && light->isActive())
			{
				const Spotlight* spotlight = static_cast<const Spotlight*>(light);
				
				spotlightColors[spotlightCount] = spotlight->getColorTween()->getSubstate() * spotlight->getIntensityTween()->getSubstate();
				spotlightPositions[spotlightCount] = Vector3(view * Vector4(spotlight->getTransformTween()->getSubstate().translation, 1.0f));
				spotlightAttenuations[spotlightCount] = spotlight->getAttenuationTween()->getSubstate();
				spotlightDirections[spotlightCount] = glm::normalize(Vector3(view * Vector4(-spotlight->getDirectionTween()->getSubstate(), 0.0f)));
				spotlightCutoffs[spotlightCount] = spotlight->getCutoffTween()->getSubstate();
				spotlightExponents[spotlightCount] = spotlight->getExponentTween()->getSubstate();
				
				++spotlightCount;
			}
		}
	}

	spotlightCountParam.setValue(spotlightCount);
	spotlightColorsParam.setValues(0, spotlightColors, spotlightCount);
	spotlightPositionsParam.setValues(0, spotlightPositions, spotlightCount);
	spotlightAttenuationsParam.setValues(0, spotlightAttenuations, spotlightCount);
	spotlightDirectionsParam.setValues(0, spotlightDirections, spotlightCount);
	spotlightCutoffsParam.setValues(0, spotlightCutoffs, spotlightCount);
	spotlightExponentsParam.setValues(0, spotlightExponents, spotlightCount);

	#if defined(DEBUG)
		wireframeLineWidthParam.setValue(wireframeLineWidth);
	#endif

	std::list<RenderOperation>* operations = renderContext->queue->getOperations();

	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Disable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);	
	
	// Load shadow map pass parameters
	Vector4 splitDistances;
	for (int i = 0; i < 4; ++i)
	{
		splitDistances[i] = shadowMapPass->getSplitViewFrustum().getSplitDistance(i + 1);
	}

	// Calculate the (light-space) view-projection matrices
	Matrix4 lightViewProjectionMatrices[4];
	for (int i = 0; i < 4; ++i)
	{
		lightViewProjectionMatrices[i] = shadowMapPass->getTileMatrix(i) * biasMatrix * shadowMapPass->getCropMatrix(i) * shadowMapPass->getLightCamera()->getViewProjection();
	}

	// Set shader parameter values
	lightViewProjectionMatricesParam->setValues(0, &lightViewProjectionMatrices[0], 4);
	splitDistancesParam.setValue(splitDistances);
	cameraPositionParam.setValue(cameraPosition);
	shadowMapParam.setValue(shadowMap);
	timeParam.setValue(time);
	clipNearParam.setValue(camera.getClipNear());
	clipFarParam.setValue(camera.getClipFar());

	std::uint32_t permutation = 0xDEADBEEF;
	bool blending = false;
	GLuint boundVAO = 0;
	
	// Sort operations
	operations->sort(RenderOpCompare());

	// Render operations
	for (const RenderOperation& operation: *operations)
	{
		// Skip operations without materials
		if (!operation.material)
		{
			continue;
		}
		
		bool hasTranslucency = operation.material->getFlags() & MATERIAL_FLAG_TRANSLUCENT;
		if (hasTranslucency && !blending)
		{
			glEnable(GL_BLEND);
			blending = true;
		}
		
		// Select permutation
		std::uint32_t targetPermutation = (operation.pose != nullptr) ? skinnedPermutation : unskinnedPermutation;
		if (permutation != targetPermutation)
		{
			permutation = targetPermutation;
			shader->activate(permutation);
		
			// Pass static params
			lightViewProjectionMatricesParam->upload();
			splitDistancesParam.upload();
			cameraPositionParam.upload();
			shadowMapParam.upload();
			timeParam.upload();

			directionalLightCountParam.upload();
			if (directionalLightCount > 0)
			{
				directionalLightColorsParam.upload();
				directionalLightDirectionsParam.upload();
			}

			spotlightCountParam.upload();
			if (spotlightCount > 0)
			{
				spotlightColorsParam.upload();
				spotlightPositionsParam.upload();
				spotlightAttenuationsParam.upload();
				spotlightDirectionsParam.upload();
				spotlightCutoffsParam.upload();
				spotlightExponentsParam.upload();
			}

			clipNearParam.upload();
			clipFarParam.upload();

			#if defined(DEBUG)
				wireframeLineWidthParam.upload();
			#endif
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewMatrix = view * modelMatrix;
		Matrix4 modelViewProjectionMatrix = projection * modelViewMatrix;	
		Matrix3 normalModelViewMatrix = glm::transpose(glm::inverse(Matrix3(modelViewMatrix)));
		Matrix3 normalModelMatrix = glm::transpose(glm::inverse(Matrix3(modelMatrix)));
		
		modelMatrixParam.setValue(modelMatrix);
		modelViewMatrixParam.setValue(modelViewMatrix);
		modelViewProjectionMatrixParam.setValue(modelViewProjectionMatrix);
		normalModelViewMatrixParam.setValue(normalModelViewMatrix);
		normalModelMatrixParam.setValue(normalModelMatrix);
		
		// Upload matrix parameters
		modelMatrixParam.upload();
		modelViewMatrixParam.upload();
		modelViewProjectionMatrixParam.upload();
		normalModelViewMatrixParam.upload();
		normalModelMatrixParam.upload();
		
		// Upload pose matrix palette
		if (operation.pose != nullptr && matrixPaletteParam->isConnected())
		{
			matrixPaletteParam->getConnectedInput()->upload(0, operation.pose->getMatrixPalette(), operation.pose->getSkeleton()->getBoneCount());
		}
		
		// Upload material parameters
		if (operation.material != nullptr)
		{
			if (operation.material->getShader() != shader)
			{
				((Material*)operation.material)->setShader(shader);
			}

			operation.material->upload();
		}
		
		if (boundVAO != operation.vao)
		{
			glBindVertexArray(operation.vao);
			boundVAO = operation.vao;
		}
		
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

bool LightingRenderPass::RenderOpCompare::operator()(const RenderOperation& opA, const RenderOperation& opB) const
{
	if (!opA.material)
		return false;
	else if (!opB.material)
		return true;
	
	// Determine transparency
	bool transparentA = opA.material->getFlags() & MATERIAL_FLAG_TRANSLUCENT;
	bool transparentB = opB.material->getFlags() & MATERIAL_FLAG_TRANSLUCENT;
	
	if (transparentA)
	{
		if (transparentB)
		{
			// A and B are both transparent, render back to front
			return (opA.depth >= opB.depth);
		}
		else
		{
			// A is transparent, B is opaque. Render B first
			return false;
		}
	}
	else
	{
		if (transparentB)
		{

			// A is opaque, B is transparent. Render A first
			return true;
		}
		else
		{
			// A and B are both opaque
			if (opA.material->getShader() == opB.material->getShader())
			{
				// A and B have the same shader
				if (opA.vao == opB.vao)
				{
					// A and B have the same VAO, sort by depth
					return (opA.depth < opB.depth);
				}
				else
				{
					// Sort by VAO
					return (opA.vao < opB.vao);
				}
			}
		}
	}
	
	// A and B are both opaque and have different shaders, sort by shader
	return (opA.material->getShader() < opB.material->getShader());
}

