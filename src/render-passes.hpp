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

#ifndef RENDER_PASSES_HPP
#define RENDER_PASSES_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

#include "material-loader.hpp"
#include "model-loader.hpp"

/**
 * Renders the distance from the view frustum's near clipping plane to scene geometry. The render target should have a depth only framebuffer.
 */
class ShadowMapRenderPass: public RenderPass
{
public:
	ShadowMapRenderPass();
	
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	
	ShaderLoader shaderLoader;
	Shader* depthShader;
};

/**
 * Writes clipped edges to stencil buffer.
 */
class ClippingRenderPass: public RenderPass
{
public:
	ClippingRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
	void setClippingPlane(const Plane& plane);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelParam;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* clippingPlanesParam;
	
	ShaderLoader shaderLoader;
	Shader* shader;
	Vector4 clippingPlane;
};
 
/**
 * Renders soil profiles.
 *
 * A soil profile generally of five soil horizons: O, A, B, C, and R.
 *
 * Horizon O: Organic
 * Horizon A: Surface
 * Horizon B: Subsoil
 * Horizon C: Substratum
 * Horizon R: Bedrock
 *
 * In this render pass, only the O, A, B, and C horizons are used. 
 */
class SoilRenderPass: public RenderPass
{
public:
	SoilRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
	inline void setHorizonOTexture(Texture* texture) { horizonOTexture = texture; }
	inline void setHorizonATexture(Texture* texture) { horizonATexture = texture; }
	inline void setHorizonBTexture(Texture* texture) { horizonBTexture = texture; }
	inline void setHorizonCTexture(Texture* texture) { horizonCTexture = texture; }
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelParam;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* horizonTexturesParam;
	
	ShaderLoader shaderLoader;
	Shader* shader;
	
	Texture* horizonOTexture;
	Texture* horizonATexture;
	Texture* horizonBTexture;
	Texture* horizonCTexture;
};


/**
 * Renders scene geometry.
 */
class LightingRenderPass: public RenderPass
{
public:
	LightingRenderPass();
	
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
	inline void setShadowMap(GLuint shadowMap) { this->shadowMap = shadowMap; }
	inline void setShadowCamera(const Camera* camera) { this->shadowCamera = camera; }
	
private:
	bool loadShader(const RenderOperation& operation);
	
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelParam;
	const ShaderParameter* modelViewParam;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* normalModelViewParam;
	const ShaderParameter* normalModelParam;
	const ShaderParameter* cameraPositionParam;
	const ShaderParameter* directionalLightCountParam;
	const ShaderParameter* directionalLightColorsParam;
	const ShaderParameter* directionalLightDirectionsParam;
	const ShaderParameter* albedoOpacityMapParam;
	const ShaderParameter* metalnessRoughnessMapParam;
	const ShaderParameter* normalOcclusionMapParam;
	const ShaderParameter* diffuseCubemapParam;
	const ShaderParameter* specularCubemapParam;
	
	ShaderLoader shaderLoader;
	std::map<std::size_t, Shader*> shaderCache;
	Shader* lightingShader;
	
	Matrix4 biasMatrix;
	GLuint shadowMap;
	Texture* treeShadow;
	Texture* diffuseCubemap;
	Texture* specularCubemap;
	const Camera* shadowCamera;
	float time;
};

/**
 * Renders bounding boxes, skeletons
 */
class DebugRenderPass: public RenderPass
{
public:
	DebugRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
	//void setDrawBounds(bool enabled);
	//void setDrawSkeletons(bool enabled);
	//void setDrawCameras(bool enabled);
	//void setDrawLights(bool enabled);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	
	ShaderLoader shaderLoader;
	Shader* unlitSolidShader;
	
	int aabbVertexCount;
	int aabbIndexCount;
	GLuint aabbVAO;
	GLuint aabbVBO;
	GLuint aabbIBO;
};

/**
 * Renders the user interface
 */
class UIRenderPass: public RenderPass
{
public:
	UIRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* textureParam;
	const ShaderParameter* texcoordOffsetParam;
	const ShaderParameter* texcoordScaleParam;
	
	ShaderLoader shaderLoader;
	Shader* texturedUIShader;
	Shader* untexturedUIShader;
};

/**
 * Renders a vignette
 */
class VignetteRenderPass: public RenderPass
{
public:
	VignetteRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* bayerTextureParam;
	
	ShaderLoader shaderLoader;
	Shader* shader;
	GLuint bayerTextureID;
};


#endif // RENDER_PASSES_HPP
