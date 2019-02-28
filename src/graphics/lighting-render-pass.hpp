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

#ifndef LIGHTING_RENDER_PASS_HPP
#define LIGHTING_RENDER_PASS_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

class ResourceManager;
class ShadowMapRenderPass;

class LightingRenderPass: public RenderPass
{
public:
	LightingRenderPass(ResourceManager* resourceManager);
	
	virtual bool load(const RenderContext* renderContext);
	virtual void unload();
	virtual void render(RenderContext* renderContext);

	inline void setShadowMapPass(const ShadowMapRenderPass* shadowMapPass) { this->shadowMapPass = shadowMapPass; }
	inline void setShadowMap(const Texture2D* shadowMap) { this->shadowMap = shadowMap; }
	inline void setTime(float time) { this->time = time; }
	inline void setWireframeLineWidth(float width) { this->wireframeLineWidth = width; }
	
private:
	class RenderOpCompare
	{
	public:
		// Sort render opations
		bool operator()(const RenderOperation& opA, const RenderOperation& opB) const;
	};
	
	ResourceManager* resourceManager;
	Shader* shader;
	int maxBoneCount;
	int frustumSplitCount;
	std::uint32_t unskinnedPermutation;
	std::uint32_t skinnedPermutation;
	Matrix4 biasMatrix;
	const ShadowMapRenderPass* shadowMapPass;
	const Texture2D* shadowMap;
	float time;
	float wireframeLineWidth;

	// Shader parameters
	ShaderMatrix4 modelMatrixParam;
	ShaderMatrix4 modelViewMatrixParam;
	ShaderMatrix4 modelViewProjectionMatrixParam;
	ShaderMatrix3 normalModelMatrixParam;
	ShaderMatrix3 normalModelViewMatrixParam;
	ShaderMatrix4* matrixPaletteParam;
	ShaderMatrix4* lightViewProjectionMatricesParam;
	ShaderVector4 splitDistancesParam;
	ShaderTexture2D shadowMapParam;
	ShaderVector3 cameraPositionParam;
	ShaderFloat timeParam;
	ShaderInt directionalLightCountParam;
	ShaderVector3 directionalLightColorsParam;
	ShaderVector3 directionalLightDirectionsParam;
	ShaderInt spotlightCountParam;
	ShaderVector3 spotlightColorsParam;
	ShaderVector3 spotlightPositionsParam;
	ShaderVector3 spotlightAttenuationsParam;
	ShaderVector3 spotlightDirectionsParam;
	ShaderFloat spotlightCutoffsParam;
	ShaderFloat spotlightExponentsParam;

	#if defined(DEBUG)
		ShaderFloat wireframeLineWidthParam;
	#endif
};

#endif // LIGHTING_RENDER_PASS_HPP
