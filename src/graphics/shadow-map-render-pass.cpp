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

#include "shadow-map-render-pass.hpp"
#include "resources/resource-manager.hpp"

ShadowMapRenderPass::ShadowMapRenderPass(ResourceManager* resourceManager):
	resourceManager(resourceManager),
	shader(nullptr),
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
	splitViewFrustum->setSplitSchemeWeight(0.6f);
	
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
	
	// Load shader
	shader = resourceManager->load<Shader>("depth-pass.glsl");
	
	// Generate unskinned and skinned permutations
	if (!shader->generatePermutation(unskinnedPermutation) || !shader->generatePermutation(skinnedPermutation))
	{
		std::cerr << std::string("ShadowMapRenderPass: failed to generate shader permutation.") << std::endl;
		return false;
	}
	
	// Allocate bone palette parameter
	matrixPaletteParam = new ShaderMatrix4(maxBoneCount);
	
	// Connect shader variables
	modelViewProjectionParam.connect(shader->getInput("modelViewProjectionMatrix"));
	matrixPaletteParam->connect(shader->getInput("matrixPalette"));
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
	shader->deleteAllPermutations();
	
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
	
	// Draw back faces only
	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	// Disable alpha blending
	glDisable(GL_BLEND);

	//const Camera& lightCamera = *(renderContext->camera);
	std::list<RenderOperation>* operations = renderContext->queue->getOperations();	
	
	GLuint boundVAO = 0;
	
	const Matrix4& viewCameraView = viewCamera->getViewTween()->getSubstate();
	const Matrix4& viewCameraProjection = viewCamera->getProjectionTween()->getSubstate();
	const Matrix4& lightCameraViewProjection = lightCamera->getViewProjectionTween()->getSubstate();
	splitViewFrustum->setMatrices(viewCameraView, viewCameraProjection);

	
	// Sort operations
	operations->sort(RenderOpCompare());
	
	std::uint32_t permutation = 0xDEADBEEF;
	std::uint32_t noShadowCastingFlag = 4;
	
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
			AABB croppingBounds = subfrustumBounds.transformed(lightCameraViewProjection);
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
		
		Matrix4 croppedViewProjection = cropMatrices[i] * lightCameraViewProjection;
		
		// Activate viewport for corresponding cropped shadow map
		const Vector4& viewport = croppedShadowMapViewports[i];
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Render operations
		for (const RenderOperation& operation: *operations)
		{
			// Skip operations with no materials and materials with no shadows
			if (operation.material == nullptr || (operation.material->getFlags() & noShadowCastingFlag))
			{
				continue;
			}
			
			// TODO: Perform culling for subfrustums
			
			// Select permutation
			std::uint32_t targetPermutation = (operation.pose != nullptr) ? skinnedPermutation : unskinnedPermutation;
			if (permutation != targetPermutation)
			{
				permutation = targetPermutation;
				shader->activate(permutation);
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

