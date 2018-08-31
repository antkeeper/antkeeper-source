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

#include "render-passes.hpp"
#include "configuration.hpp"
#include "ui/ui.hpp"
#include <iostream>
#include <limits>

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

BlurRenderPass::BlurRenderPass():
	gammaCorrect(false)
{}

bool BlurRenderPass::load(const RenderContext* renderContext)
{
	permutation = (gammaCorrect) ? 1 : 0;
	
	// Load shader source
	if (!shader.loadSource("data/shaders/blur.glsl"))
	{
		std::cerr << std::string("BlurRenderPass: failed to load shader source.") << std::endl;
		return false;
	}
	
	// Generate permutation
	if (!shader.generatePermutation(permutation))
	{
		std::cerr << std::string("BlurRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	textureParam.connect(shader.getInput("blurTexture"));
	resolutionParam.connect(shader.getInput("resolution"));
	directionParam.connect(shader.getInput("direction"));
	if (!textureParam.isConnected() ||
		!resolutionParam.isConnected() ||
		!directionParam.isConnected())
	{
		std::cerr << std::string("BlurRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
		return false;
	}
	
	// Initialize shader variables
	resolutionParam.setValue(Vector2(renderTarget->width, renderTarget->height));
	
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
	
	// Create AABB geometry
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
	
	return true;
}

void BlurRenderPass::unload()
{
	textureParam.disconnect();
	resolutionParam.disconnect();
	directionParam.disconnect();
	shader.deleteAllPermutations();
	
	glDeleteBuffers(1, &quadIBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
}

void BlurRenderPass::render(RenderContext* renderContext)
{
	// Bind framebuffer and set up viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Set up OpenGL state
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	
	// Activate shader permutation
	shader.activate(permutation);
	
	// Upload shader variables to shader permutation
	textureParam.upload();
	resolutionParam.upload();
	directionParam.upload();
	
	// Render quad
	glBindVertexArray(quadVAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT, (void*)0, 0);
	
	// Unbind texture
	//glBindTexture(GL_TEXTURE_2D, 0);
}

ShadowMapRenderPass::ShadowMapRenderPass():
	croppedShadowMapViewports(nullptr),
	viewCamera(nullptr),
	splitViewFrustum(nullptr),
	cropMatrices(nullptr),
	tileMatrices(nullptr)
{}

bool ShadowMapRenderPass::load(const RenderContext* renderContext)
{
	// Set maximum number of bones for skinned meshes
	maxBoneCount = 64;
	
	// Create split view frustum
	splitViewFrustum = new SplitViewFrustum(4);
	splitViewFrustum->setSplitSchemeWeight(0.85f);
	
	// Determine resolution of shadow maps
	shadowMapResolution = 4096;
	croppedShadowMapResolution = shadowMapResolution >> 1;
	
	// Allocate viewports
	croppedShadowMapViewports = new Vector4[splitViewFrustum->getSubfrustumCount()];
	
	// Setup viewports
	for (int i = 0; i < splitViewFrustum->getSubfrustumCount(); ++i)
	{
		int x = i % 2;
		int y = i / 2;
		
		Vector4* viewport = &croppedShadowMapViewports[i];
		(*viewport)[0] = static_cast<float>(x * croppedShadowMapResolution);
		(*viewport)[1] = static_cast<float>(y * croppedShadowMapResolution);
		(*viewport)[2] = static_cast<float>(croppedShadowMapResolution);
		(*viewport)[3] = static_cast<float>(croppedShadowMapResolution);
	}
	
	// Allocate matrices
	cropMatrices = new Matrix4[splitViewFrustum->getSubfrustumCount()];
	tileMatrices = new Matrix4[splitViewFrustum->getSubfrustumCount()];
	
	// Setup tile matrices
	Matrix4 tileScale = glm::scale(Vector3(0.5f, 0.5f, 1.0f));
	for (int i = 0; i < splitViewFrustum->getSubfrustumCount(); ++i)
	{
		float x = static_cast<float>(i % 2) * 0.5f;
		float y = static_cast<float>(i / 2) * 0.5f;
		tileMatrices[i] = glm::translate(Vector3(x, y, 0.0f)) * tileScale;
	}
	
	// Setup permutation values
	unskinnedPermutation = 0;
	skinnedPermutation = 1;
	
	// Load shader source
	if (!shader.loadSource("data/shaders/depth-pass.glsl"))
	{
		std::cerr << std::string("ShadowMapRenderPass: failed to load shader source.") << std::endl;
		return false;
	}
	
	// Generate unskinned and skinned permutations
	if (!shader.generatePermutation(unskinnedPermutation) || !shader.generatePermutation(skinnedPermutation))
	{
		std::cerr << std::string("ShadowMapRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Allocate bone palette parameter
	matrixPaletteParam = new ShaderMatrix4(maxBoneCount);
	
	// Connect shader variables
	modelViewProjectionParam.connect(shader.getInput("modelViewProjectionMatrix"));
	matrixPaletteParam->connect(shader.getInput("matrixPalette"));
	if (!modelViewProjectionParam.isConnected() ||
		!matrixPaletteParam->isConnected())
	{
		std::cerr << std::string("ShadowMapRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
		return false;
	}
	
	return true;
}

void ShadowMapRenderPass::unload()
{
	modelViewProjectionParam.disconnect();
	matrixPaletteParam->disconnect();
	delete matrixPaletteParam;
	shader.deleteAllPermutations();
	
	delete[] croppedShadowMapViewports;
	croppedShadowMapViewports = nullptr;
	
	delete splitViewFrustum;
	splitViewFrustum = nullptr;
	
	delete[] cropMatrices;
	cropMatrices = nullptr;
	
	delete[] tileMatrices;
	tileMatrices = nullptr;
}

void ShadowMapRenderPass::render(RenderContext* renderContext)
{
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
	// Clear the framebuffer depth
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Draw front and back faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable alpha blending
	glDisable(GL_BLEND);

	//const Camera& lightCamera = *(renderContext->camera);
	std::list<RenderOperation>* operations = renderContext->queue->getOperations();	
	
	GLuint boundVAO = 0;
	
	splitViewFrustum->setMatrices(viewCamera->getView(), viewCamera->getProjection());
	
	// Sort operations
	operations->sort(RenderOpCompare());
	
	std::uint32_t permutation = 0xDEADBEEF;
	
	// For each frustum split
	for (int i = 0; i < splitViewFrustum->getSubfrustumCount(); ++i)
	{
		// Calculate crop matrix
		{
			const ViewFrustum& subfrustum = splitViewFrustum->getSubfrustum(i);
			
			// Create AABB containing the subfrustum corners			
			AABB subfrustumBounds(subfrustum.getCorner(0), subfrustum.getCorner(0));
			for (std::size_t j = 1; j < 8; ++j)
			{
				subfrustumBounds.add(subfrustum.getCorner(j));
			}

			// Transform subfrustum bounds into light's clip space
			AABB croppingBounds = subfrustumBounds.transformed(lightCamera->getViewProjection());
			Vector3 cropMax = croppingBounds.getMax();
			Vector3 cropMin = croppingBounds.getMin();
			
			// Calculate scale
			Vector3 scale;
			scale.x = 2.0f / (cropMax.x - cropMin.x);
			scale.y = 2.0f / (cropMax.y - cropMin.y);
			scale.z = 1.0f / (cropMax.z - cropMin.z);
			
			// Quantize scale
			float scaleQuantizer = 64.0f;
			scale.x = 1.0f / std::ceil(1.0f / scale.x * scaleQuantizer) * scaleQuantizer;
			scale.y = 1.0f / std::ceil(1.0f / scale.y * scaleQuantizer) * scaleQuantizer;
			
			// Calculate offset
			Vector3 offset;
			offset.x = (cropMax.x + cropMin.x) * scale.x * -0.5f;
			offset.y = (cropMax.y + cropMin.y) * scale.y * -0.5f;
			offset.z = -cropMin.z * scale.z;

			// Quantize offset
			float halfTextureSize = static_cast<float>(croppedShadowMapResolution) * 0.5f;
			offset.x = std::ceil(offset.x * halfTextureSize) / halfTextureSize;
			offset.y = std::ceil(offset.y * halfTextureSize) / halfTextureSize;
			
			cropMatrices[i] = glm::translate(offset) * glm::scale(scale);
		}
		
		Matrix4 croppedViewProjection = cropMatrices[i] * lightCamera->getViewProjection();
		
		// Activate viewport for corresponding cropped shadow map
		const Vector4& viewport = croppedShadowMapViewports[i];
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Render operations
		for (const RenderOperation& operation: *operations)
		{
			// Skip operations with no materials and materials with no shadows
			if (operation.material == nullptr || (operation.material->getFlags() & MATERIAL_FLAG_DISABLE_SHADOW_CASTING))
			{
				continue;
			}
			
			// TODO: Perform culling for subfrustums
			
			// Select permutation
			std::uint32_t targetPermutation = (operation.pose != nullptr) ? skinnedPermutation : unskinnedPermutation;
			if (permutation != targetPermutation)
			{
				permutation = targetPermutation;
				shader.activate(permutation);
			}
		
			// Pass matrix palette
			if (operation.pose != nullptr)
			{
				matrixPaletteParam->getConnectedInput()->upload(0, operation.pose->getMatrixPalette(), operation.pose->getSkeleton()->getBoneCount());
			}
			
			const Matrix4& modelMatrix = operation.transform;
			Matrix4 modelViewProjectionMatrix = croppedViewProjection * modelMatrix;
			
			modelViewProjectionParam.setValue(modelViewProjectionMatrix);
			modelViewProjectionParam.upload();
			
			if (boundVAO != operation.vao)
			{
				glBindVertexArray(operation.vao);
				boundVAO = operation.vao;
			}
			
			glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
		}
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ShadowMapRenderPass::RenderOpCompare::operator()(const RenderOperation& opA, const RenderOperation& opB) const
{
	// If A is rigged
	if (opA.pose != nullptr)
	{
		// And B is rigged
		if (opB.pose != nullptr)
		{
			// Sort by VAO ID
			return (opA.vao <= opB.vao);
		}
		else
		{
			// Render A first
			return true;
		}
	}
	
	// Sort by VAO ID
	return (opA.vao <= opB.vao);
}

LightingRenderPass::LightingRenderPass():
	shadowMap(0),
	shadowCamera(nullptr),
	diffuseCubemap(nullptr),
	specularCubemap(nullptr),
	shadowMapPass(nullptr)
{
	// Initialize bias matrix for calculating the model-view-projection-bias matrix (used for shadow map texture coordinate calculation)
	biasMatrix = glm::translate(Vector3(0.5f)) * glm::scale(Vector3(0.5f));
	
	maxBoneCount = 64;
	maxDirectionalLightCount = 1;
	maxSpotlightCount = 1;
}

bool LightingRenderPass::load(const RenderContext* renderContext)
{
	// Load shaders for each material
	
	// Register parameter sets for each unique shader
	
	if (!shader.loadSource("data/shaders/standard.glsl"))
	{
		std::cerr << std::string("LightingRenderPass: Failed to load shader source.") << std::endl;
		return false;
	}
	
	/*
	if (renderContext != nullptr)
	{
		std::list<RenderOperation>* operations = renderContext->queue->getOperations();
		
		for (RenderOperation& op: *operations)
		{
			
		}
	}
	*/
	
	// Set permutation values
	std::uint32_t IS_SKINNED = 1;
	std::uint32_t HAS_AMBIENT_CUBE = 1 << 7;
	unskinnedPermutation = HAS_AMBIENT_CUBE;
	skinnedPermutation = IS_SKINNED | HAS_AMBIENT_CUBE;
	
	// Generate shader permutations
	if (!shader.generatePermutation(unskinnedPermutation) || !shader.generatePermutation(skinnedPermutation))
	{
		std::cerr << std::string("LightingRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Allocate shader array parameters
	parameters.matrixPalette = new ShaderMatrix4(maxBoneCount);
	parameters.lightViewProjectionMatrices = new ShaderMatrix4(4);
	
	// Connect shader parameters
	parameters.matrixPalette->connect(shader.getInput("matrixPalette"));
	parameters.modelMatrix.connect(shader.getInput("modelMatrix"));
	parameters.modelViewMatrix.connect(shader.getInput("modelViewMatrix"));
	parameters.modelViewProjectionMatrix.connect(shader.getInput("modelViewProjectionMatrix"));
	parameters.normalModelViewMatrix.connect(shader.getInput("normalModelViewMatrix"));
	parameters.normalModelMatrix.connect(shader.getInput("normalModelMatrix"));
	parameters.lightViewProjectionMatrices->connect(shader.getInput("lightViewProjectionMatrices"));
	parameters.splitDistances.connect(shader.getInput("splitDistances"));
	parameters.shadowMap.connect(shader.getInput("shadowMap"));
	parameters.cameraPosition.connect(shader.getInput("cameraPosition"));
	parameters.diffuseCubemap.connect(shader.getInput("diffuseCubemap"));
	parameters.specularCubemap.connect(shader.getInput("specularCubemap"));
	
	parameters.albedoOpacityMap.connect(shader.getInput("albedoOpacityMap"));
	parameters.metalnessRoughnessMap.connect(shader.getInput("metalnessRoughnessMap"));
	parameters.normalOcclusionMap.connect(shader.getInput("normalOcclusionMap"));
	
	if (!parameters.matrixPalette->isConnected() ||
		!parameters.modelMatrix.isConnected() ||
		!parameters.modelViewMatrix.isConnected() ||
		!parameters.modelViewProjectionMatrix.isConnected() ||
		!parameters.normalModelViewMatrix.isConnected() ||
		!parameters.normalModelMatrix.isConnected() ||
		!parameters.lightViewProjectionMatrices->isConnected() ||
		!parameters.splitDistances.isConnected() ||
		!parameters.shadowMap.isConnected() ||
		!parameters.cameraPosition.isConnected() ||
		!parameters.diffuseCubemap.isConnected() ||
		!parameters.specularCubemap.isConnected())
	{
		std::cerr << std::string("LightingRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
	}
	
	return true;
}

void LightingRenderPass::unload()
{
	// Free shader array parameters
	delete parameters.matrixPalette;
	delete parameters.lightViewProjectionMatrices;
	parameters.matrixPalette = nullptr;
	parameters.lightViewProjectionMatrices = nullptr;
	shader.deleteAllPermutations();
}

void LightingRenderPass::render(RenderContext* renderContext)
{	
	const Camera& camera = *(renderContext->camera);
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

	// Enable alpha blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);	
	
	
	Vector4 splitDistances;
	for (int i = 0; i < 4; ++i)
	{
		splitDistances[i] = shadowMapPass->getSplitViewFrustum().getSplitDistance(i + 1);
	}
	
	Vector3 directionalLightColors[3];
	Vector3 directionalLightDirections[3];
	
	Vector3 spotlightColors[3];
	Vector3 spotlightPositions[3];
	Vector3 spotlightAttenuations[3];
	Vector3 spotlightDirections[3];
	float spotlightCutoffs[3];
	float spotlightExponents[3];
	
	// Add directional light
	int directionalLightCount = 1;
	directionalLightColors[0] = Vector3(1);
	directionalLightDirections[0] = glm::normalize(Vector3(camera.getView() * -Vector4(0, -2, -1, 0)));
	
	// Add spotlights
	int spotlightCount = 0;
	const std::list<SceneObject*>* lights = renderContext->layer->getObjects(SceneObjectType::LIGHT);
	if (lights != nullptr)
	{
		for (auto object: *lights)
		{
			const Light* light = static_cast<const Light*>(object);
			LightType lightType = light->getLightType();
			
			if (lightType == LightType::SPOTLIGHT && light->isActive())
			{
				const Spotlight* spotlight = static_cast<const Spotlight*>(light);
				
				spotlightColors[spotlightCount] = spotlight->getScaledColor();
				spotlightPositions[spotlightCount] = Vector3(camera.getView() * Vector4(spotlight->getTranslation(), 1.0f));
				spotlightAttenuations[spotlightCount] = spotlight->getAttenuation();
				spotlightDirections[spotlightCount] = glm::normalize(Vector3(camera.getView() * Vector4(-spotlight->getDirection(), 0.0f)));
				spotlightCutoffs[spotlightCount] = spotlight->getCutoff();
				spotlightExponents[spotlightCount] = spotlight->getExponent();
				
				++spotlightCount;
			}
		}
	}
	
	// Calculate the (light-space) view-projection matrices
	Matrix4 lightViewProjectionMatrices[4];
	for (int i = 0; i < 4; ++i)
	{
		lightViewProjectionMatrices[i] = shadowMapPass->getTileMatrix(i) * biasMatrix * shadowMapPass->getCropMatrix(i) * shadowCamera->getViewProjection();
	}
	
	// Set shader parameter values
	parameters.lightViewProjectionMatrices->setValues(0, &lightViewProjectionMatrices[0], 4);
	parameters.splitDistances.setValue(splitDistances);
	parameters.cameraPosition.setValue(camera.getTranslation());
	parameters.diffuseCubemap.setValue(diffuseCubemap);
	parameters.specularCubemap.setValue(specularCubemap);
	parameters.shadowMap.setValue(shadowMap);
	parameters.directionalLightCount.setValue(0);
	parameters.spotlightCount.setValue(0);
	
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
			shader.activate(permutation);
		
			// Pass static params
			parameters.lightViewProjectionMatrices->upload();
			parameters.splitDistances.upload();
			parameters.cameraPosition.upload();
			parameters.diffuseCubemap.upload();
			parameters.specularCubemap.upload();
			parameters.shadowMap.upload();
			parameters.directionalLightCount.upload();
			parameters.spotlightCount.upload();
			
			/*
			shader->setParameter(directionalLightCountParam, directionalLightCount);
			shader->setParameter(directionalLightColorsParam, 0, &directionalLightColors[0], directionalLightCount);
			shader->setParameter(directionalLightDirectionsParam, 0, &directionalLightDirections[0], directionalLightCount);
			
			shader->setParameter(spotlightCountParam, spotlightCount);
			shader->setParameter(spotlightColorsParam, 0, &spotlightColors[0], spotlightCount);
			shader->setParameter(spotlightPositionsParam, 0, &spotlightPositions[0], spotlightCount);
			shader->setParameter(spotlightAttenuationsParam, 0, &spotlightAttenuations[0], spotlightCount);
			shader->setParameter(spotlightDirectionsParam, 0, &spotlightDirections[0], spotlightCount);
			shader->setParameter(spotlightCutoffsParam, 0, &spotlightCutoffs[0], spotlightCount);
			shader->setParameter(spotlightExponentsParam, 0, &spotlightExponents[0], spotlightCount);
			*/
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewMatrix = camera.getView() * modelMatrix;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;	
		Matrix3 normalModelViewMatrix = glm::transpose(glm::inverse(Matrix3(modelViewMatrix)));
		Matrix3 normalModelMatrix = glm::transpose(glm::inverse(Matrix3(modelMatrix)));
		
		parameters.modelMatrix.setValue(modelMatrix);
		parameters.modelViewMatrix.setValue(modelViewMatrix);
		parameters.modelViewProjectionMatrix.setValue(modelViewProjectionMatrix);
		parameters.normalModelViewMatrix.setValue(normalModelViewMatrix);
		parameters.normalModelMatrix.setValue(normalModelMatrix);
		
		// Upload matrix parameters
		parameters.modelMatrix.upload();
		parameters.modelViewMatrix.upload();
		parameters.modelViewProjectionMatrix.upload();
		parameters.normalModelViewMatrix.upload();
		parameters.normalModelMatrix.upload();
		
		// Upload pose matrix palette
		if (operation.pose != nullptr && parameters.matrixPalette->isConnected())
		{
			parameters.matrixPalette->getConnectedInput()->upload(0, operation.pose->getMatrixPalette(), operation.pose->getSkeleton()->getBoneCount());
		}
		
		// Upload material parameters
		if (operation.material != nullptr)
		{
			if (operation.material->getShader() != &shader)
			{
				((Material*)operation.material)->setShader(&shader);
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
	
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool LightingRenderPass::RenderOpCompare::operator()(const RenderOperation& opA, const RenderOperation& opB) const
{
	if (!opA.material)
		return false;
	else if (!opB.material);
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

DebugRenderPass::DebugRenderPass()
{}

bool DebugRenderPass::load(const RenderContext* renderContext)
{
	if (!shader.loadSource("data/shaders/unlit-solid.glsl"))
	{
		std::cerr << std::string("DebugRenderPass: Failed to load shader source.") << std::endl;
		return false;
	}
	
	// Set permutation values
	permutation = 0;
	
	// Generate shader permutations
	if (!shader.generatePermutation(permutation))
	{
		std::cerr << std::string("DebugRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	modelViewProjectionMatrixParam.connect(shader.getInput("modelViewProjectionMatrix"));
	if (!modelViewProjectionMatrixParam.isConnected())
	{
		std::cerr << std::string("DebugRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
		return false;
	}
	
	const float aabbVertexData[] =
	{
		-0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,
		 0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		-0.5, -0.5,  0.5,
		 0.5, -0.5,  0.5,
		 0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
	};

	const std::uint32_t aabbIndexData[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	aabbVertexCount = 8;
	aabbIndexCount = 24;
	
	// Create AABB geometry
	glGenVertexArrays(1, &aabbVAO);
	glBindVertexArray(aabbVAO);
	glGenBuffers(1, &aabbVBO);
	glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * aabbVertexCount, aabbVertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(EMERGENT_VERTEX_POSITION);
	glVertexAttribPointer(EMERGENT_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (char*)0 + 0*sizeof(float));
	glGenBuffers(1, &aabbIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * aabbIndexCount, aabbIndexData, GL_STATIC_DRAW);
	
	return true;
}

void DebugRenderPass::unload()
{
	modelViewProjectionMatrixParam.disconnect();
	shader.deleteAllPermutations();
	
	glDeleteBuffers(1, &aabbIBO);
	glDeleteBuffers(1, &aabbVBO);
	glDeleteVertexArrays(1, &aabbVAO);
}

void DebugRenderPass::render(RenderContext* renderContext)
{
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	const Camera& camera = *(renderContext->camera);

	/*
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Clear the framebuffer depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Disable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	*/
	
	// Disable backface culling
	glDisable(GL_CULL_FACE);
	
	// Disable alpha blending
	glDisable(GL_BLEND);
	
	// Bind unlit solid shader
	shader.activate(permutation);
	
	// Bind AABB geometry
	glBindVertexArray(aabbVAO);
	
	const std::list<SceneObject*>* objects = renderContext->layer->getObjects();
	for (auto object: *objects)
	{
		if (!camera.getCullingMask()->intersects(object->getBounds()))
			continue;
		
		const AABB& bounds = object->getBounds();
		const Vector3& min = bounds.getMin();
		const Vector3& max = bounds.getMax();
		
		Vector3 scale = max - min;
		Vector3 center = (min + max) * 0.5f;
		const Vector3& translation = center;
		Matrix4 modelMatrix = glm::translate(translation) * glm::scale(scale);
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
		
		modelViewProjectionMatrixParam.setValue(modelViewProjectionMatrix);
		modelViewProjectionMatrixParam.upload();
		
		glDrawElements(GL_LINES, aabbIndexCount, GL_UNSIGNED_INT, (void*)0);
	}
}

UIRenderPass::UIRenderPass()
{}

bool UIRenderPass::load(const RenderContext* renderContext)
{
	if (!shader.loadSource("data/shaders/ui.glsl"))
	{
		std::cerr << std::string("UIRenderPass: Failed to load shader source.") << std::endl;
		return false;
	}
	
	// Set permutation values
	untexturedPermutation = 0;
	texturedPermutation = 1;
	
	// Generate shader permutations
	if (!shader.generatePermutation(untexturedPermutation) || !shader.generatePermutation(texturedPermutation))
	{
		std::cerr << std::string("UIRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	modelViewProjectionMatrixParam.connect(shader.getInput("modelViewProjectionMatrix"));
	textureParam.connect(shader.getInput("tex"));
	textureOffsetParam.connect(shader.getInput("texcoordOffset"));
	textureScaleParam.connect(shader.getInput("texcoordScale"));
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
	shader.deleteAllPermutations();
}

void UIRenderPass::render(RenderContext* renderContext)
{
	const Camera& camera = *(renderContext->camera);
	
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
			shader.activate(texturedPermutation);
			
			textureParam.setValue(material->texture->getValue());
			textureOffsetParam.setValue(material->textureOffset->getValue());
			textureScaleParam.setValue(material->textureScale->getValue());
			
			textureParam.upload();
			textureOffsetParam.upload();
			textureScaleParam.upload();
		}
		else
		{
			shader.activate(untexturedPermutation);
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
		
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

/*
VignetteRenderPass::VignetteRenderPass():
	shader(nullptr)
{
	bayerTextureParam = parameterSet.addParameter("bayerTexture", ShaderParameter::Type::INT, 1);
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
}

bool VignetteRenderPass::load(const RenderContext* renderContext)
{
	shaderLoader.undefine();
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_COLOR", EMERGENT_VERTEX_COLOR);
	shaderLoader.define("TEXTURE_COUNT", 1);
	
	shader = shaderLoader.load("data/shaders/vignette.glsl", &parameterSet);
	if (!shader)
	{
		return false;
	}
	
	/// @see http://www.anisopteragames.com/how-to-fix-color-banding-with-dithering/
	static const char pattern[] =
	{
		 0, 32,  8, 40,  2, 34, 10, 42,
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
	};
	
	glGenTextures(1, &bayerTextureID);
	glBindTexture(GL_TEXTURE_2D, bayerTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, pattern);
	
	return true;
}

void VignetteRenderPass::unload()
{
	delete shader;
	shader = nullptr;
	
	glDeleteTextures(1, &bayerTextureID);
}

void VignetteRenderPass::render(RenderContext* renderContext)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	// Bind shader
	shader->bind();
	
	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bayerTextureID);
	
	// Pass texture unit to shader
	shader->setParameter(bayerTextureParam, 0);
	
	const Camera& camera = *(renderContext->camera);
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	
	// Render operations
	for (const RenderOperation& operation: *operations)
	{
		const Material* material = operation.material;
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
		shader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}
*/

SkyboxRenderPass::SkyboxRenderPass():
	cubemap(nullptr)
{}

bool SkyboxRenderPass::load(const RenderContext* renderContext)
{
	if (!shader.loadSource("data/shaders/skybox.glsl"))
	{
		std::cerr << std::string("SkyboxRenderPass: Failed to load shader source.") << std::endl;
		return false;
	}
	
	// Set permutation values
	permutation = 0;
	
	// Generate shader permutations
	if (!shader.generatePermutation(permutation))
	{
		std::cerr << std::string("SkyboxRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Connect shader variables
	matrixParam.connect(shader.getInput("matrix"));
	cubemapParam.connect(shader.getInput("cubemap"));
	if (!matrixParam.isConnected() || !cubemapParam.isConnected())
	{
		std::cerr << std::string("SkyboxRenderPass: one or more shader variables were not connected to shader inputs.") << std::endl;
		return false;
	}
	
	
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
	
	// Create AABB geometry
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
	
	return true;
}

void SkyboxRenderPass::unload()
{
	matrixParam.disconnect();
	cubemapParam.disconnect();
	shader.deleteAllPermutations();
	
	glDeleteBuffers(1, &quadIBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
}

void SkyboxRenderPass::render(RenderContext* renderContext)
{
	if (!cubemap)
	{
		return;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	// Bind shader
	shader.activate(permutation);
	
	// Bind cubemap texture
	cubemapParam.setValue(cubemap);
	cubemapParam.upload();
	
	// Calculate matrix
	const Camera& camera = *(renderContext->camera);
	Matrix4 modelView = Matrix4(Matrix3(camera.getView()));
	Matrix4 matrix = glm::inverse(modelView) * glm::inverse(camera.getProjection());
	
	// Upload matrix to shader
	matrixParam.setValue(matrix);
	matrixParam.upload();
	
	// Render quad
	glBindVertexArray(quadVAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT, (void*)0, 0);
}
