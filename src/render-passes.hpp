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
	ShaderLoader shaderLoader;
	Shader* shader;
	Vector4 clippingPlane;
};
 
/**
 * Caps clipped geometry.
 */
class CappingRenderPass: public RenderPass
{
public:
	CappingRenderPass();
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
	ShaderLoader shaderLoader;
	Shader* shader;
	
	Texture horizonOTexture;
	Texture horizonATexture;
	Texture horizonBTexture;
	Texture horizonCTexture;
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
	
	inline void setClippingPlanes(const Plane* planes)
	{
		for (int i = 0; i < 5; ++i)
		{
			this->clippingPlanes[i] = planes[i];
		}
	}
	
private:
	bool loadShader(const RenderOperation& operation);
	ShaderLoader shaderLoader;
	std::map<std::size_t, Shader*> shaderCache;
	
	Matrix4 biasMatrix;
	GLuint shadowMap;
	Texture treeShadow;
	const Camera* shadowCamera;
	float time;
	
	// Cutaway clipping and capping
	Plane clippingPlanes[5];
	Model* unitPlaneModel;
	ModelInstance cappingPlaneInstances[5];
	RenderQueue cappingRenderQueue;
	RenderContext cappingRenderContext;
	ClippingRenderPass clippingRenderPass;
	CappingRenderPass cappingRenderPass;
	
	MaterialLoader materialLoader;
	ModelLoader modelLoader;
};

/**
 * Renders bounding boxes, skeletons
 */
class DebugRenderPass: public RenderPass
{
public:
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
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
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(const RenderContext* renderContext);
	
private:
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
	ShaderLoader shaderLoader;
	Shader* shader;
	GLuint bayerTextureID;
};


#endif // RENDER_PASSES_HPP
