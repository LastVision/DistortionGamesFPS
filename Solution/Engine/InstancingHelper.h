#pragma once

#include <GrowingArray.h>
#include <unordered_map>
#include <GameEnum.h>



namespace Prism
{
	class Camera;
	class Model;

	struct ModelData
	{
		CU::GrowingArray<CU::Matrix44<float>> myMatrices;
		CU::GrowingArray<CU::Vector3<float>> myScales;
		CU::GrowingArray<float> myHeights;
	};
	typedef std::unordered_map<Model*, ModelData> ModelMap;

	class InstancingHelper
	{
	public:
		InstancingHelper();

		void SetCamera(const Camera* aCamera);
		void AddModel(Model* aModel, const CU::Matrix44<float>& aWorldMatrix
			, const CU::Vector3<float>& aScale, float aHeight);

		void Render(CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someLights, bool aIsDepthRender);

	private:
		void RenderModel(Model* aModel, ModelData& aModelData, Effect* aEffect);

		const Camera* myCamera;

		ModelMap myRenderInfo;
	};
}