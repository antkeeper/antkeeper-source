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
#include "materials.hpp"
#include <iostream>

ShadowMapRenderPass::ShadowMapRenderPass():
	depthShader(nullptr)
{
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjection", ShaderParameter::Type::MATRIX_4, 1);
}

bool ShadowMapRenderPass::load(const RenderContext* renderContext)
{
	depthShader = shaderLoader.load("data/shaders/depth-pass.glsl", &parameterSet);
	if (!depthShader)
	{
		return false;
	}
	
	return true;
}

void ShadowMapRenderPass::unload()
{
	delete depthShader;
	depthShader = nullptr;
}

void ShadowMapRenderPass::render(RenderContext* renderContext)
{
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Clear the framebuffer depth
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable alpha blending
	glDisable(GL_BLEND);
	
	// Bind shader
	depthShader->bind();

	const Camera& camera = *(renderContext->camera);
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	
	// Render operations
	for (const RenderOperation& operation: *operations)
	{		
		// Skip render operations with unsupported materials
		if (operation.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::PHYSICAL))
		{
			continue;
		}
		
		const PhysicalMaterial* material = static_cast<const PhysicalMaterial*>(operation.material);
		
		// Skip non shadow casters
		if (!material->shadowCaster)
		{
			continue;
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;		
		depthShader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}



ClippingRenderPass::ClippingRenderPass():
	shader(nullptr)
{
	clippingPlanesParam = parameterSet.addParameter("clippingPlanes", ShaderParameter::Type::VECTOR_4, 1);
	modelParam = parameterSet.addParameter("modelMatrix", ShaderParameter::Type::MATRIX_4, 1);
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
}

bool ClippingRenderPass::load(const RenderContext* renderContext)
{
	shaderLoader.undefine();
	shaderLoader.define("CLIPPING_PLANE_COUNT", 1);
	
	shader = shaderLoader.load("data/shaders/clip.glsl", &parameterSet);
	if (!shader)
	{
		return false;
	}
	
	return true;
}

void ClippingRenderPass::unload()
{
	delete shader;
	shader = nullptr;
}

void ClippingRenderPass::render(RenderContext* renderContext)
{
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	// Bind shader
	shader->bind();
	
	// Pass clipping planes to shader
	shader->setParameter(clippingPlanesParam, clippingPlane);
	
	// Grab render context parameters
	const Camera& camera = *(renderContext->camera);
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	
	// Two passes
	for (int i = 0; i < 2; ++i)
	{
		if (!i)
		{
			// Increment stencil for back faces
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glCullFace(GL_FRONT);
		}
		else
		{
			// Decrement stencil for front faces
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			glCullFace(GL_BACK);
		}
		
		for (const RenderOperation& operation: *operations)
		{			
			const Matrix4& modelMatrix = operation.transform;
			Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
			shader->setParameter(modelParam, modelMatrix);
			shader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
			
			glBindVertexArray(operation.vao);
			glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
		}
	}
	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_CLIP_DISTANCE0);
}

void ClippingRenderPass::setClippingPlane(const Plane& plane)
{
	this->clippingPlane = Vector4(plane.getNormal(), plane.getDistance());
}

SoilRenderPass::SoilRenderPass():
	shader(nullptr)
{
	horizonTexturesParam = parameterSet.addParameter("horizonTextures", ShaderParameter::Type::INT, 4);
	modelParam = parameterSet.addParameter("modelMatrix", ShaderParameter::Type::MATRIX_4, 1);
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
	
	horizonOTexture = nullptr;
	horizonATexture = nullptr;
	horizonBTexture = nullptr;
	horizonCTexture = nullptr;
}

bool SoilRenderPass::load(const RenderContext* renderContext)
{
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_TEXCOORD", EMERGENT_VERTEX_TEXCOORD);
	shaderLoader.define("VERTEX_NORMAL", EMERGENT_VERTEX_NORMAL);
	shader = shaderLoader.load("data/shaders/soil-profile.glsl", &parameterSet);
	if (!shader)
	{
		return false;
	}
	
	return true;
}

void SoilRenderPass::unload()
{
	shaderLoader.undefine();
	
	delete shader;
	shader = nullptr;
	
	delete horizonOTexture;
	delete horizonATexture;
	delete horizonBTexture;
	delete horizonCTexture;
	
	horizonOTexture = nullptr;
	horizonATexture = nullptr;
	horizonBTexture = nullptr;
	horizonCTexture = nullptr;
}

void SoilRenderPass::render(RenderContext* renderContext)
{
	// Bind shader
	shader->bind();
	
	if (!horizonOTexture || !horizonATexture || !horizonBTexture || !horizonCTexture)
	{
		return;
	}
	
	// Bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, horizonOTexture->getTextureID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, horizonATexture->getTextureID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, horizonBTexture->getTextureID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, horizonCTexture->getTextureID());
	
	// Pass texture units to shader
	int textureUnits[] = {0, 1, 2, 3};
	shader->setParameter(horizonTexturesParam, 0, &textureUnits[0], 4);
	
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	const Camera& camera = *(renderContext->camera);
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	
	// Render operations
	for (const RenderOperation& operation: *operations)
	{
		// Skip render operations with unsupported materials
		if (operation.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::PHYSICAL))
		{
			continue;
		}	
		const PhysicalMaterial* material = static_cast<const PhysicalMaterial*>(operation.material);
		if (!(material->flags & (unsigned int)PhysicalMaterial::Flags::SOIL))
		{
			continue;
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
		shader->setParameter(modelParam, modelMatrix);
		shader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

LightingRenderPass::LightingRenderPass():
	shadowMap(0),
	shadowCamera(nullptr),
	treeShadow(nullptr),
	diffuseCubemap(nullptr)
{
	// Initialize bias matrix for calculating the model-view-projection-bias matrix (used for shadow map texture coordinate calculation)
	biasMatrix = Matrix4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);
	
	maxBoneCount = 64;
	
	matrixPaletteParam = parameterSet.addParameter("matrixPalette", ShaderParameter::Type::MATRIX_4, maxBoneCount);
	modelParam = parameterSet.addParameter("modelMatrix", ShaderParameter::Type::MATRIX_4, 1);
	modelViewParam = parameterSet.addParameter("modelViewMatrix", ShaderParameter::Type::MATRIX_4, 1);
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
	normalModelViewParam = parameterSet.addParameter("normalModelViewMatrix", ShaderParameter::Type::MATRIX_3, 1);
	normalModelParam = parameterSet.addParameter("normalModelMatrix", ShaderParameter::Type::MATRIX_3, 1);
	cameraPositionParam = parameterSet.addParameter("cameraPosition", ShaderParameter::Type::VECTOR_3, 1);
	directionalLightCountParam = parameterSet.addParameter("directionalLightCount", ShaderParameter::Type::INT, 1);
	directionalLightColorsParam = parameterSet.addParameter("directionalLightColors", ShaderParameter::Type::VECTOR_3, 1);
	directionalLightDirectionsParam = parameterSet.addParameter("directionalLightDirections", ShaderParameter::Type::VECTOR_3, 1);
	albedoOpacityMapParam = parameterSet.addParameter("albedoOpacityMap", ShaderParameter::Type::INT, 1);
	metalnessRoughnessMapParam = parameterSet.addParameter("metalnessRoughnessMap", ShaderParameter::Type::INT, 1);
	normalOcclusionMapParam = parameterSet.addParameter("normalOcclusionMap", ShaderParameter::Type::INT, 1);
	diffuseCubemapParam = parameterSet.addParameter("diffuseCubemap", ShaderParameter::Type::INT, 1);
	specularCubemapParam = parameterSet.addParameter("specularCubemap", ShaderParameter::Type::INT, 1);
}

bool LightingRenderPass::load(const RenderContext* renderContext)
{
	// Load tree shadow
	TextureLoader textureLoader;
	treeShadow = textureLoader.load("data/textures/tree-shadow-0.png");
	if (!treeShadow)
	{
		std::cerr << "Failed to load tree shadow" << std::endl;
	}
	
	// Load cubemap
	textureLoader.setCubemap(true);
	textureLoader.setMipmapChain(false);
	diffuseCubemap = textureLoader.load("data/textures/campus-diffuse.png");
	if (!diffuseCubemap)
	{
		std::cerr << "Failed to load cubemap" << std::endl;
	}
	
	textureLoader.setCubemap(true);
	textureLoader.setMipmapChain(true);
	specularCubemap = textureLoader.load("data/textures/campus-specular_m%02d.png");
	if (!specularCubemap)
	{
		std::cerr << "Failed to load cubemap" << std::endl;
	}
	
	// Load unskinned shader
	shaderLoader.undefine();
	shaderLoader.define("TEXTURE_COUNT", 0);
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_NORMAL", EMERGENT_VERTEX_NORMAL);
	shaderLoader.define("VERTEX_TEXCOORD", EMERGENT_VERTEX_TEXCOORD);
	unskinnedShader = shaderLoader.load("data/shaders/lit-object.glsl", &parameterSet);
	
	// Load skinned shader
	shaderLoader.define("SKINNED");
	shaderLoader.define("MAX_BONE_COUNT", maxBoneCount);
	shaderLoader.define("VERTEX_BONE_INDICES", EMERGENT_VERTEX_BONE_INDICES);
	shaderLoader.define("VERTEX_BONE_WEIGHTS", EMERGENT_VERTEX_BONE_WEIGHTS);
	skinnedShader = shaderLoader.load("data/shaders/lit-object.glsl", &parameterSet);
	
	if (!unskinnedShader || !skinnedShader)
	{
		return false;
	}
	
	time = 0.0f;
		
	return true;
}

void LightingRenderPass::unload()
{
	delete unskinnedShader;
	delete skinnedShader;
	
	unskinnedShader = nullptr;
	skinnedShader = nullptr;
	
	for (auto it = shaderCache.begin(); it != shaderCache.end(); ++it)
	{
		delete it->second;
	}
	shaderCache.clear();
	
	delete treeShadow;
	treeShadow = nullptr;
	
	delete diffuseCubemap;
	diffuseCubemap = nullptr;
	
	delete specularCubemap;
	specularCubemap = nullptr;
}

void LightingRenderPass::render(RenderContext* renderContext)
{
	/*
	time += 1.0f / 60.f;
	
	// Bind framebuffer and setup viewport
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->framebuffer);
	glViewport(0, 0, renderTarget->width, renderTarget->height);
	
	// Clear the framebuffer depth
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearDepth(1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Enable clipping
	if (renderContext->layer->getIndex() == 0)
	{
		// Clipping pass
		cappingRenderContext.camera = renderContext->camera;
		cappingRenderContext.layer = renderContext->layer;
		for (int i = 0; i < 5; ++i)
		{
			clippingRenderPass.setClippingPlane(clippingPlanes[i]);
			clippingRenderPass.render(renderContext);
			
			Transform transform = Transform::getIdentity();
			transform.translation = clippingPlanes[i].getNormal() * -clippingPlanes[i].getDistance();
			transform.scale = Vector3(150.0f);
			transform.rotation = glm::normalize(glm::rotation(Vector3(0, 0, -1), clippingPlanes[i].getNormal()));

			ModelInstance* cappingPlaneInstance = &cappingPlaneInstances[i];
			cappingPlaneInstance->setTransform(transform);
			
			cappingRenderQueue.clear();
			cappingRenderQueue.queue(cappingPlaneInstance);
			
			cappingRenderPass.render(&cappingRenderContext);

		}
		
		glEnable(GL_CLIP_DISTANCE0);
	}
	else
	{
		glDisable(GL_CLIP_DISTANCE0);
	}
		
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const Camera& camera = *(renderContext->camera);
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	
	// Calculate camera direction
	const Vector3& cameraDirection = camera.getForward();
	
	// Textures
	const std::size_t maxTextures = 8;
	int materialTexture[maxTextures];
	Vector2 texcoordOffset[maxTextures];
	Vector2 texcoordScale[maxTextures];
	float textureDiffuseInfluence[maxTextures];
	float textureSpecularInfluence[maxTextures];
	float textureEmissiveInfluence[maxTextures];
	float textureRoughnessInfluence[maxTextures];
	float textureOpacityInfluence[maxTextures];
	float textureNormalInfluence[maxTextures];
	
	for (std::size_t i = 0; i < maxTextures; ++i)
	{
		materialTexture[i] = i + 2;
	}
	
	// Lights
	// Point lights
	const std::size_t maxPointLightCount = 4;
	std::size_t pointLightCount = 0;
	Vector3 pointLightColor[maxPointLightCount];
	Vector3 pointLightPosition[maxPointLightCount];
	Vector3 pointLightAttenuation[maxPointLightCount];
	for (std::size_t i = 0; i < maxPointLightCount; ++i)
		pointLightColor[i] = Vector3(0.0f);
	
	// Directional lights
	const std::size_t maxDirectionalLightCount = 4;
	std::size_t directionalLightCount = 0;
	Vector3 directionalLightColor[maxDirectionalLightCount];
	Vector3 directionalLightDirection[maxDirectionalLightCount];
	for (std::size_t i = 0; i < maxDirectionalLightCount; ++i)
		directionalLightColor[i] = Vector3(0.0f);
	
	// Spotlights
	const std::size_t maxSpotlightCount = 4;
	std::size_t spotlightCount = 0;
	Vector3 spotlightColor[maxSpotlightCount];
	Vector3 spotlightPosition[maxSpotlightCount];
	Vector3 spotlightAttenuation[maxSpotlightCount];
	Vector3 spotlightDirection[maxSpotlightCount];
	float spotlightCutoff[maxSpotlightCount];
	float spotlightExponent[maxSpotlightCount];
	for (std::size_t i = 0; i < maxSpotlightCount; ++i)
		spotlightColor[i] = Vector3(0.0f);
	
	const std::list<SceneObject*>* lights = renderContext->layer->getObjects(SceneObjectType::LIGHT);
	if (lights != nullptr)
	{
		for (auto object: *lights)
		{
			const Light* light = static_cast<const Light*>(object);
			LightType lightType = light->getLightType();
			
			if (lightType == LightType::POINT)
			{
				const PointLight* pointLight = static_cast<const PointLight*>(light);
				
				pointLightColor[pointLightCount] = pointLight->getScaledColor();
				pointLightPosition[pointLightCount] = Vector3(camera.getView() * Vector4(pointLight->getTranslation(), 1.0f));
				pointLightAttenuation[pointLightCount] = pointLight->getAttenuation();
				
				++pointLightCount;
			}
			else if (lightType == LightType::DIRECTIONAL)
			{
				const DirectionalLight* directionalLight = static_cast<const DirectionalLight*>(light);
				
				directionalLightColor[directionalLightCount] = directionalLight->getScaledColor();
				directionalLightDirection[directionalLightCount] = glm::normalize(Vector3(camera.getView() * Vector4(-directionalLight->getDirection(), 0.0f)));
				
				++directionalLightCount;
			}
			else if (lightType == LightType::SPOTLIGHT)
			{
				const Spotlight* spotlight = static_cast<const Spotlight*>(light);
				
				spotlightColor[spotlightCount] = spotlight->getScaledColor();
				spotlightPosition[spotlightCount] = Vector3(camera.getView() * Vector4(spotlight->getTranslation(), 1.0f));
				spotlightAttenuation[spotlightCount] = spotlight->getAttenuation();
				spotlightDirection[spotlightCount] = glm::normalize(Vector3(camera.getView() * Vector4(-spotlight->getDirection(), 0.0f)));
				spotlightCutoff[spotlightCount] = spotlight->getCutoff();
				spotlightExponent[spotlightCount] = spotlight->getExponent();
				
				++spotlightCount;
			}
		}
	}
	
	// Calculate the (light-space) view-projection-bias matrix
	Matrix4 viewProjectionBiasMatrix = biasMatrix * shadowCamera->getViewProjection();

	// Bind shader
	Shader* boundShader = nullptr;
	
	// Bind shadow map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	
	// Bind tree shadow
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, treeShadow.getTextureID());
	
	// For each clipping plane
	
	for (int clippingPlaneIndex = 0; clippingPlaneIndex < 5; ++clippingPlaneIndex)
	{
		// Render operations
		for (const RenderOperation& operation: *operations)
		{
			const Material* material = operation.material;
			
			// Find shader
			std::size_t hashValue = material->getHashValue();
			auto it = shaderCache.find(hashValue);
			if (it == shaderCache.end())
			{
				std::cerr << "Warning: material requires unloaded shader" << std::endl;
				continue;
			}
			
			// Bind shader
			Shader* shader = it->second;
			if (shader != boundShader)
			{
				shader->bind();
				boundShader = shader;
			}
			
			// Get shader parameters
			ShaderParameterSet* parameters = shader->getParameters();
			
			const Matrix4& modelMatrix = operation.transform;
			Matrix4 modelViewMatrix = camera.getView() * modelMatrix;
			Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
			
			// Pass matrix parameters
			if (parameters->hasParameter(ShaderParameter::MODEL_MATRIX))
			{
				parameters->setValue(ShaderParameter::MODEL_MATRIX, modelMatrix);
			}
			
			if (parameters->hasParameter(ShaderParameter::MODEL_VIEW_MATRIX))
			{
				parameters->setValue(ShaderParameter::MODEL_VIEW_MATRIX, modelViewMatrix);
			}
			if (parameters->hasParameter(ShaderParameter::MODEL_VIEW_PROJECTION_MATRIX))
			{
				parameters->setValue(ShaderParameter::MODEL_VIEW_PROJECTION_MATRIX, modelViewProjectionMatrix);
			}
			if (parameters->hasParameter(ShaderParameter::NORMAL_MODEL_MATRIX))
			{
				Matrix3 normalModelMatrix = glm::transpose(glm::inverse(Matrix3(modelMatrix)));
				parameters->setValue(ShaderParameter::NORMAL_MODEL_MATRIX, normalModelMatrix);
			}
			if (parameters->hasParameter(ShaderParameter::NORMAL_MODEL_VIEW_MATRIX))
			{
				Matrix3 normalModelViewMatrix = glm::transpose(glm::inverse(Matrix3(modelViewMatrix)));
				parameters->setValue(ShaderParameter::NORMAL_MODEL_VIEW_MATRIX, normalModelViewMatrix);
			}
			if (parameters->hasParameter(ShaderParameter::CAMERA_DIRECTION))
			{
				parameters->setValue(ShaderParameter::CAMERA_DIRECTION, cameraDirection);
			}
			
			// Pass material parameters
			if (parameters->hasParameter(ShaderParameter::MATERIAL_DIFFUSE_COLOR))
			{
				parameters->setValue(ShaderParameter::MATERIAL_DIFFUSE_COLOR, material->getDiffuseColor());
			}
			if (parameters->hasParameter(ShaderParameter::MATERIAL_SPECULAR_COLOR))
			{
				parameters->setValue(ShaderParameter::MATERIAL_SPECULAR_COLOR, material->getSpecularColor());
			}
			if (parameters->hasParameter(ShaderParameter::MATERIAL_EMISSIVE_COLOR))
			{
				parameters->setValue(ShaderParameter::MATERIAL_EMISSIVE_COLOR, material->getEmissiveColor());
			}
			if (parameters->hasParameter(ShaderParameter::MATERIAL_ROUGHNESS))
			{
				parameters->setValue(ShaderParameter::MATERIAL_ROUGHNESS, material->getRoughness());
			}
			if (parameters->hasParameter(ShaderParameter::MATERIAL_OPACITY))
			{
				parameters->setValue(ShaderParameter::MATERIAL_OPACITY, material->getOpacity());
			}
			
			// Pass texture parameters
			if (parameters->hasParameter(ShaderParameter::MATERIAL_TEXTURE))
			{
				std::size_t textureCount = material->getTextureCount();
				
				for (std::size_t i = 0; i < textureCount; ++i)
				{
					const Texture* texture = material->getTexture(i);
					texcoordOffset[i] = Vector2(texture->getCoordinateOffset());
					texcoordScale[i] = Vector2(texture->getCoordinateScale());
					textureDiffuseInfluence[i] = texture->getDiffuseInfluence();
					textureSpecularInfluence[i] = texture->getSpecularInfluence();
					textureEmissiveInfluence[i] = texture->getEmissiveInfluence();
					textureRoughnessInfluence[i] = texture->getRoughnessInfluence();
					textureOpacityInfluence[i] = texture->getOpacityInfluence();
					textureNormalInfluence[i] = texture->getNormalInfluence();
					
					// Bind texture
					glActiveTexture(GL_TEXTURE2 + i);
					glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
				}
				
				parameters->setValue(ShaderParameter::MATERIAL_TEXTURE, 0, &materialTexture[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXCOORD_OFFSET))
					parameters->setValue(ShaderParameter::TEXCOORD_OFFSET, 0, &texcoordOffset[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXCOORD_SCALE))
					parameters->setValue(ShaderParameter::TEXCOORD_SCALE, 0, &texcoordScale[0], textureCount);
				
				if (parameters->hasParameter(ShaderParameter::TEXTURE_DIFFUSE_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_DIFFUSE_INFLUENCE, 0, &textureDiffuseInfluence[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXTURE_SPECULAR_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_SPECULAR_INFLUENCE, 0, &textureSpecularInfluence[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXTURE_EMISSIVE_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_EMISSIVE_INFLUENCE, 0, &textureEmissiveInfluence[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXTURE_ROUGHNESS_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_ROUGHNESS_INFLUENCE, 0, &textureRoughnessInfluence[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXTURE_OPACITY_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_OPACITY_INFLUENCE, 0, &textureOpacityInfluence[0], textureCount);
				if (parameters->hasParameter(ShaderParameter::TEXTURE_NORMAL_INFLUENCE))
					parameters->setValue(ShaderParameter::TEXTURE_NORMAL_INFLUENCE, 0, &textureNormalInfluence[0], textureCount);
			}
			
			// Pass lighting parameters
			if (parameters->hasParameter(ShaderParameter::POINT_LIGHT_COLOR))
			{
				parameters->setValue(ShaderParameter::POINT_LIGHT_COLOR, 0, &pointLightColor[0], spotlightCount);
				parameters->setValue(ShaderParameter::POINT_LIGHT_POSITION, 0, &pointLightPosition[0], spotlightCount);
				parameters->setValue(ShaderParameter::POINT_LIGHT_ATTENUATION, 0, &pointLightAttenuation[0], spotlightCount);
			}
			if (parameters->hasParameter(ShaderParameter::DIRECTIONAL_LIGHT_COLOR))
			{
				parameters->setValue(ShaderParameter::DIRECTIONAL_LIGHT_COLOR, 0, &directionalLightColor[0], directionalLightCount);
				parameters->setValue(ShaderParameter::DIRECTIONAL_LIGHT_DIRECTION, 0, &directionalLightDirection[0], directionalLightCount);
			}
			if (parameters->hasParameter(ShaderParameter::SPOTLIGHT_COLOR))
			{
				parameters->setValue(ShaderParameter::SPOTLIGHT_COLOR, 0, &spotlightColor[0], spotlightCount);
				parameters->setValue(ShaderParameter::SPOTLIGHT_POSITION, 0, &spotlightPosition[0], spotlightCount);
				parameters->setValue(ShaderParameter::SPOTLIGHT_ATTENUATION, 0, &spotlightAttenuation[0], spotlightCount);
				parameters->setValue(ShaderParameter::SPOTLIGHT_DIRECTION, 0, &spotlightDirection[0], spotlightCount);
				parameters->setValue(ShaderParameter::SPOTLIGHT_CUTOFF, 0, &spotlightCutoff[0], spotlightCount);
				parameters->setValue(ShaderParameter::SPOTLIGHT_EXPONENT, 0, &spotlightExponent[0], spotlightCount);
			}
			if (parameters->hasParameter(ShaderParameter::DIFFUSE_ENVIRONMENT_MAP))
			{
				parameters->setValue(ShaderParameter::DIFFUSE_ENVIRONMENT_MAP, 1);
			}
			if (parameters->hasParameter(ShaderParameter::SPECULAR_ENVIRONMENT_MAP))
			{
				parameters->setValue(ShaderParameter::SPECULAR_ENVIRONMENT_MAP, 2);
			}
			
			// Pass shadow parameters
			if (parameters->hasParameter(ShaderParameter::MODEL_VIEW_PROJECTION_BIAS_MATRIX))
			{
				Matrix4 modelViewProjectionBiasMatrix = viewProjectionBiasMatrix * modelMatrix;
				parameters->setValue(ShaderParameter::MODEL_VIEW_PROJECTION_BIAS_MATRIX, modelViewProjectionBiasMatrix);
			}
			if (parameters->hasParameter(ShaderParameter::SHADOW_MAP))
			{			
				parameters->setValue(ShaderParameter::SHADOW_MAP, 0);
			}
			if (parameters->hasParameter(ShaderParameter::TREE_SHADOW))
			{			
				parameters->setValue(ShaderParameter::TREE_SHADOW, 1);
			}
			if (parameters->hasParameter(ShaderParameter::TIME))
			{			
				parameters->setValue(ShaderParameter::TIME, time);
			}
			
			// Pass clipping parameters
			if (parameters->hasParameter(ShaderParameter::CLIPPING_PLANES))
			{
				const Plane& clippingPlane = clippingPlanes[clippingPlaneIndex];
				parameters->setValue(ShaderParameter::CLIPPING_PLANES, Vector4(clippingPlane.getNormal(), clippingPlane.getDistance()));
			}
			
			// Draw geometry
			glBindVertexArray(operation.vao);
			if (!material->isOpaque())
			{	
				
				//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
				//glBlendFunc(GL_ONE, GL_ONE);
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//glBlendFunc(GL_ZERO, GL_SRC_COLOR);
				//glDepthMask(GL_FALSE);
				glFrontFace(GL_CW);
				

				glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
				
				parameters->setValue(ShaderParameter::MATERIAL_DIFFUSE_COLOR, material->getDiffuseColor() * 0.05f);
				glFrontFace(GL_CCW);
				//glDepthMask(GL_TRUE);
				glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
			}
			
			
		}
	}
	
	// Disable clipping
	glDisable(GL_CLIP_DISTANCE0);
	*/
	
	const Camera& camera = *(renderContext->camera);
	std::list<RenderOperation>* operations = renderContext->queue->getOperations();

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	int directionalLightCount = 1;
	Vector3 directionalLightColors[3];
	Vector3 directionalLightDirections[3];
	directionalLightColors[0] = Vector3(1);
	directionalLightDirections[0] = glm::normalize(Vector3(camera.getView() * -Vector4(0, 0, -1, 0)));
		
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseCubemap->getTextureID());
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, specularCubemap->getTextureID());
	
	Shader* shader = nullptr;
	Texture* albedoOpacityMap = nullptr;
	Texture* metalnessRoughnessMap = nullptr;
	Texture* normalOcclusionMap = nullptr;
	
	// Sort operations
	operations->sort(RenderOpCompare());
	
	// Render operations
	for (const RenderOperation& operation: *operations)
	{
		// Skip render operations with unsupported materials
		if (operation.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::PHYSICAL))
		{
			continue;
		}
		const PhysicalMaterial* material = static_cast<const PhysicalMaterial*>(operation.material);
		if (!(material->flags & (unsigned int)PhysicalMaterial::Flags::OBJECT))
		{
			continue;
		}
		
		// Skip render operations with unsupported vertex formats
		
		// Select shader
		Shader* targetShader = nullptr;
		if (operation.pose != nullptr)
		{
			targetShader = skinnedShader;
		}
		else
		{
			targetShader = unskinnedShader;
		}
		
		// Switch shader if necessary
		if (shader != targetShader)
		{
			shader = targetShader;
			
			// Bind shader
			shader->bind();
			
			// Pass static params
			shader->setParameter(albedoOpacityMapParam, 0);
			shader->setParameter(metalnessRoughnessMapParam, 1);
			shader->setParameter(normalOcclusionMapParam, 2);
			shader->setParameter(diffuseCubemapParam, 3);
			shader->setParameter(specularCubemapParam, 4);
			shader->setParameter(directionalLightCountParam, directionalLightCount);
			shader->setParameter(directionalLightColorsParam, 0, &directionalLightColors[0], directionalLightCount);
			shader->setParameter(directionalLightDirectionsParam, 0, &directionalLightDirections[0], directionalLightCount);
			shader->setParameter(cameraPositionParam, camera.getTranslation());
		}
		
		// Pass matrix palette
		if (operation.pose != nullptr)
		{
			shader->setParameter(matrixPaletteParam, 0, operation.pose->getMatrixPalette(), operation.pose->getSkeleton()->getBoneCount());
		}
		
		// Bind albedo-opacity map
		if (material->albedoOpacityMap != albedoOpacityMap)
		{
			albedoOpacityMap = material->albedoOpacityMap;
			
			if (albedoOpacityMap != nullptr)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, albedoOpacityMap->getTextureID());
			}
		}
		
		// Bind metalness-roughness map
		if (material->metalnessRoughnessMap != metalnessRoughnessMap)
		{
			metalnessRoughnessMap = material->metalnessRoughnessMap;
			
			if (metalnessRoughnessMap != nullptr)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, metalnessRoughnessMap->getTextureID());
			}
		}
		
		// Bind normal-occlusion map
		if (material->normalOcclusionMap != normalOcclusionMap)
		{
			normalOcclusionMap = material->normalOcclusionMap;
			
			if (normalOcclusionMap != nullptr)
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, normalOcclusionMap->getTextureID());
			}
		}
		
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewMatrix = camera.getView() * modelMatrix;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;	
		Matrix3 normalModelViewMatrix = glm::transpose(glm::inverse(Matrix3(modelViewMatrix)));
		Matrix3 normalModelMatrix = glm::transpose(glm::inverse(Matrix3(modelMatrix)));
		
		shader->setParameter(modelParam, modelMatrix);
		shader->setParameter(modelViewParam, modelViewMatrix);
		shader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		shader->setParameter(normalModelViewParam, normalModelViewMatrix);
		shader->setParameter(normalModelParam, normalModelMatrix);
		
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

bool LightingRenderPass::loadShader(const RenderOperation& operation)
{
	/*
	const std::string shaderFilename = "data/shaders/main.glsl";
	
	// Get material and its hash value
	const Material* material = operation.material;
	std::size_t hashValue = material->getHashValue();

	// Check if shader has already been loaded
	auto it = shaderCache.find(hashValue);
	if (it != shaderCache.end())
		return true;
	
	// Define shader preprocessor macros
	
	// Undefine previous definitions
	shaderLoader.undefine();
	
	// Clipping
	shaderLoader.define("CLIPPING_PLANE_COUNT", 1);

	// Vertex format
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_NORMAL", EMERGENT_VERTEX_NORMAL);
	//shaderLoader.define("VERTEX_COLOR", EMERGENT_VERTEX_COLOR);

	// Lighting


	// Material
	if (material->isShadeless())
	{
		shaderLoader.define("SHADELESS");
	}
	else
	{
		// Lighting
		//shaderLoader.define("POINT_LIGHT_COUNT", 1);
		shaderLoader.define("DIRECTIONAL_LIGHT_COUNT", 2);
		//shaderLoader.define("SPOTLIGHT_COUNT", 2);
		//shaderLoader.define("ENVIRONMENT_MAP");
	}
	
	if (material->isShadowReceiver())
		shaderLoader.define("SHADOWED");

	// Final
	shaderLoader.define("GAMMA_CORRECT");

	// Load shader
	Shader* shader = shaderLoader.load(shaderFilename);
	if (!shader)
	{
		std::cerr << "Failed to load shader \"" << shaderFilename << "\"" << std::endl;
		return false;
	}

	// Store shader in cache
	shaderCache[hashValue] = shader;
	
	return true;
	*/
	return false;
}

bool LightingRenderPass::RenderOpCompare::operator()(const RenderOperation& opA, const RenderOperation& opB) const
{
	// Skip render operations with unsupported materials
	if (opA.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::PHYSICAL))
	{
		return false;
	}
	else if (opB.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::PHYSICAL))
	{
		return true;
	}
	
	// Cast materials
	const PhysicalMaterial* materialA = static_cast<const PhysicalMaterial*>(opA.material);
	const PhysicalMaterial* materialB = static_cast<const PhysicalMaterial*>(opB.material);
	
	// Determine transparency
	bool transparentA = materialA->flags & (unsigned int)PhysicalMaterial::Flags::TRANSLUCENT;
	bool transparentB = materialB->flags & (unsigned int)PhysicalMaterial::Flags::TRANSLUCENT;
	
	if (transparentA)
	{
		if (transparentB)
		{
			// A and B are both transparent, sort by depth
			return (opA.depth <= opB.depth);
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
			// A and B are both opaque, sort by material
			return (opA.material < opB.material);
		}
	}
}

DebugRenderPass::DebugRenderPass()
{
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
}

bool DebugRenderPass::load(const RenderContext* renderContext)
{
	unlitSolidShader = shaderLoader.load("data/shaders/unlit-solid.glsl", &parameterSet);
	if (!unlitSolidShader)
	{
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
	delete unlitSolidShader;
	unlitSolidShader = nullptr;
	
	glDeleteBuffers(1, &aabbIBO);
	glDeleteBuffers(1, &aabbVBO);
	glDeleteVertexArrays(1, &aabbVAO);
}

void DebugRenderPass::render(RenderContext* renderContext)
{
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
	unlitSolidShader->bind();
	
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
		
		unlitSolidShader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		
		glDrawElements(GL_LINES, aabbIndexCount, GL_UNSIGNED_INT, (void*)0);
	}
}

UIRenderPass::UIRenderPass()
{
	modelViewProjectionParam = parameterSet.addParameter("modelViewProjectionMatrix", ShaderParameter::Type::MATRIX_4, 1);
	textureParam = parameterSet.addParameter("tex", ShaderParameter::Type::INT, 1);
	texcoordOffsetParam = parameterSet.addParameter("texcoordOffset", ShaderParameter::Type::VECTOR_2, 1);
	texcoordScaleParam = parameterSet.addParameter("texcoordScale", ShaderParameter::Type::VECTOR_2, 1);
}

bool UIRenderPass::load(const RenderContext* renderContext)
{
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_TEXCOORD", EMERGENT_VERTEX_TEXCOORD);
	shaderLoader.define("VERTEX_COLOR", EMERGENT_VERTEX_COLOR);
	shaderLoader.define("GAMMA_CORRECT");
	shaderLoader.define("TEXTURE_COUNT", 1);
	
	texturedUIShader = shaderLoader.load("data/shaders/ui.glsl", &parameterSet);
	
	shaderLoader.undefine();
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	shaderLoader.define("VERTEX_COLOR", EMERGENT_VERTEX_COLOR);
	shaderLoader.define("GAMMA_CORRECT");
	
	untexturedUIShader = shaderLoader.load("data/shaders/ui.glsl", &parameterSet);
	
	if (!texturedUIShader || !untexturedUIShader)
	{
		return false;
	}
	
	return true;
}

void UIRenderPass::unload()
{
	delete texturedUIShader;
	delete untexturedUIShader;
	
	texturedUIShader = nullptr;
	untexturedUIShader = nullptr;
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
	
	glActiveTexture(GL_TEXTURE0);
	
	Shader* shader = nullptr;
	
	// Render operations
	const std::list<RenderOperation>* operations = renderContext->queue->getOperations();
	for (const RenderOperation& operation: *operations)
	{
		// Skip render operations with unsupported materials
		if (operation.material->getMaterialFormatID() != static_cast<unsigned int>(MaterialFormat::UI))
		{
			continue;
		}
		const UIMaterial* material = static_cast<const UIMaterial*>(operation.material);
		
		
		if (material->texture != nullptr)
		{
			shader = texturedUIShader;
			shader->bind();
			shader->setParameter(textureParam, 0);
			shader->setParameter(texcoordOffsetParam, Vector2(0.0f));
			shader->setParameter(texcoordScaleParam, Vector2(1.0f));
			
			glBindTexture(GL_TEXTURE_2D, material->texture->getTextureID());
		}
		else
		{
			shader = untexturedUIShader;
			shader->bind();
		}
				
		const Matrix4& modelMatrix = operation.transform;
		Matrix4 modelViewProjectionMatrix = camera.getViewProjection() * modelMatrix;
		
		// Pass matrix parameters
		shader->setParameter(modelViewProjectionParam, modelViewProjectionMatrix);
		
		// Draw geometry
		glBindVertexArray(operation.vao);
		glDrawElementsBaseVertex(GL_TRIANGLES, operation.triangleCount * 3, GL_UNSIGNED_INT, (void*)0, operation.indexOffset);
	}
}

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

SkyboxRenderPass::SkyboxRenderPass():
	shader(nullptr),
	cubemap(nullptr)
{
	matrixParam = parameterSet.addParameter("matrix", ShaderParameter::Type::MATRIX_4, 1);
	cubemapParam = parameterSet.addParameter("cubemap", ShaderParameter::Type::INT, 1);
}

bool SkyboxRenderPass::load(const RenderContext* renderContext)
{
	shaderLoader.undefine();
	shaderLoader.define("VERTEX_POSITION", EMERGENT_VERTEX_POSITION);
	
	shader = shaderLoader.load("data/shaders/skybox.glsl", &parameterSet);
	if (!shader)
	{
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
	delete shader;
	shader = nullptr;
	
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
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	// Bind shader
	shader->bind();
	
	// Bind cubemap texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getTextureID());
	
	// Pass texture unit to shader
	shader->setParameter(cubemapParam, 0);
	
	// Calculate matrix
	const Camera& camera = *(renderContext->camera);
	Matrix4 modelView = Matrix4(Matrix3(camera.getView()));
	Matrix4 matrix = glm::inverse(modelView) * glm::inverse(camera.getProjection());
	
	// Pass matrix to shader
	shader->setParameter(matrixParam, matrix);
	
	// Render quad
	glBindVertexArray(quadVAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, quadIndexCount, GL_UNSIGNED_INT, (void*)0, 0);
}
