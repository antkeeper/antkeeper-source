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

#ifndef SHADOW_MAP_RENDER_PASS_HPP
#define SHADOW_MAP_RENDER_PASS_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

class ResourceManager;

/**
 * Renders the distance from the view frustum's near clipping plane to scene geometry. The render target should have a depth only framebuffer.
 */
class ShadowMapRenderPass: public RenderPass
{
public:
	ShadowMapRenderPass(ResourceManager* resourceManager);
	
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);
	
	inline void setViewCamera(const Camera* camera) { this->viewCamera = camera; }
	inline void setLightCamera(Camera* camera) { this->lightCamera = camera; }
	
	inline const Camera* getLightCamera() const { return lightCamera; }
	inline const SplitViewFrustum& getSplitViewFrustum() const { return *splitViewFrustum; }
	inline const Matrix4& getCropMatrix(std::size_t index) const { return cropMatrices[index]; }
	inline const Matrix4& getTileMatrix(std::size_t index) const { return tileMatrices[index]; }
	
private:
	class RenderOpCompare
	{
	public:
		// Sort render opations
		bool operator()(const RenderOperation& opA, const RenderOperation& opB) const;
	};
	
	ResourceManager* resourceManager;
	Shader* shader;
	std::uint32_t unskinnedPermutation;
	std::uint32_t skinnedPermutation;
	ShaderMatrix4 modelViewProjectionParam;
	ShaderMatrix4* matrixPaletteParam; // data not used, just getConnectedInput() then pass pose matrix palette pointer directly
	
	int maxBoneCount;
	int shadowMapResolution;
	int croppedShadowMapResolution;
	Vector4* croppedShadowMapViewports;
	Matrix4* cropMatrices;
	Matrix4* tileMatrices;
	const Camera* viewCamera;
	Camera* lightCamera;
	SplitViewFrustum* splitViewFrustum;
};

#endif // SHADOW_MAP_RENDER_PASS_HPP
