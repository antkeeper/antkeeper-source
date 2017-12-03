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
 * Clears framebuffers
 */
class ClearRenderPass: public RenderPass
{
public:
	ClearRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);
	
	void setClear(bool color, bool depth, bool stencil);
	void setClearColor(const Vector4& color);
	void setClearDepth(float depth);
	void setClearStencil(int index);
	
private:
	bool clearColor;
	bool clearDepth;
	bool clearStencil;
	Vector4 color;
	float depth;
	int index;
};

/**
 * Blurs a texture
 */
class BlurRenderPass: public RenderPass
{
public:
	BlurRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);
	
	inline void setTexture(GLuint textureID) { this->textureID = textureID; }
	inline void setDirection(Vector2 direction) { this->direction = direction; }
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* textureParam;
	const ShaderParameter* resolutionParam;
	const ShaderParameter* directionParam;
	ShaderLoader shaderLoader;
	Shader* shader;
	
	GLuint textureID;
	Vector2 direction;
	
	int quadVertexCount;
	int quadIndexCount;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint quadIBO;
};

/**
 * Renders the distance from the view frustum's near clipping plane to scene geometry. The render target should have a depth only framebuffer.
 */
class ShadowMapRenderPass: public RenderPass
{
public:
	ShadowMapRenderPass();
	
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);
	
	inline void setViewCamera(const Camera* camera) { this->viewCamera = camera; }
	inline void setLightCamera(Camera* camera) { this->lightCamera = camera; }
	
	inline int getFrustumSplitCount() const { return frustumSplitCount; }
	inline const ViewFrustum& getSplitViewFrustum(std::size_t index) const { return splitViewFrustum->getSubfrustum(index); }
	inline const Matrix4& getCropMatrix(std::size_t index) const { return cropMatrices[index]; }
	inline const Matrix4& getTileMatrix(std::size_t index) const { return tileMatrices[index]; }
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* matrixPaletteParam;
	
	ShaderLoader shaderLoader;
	
	Shader* unskinnedShader;
	Shader* skinnedShader;
	int maxBoneCount;
	
	int frustumSplitCount;
	int shadowMapResolution;
	int croppedShadowMapResolution;
	Vector4* croppedShadowMapViewports;
	Matrix4* cropMatrices;
	Matrix4* tileMatrices;
	const Camera* viewCamera;
	Camera* lightCamera;
	SplitViewFrustum* splitViewFrustum;
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
	virtual void render(RenderContext* renderContext);
	
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
	virtual void render(RenderContext* renderContext);
	
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
	virtual void render(RenderContext* renderContext);
	
	inline void setShadowMap(GLuint shadowMap) { this->shadowMap = shadowMap; }
	inline void setShadowCamera(const Camera* camera) { this->shadowCamera = camera; }
	inline void setShadowMapPass(const ShadowMapRenderPass* shadowMapPass) { this->shadowMapPass = shadowMapPass; }
	inline void setDiffuseCubemap(const Texture* cubemap) { this->diffuseCubemap = cubemap; }
	inline void setSpecularCubemap(const Texture* cubemap) { this->specularCubemap = cubemap; }
	
private:
	class RenderOpCompare
	{
	public:
		// Sort render opations
		bool operator()(const RenderOperation& opA, const RenderOperation& opB) const;
	};

	bool loadShader(const RenderOperation& operation);
	
	ShaderParameterSet parameterSet;
	const ShaderParameter* matrixPaletteParam;
	const ShaderParameter* modelParam;
	const ShaderParameter* modelViewParam;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* normalModelViewParam;
	const ShaderParameter* normalModelParam;
	const ShaderParameter* lightViewProjectionParam;
	const ShaderParameter* shadowMapParam;
	const ShaderParameter* cameraPositionParam;
	const ShaderParameter* directionalLightCountParam;
	const ShaderParameter* directionalLightColorsParam;
	const ShaderParameter* directionalLightDirectionsParam;
	const ShaderParameter* albedoOpacityMapParam;
	const ShaderParameter* metalnessRoughnessMapParam;
	const ShaderParameter* normalOcclusionMapParam;
	const ShaderParameter* diffuseCubemapParam;
	const ShaderParameter* specularCubemapParam;
	
	Shader* unskinnedShader;
	Shader* skinnedShader;
	
	int maxBoneCount;
	ShaderLoader shaderLoader;
	std::map<std::size_t, Shader*> shaderCache;
	//Shader* lightingShader;
	
	Matrix4 biasMatrix;
	GLuint shadowMap;
	Texture* treeShadow;
	const Texture* diffuseCubemap;
	const Texture* specularCubemap;
	const Camera* shadowCamera;
	float time;
	const ShadowMapRenderPass* shadowMapPass;
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
	virtual void render(RenderContext* renderContext);
	
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
	virtual void render(RenderContext* renderContext);
	
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
	virtual void render(RenderContext* renderContext);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* modelViewProjectionParam;
	const ShaderParameter* bayerTextureParam;
	
	ShaderLoader shaderLoader;
	Shader* shader;
	GLuint bayerTextureID;
};

/**
 * Renders a skybox
 */
class SkyboxRenderPass: public RenderPass
{
public:
	SkyboxRenderPass();
	
	inline void setCubemap(Texture* cubemap) { this->cubemap = cubemap; }
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);
	
private:
	ShaderParameterSet parameterSet;
	const ShaderParameter* matrixParam;
	const ShaderParameter* cubemapParam;
	
	ShaderLoader shaderLoader;
	Shader* shader;
	Texture* cubemap;
	
	int quadVertexCount;
	int quadIndexCount;
	GLuint quadVAO;
	GLuint quadVBO;
	GLuint quadIBO;
};



#endif // RENDER_PASSES_HPP
