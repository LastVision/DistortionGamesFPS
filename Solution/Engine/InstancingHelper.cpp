#include "stdafx.h"

#include "Camera.h"
#include "InstancingHelper.h"
#include "Effect.h"
#include <d3dx11effect.h>

namespace Prism
{
	InstancingHelper::InstancingHelper()
		: myCamera(nullptr)
	{
	}

	void InstancingHelper::SetCamera(const Camera* aCamera)
	{
		myCamera = aCamera;
	}

	void InstancingHelper::AddModel(Model* aModel, const CU::Matrix44<float>& aWorldMatrix
		, const CU::Vector3<float>& aScale, float aHeight)
	{
		if (myRenderInfo.find(aModel) == myRenderInfo.end())
		{
			myRenderInfo[aModel].myMatrices.Init(128);
			myRenderInfo[aModel].myScales.Init(128);
			myRenderInfo[aModel].myHeights.Init(128);
		}

		myRenderInfo[aModel].myMatrices.Add(aWorldMatrix);
		myRenderInfo[aModel].myScales.Add(aScale);
		myRenderInfo[aModel].myHeights.Add(aHeight);
	}

	void InstancingHelper::Render(CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someLights
		, bool aIsDepthRender)
	{
		DL_ASSERT_EXP(myCamera != nullptr, "Tried to render without a camera");

		Effect* oldEffect = nullptr;
		D3DX11_TECHNIQUE_DESC techDesc;
		ID3DX11EffectTechnique* tech;

		for (auto it = myRenderInfo.begin(); it != myRenderInfo.end(); ++it)
		{
			
			Model* currModel = it->first;
			CU::GrowingArray<CU::Matrix44<float>>& matrices = it->second.myMatrices;
			CU::GrowingArray<CU::Vector3<float>>& scales = it->second.myScales;
			CU::GrowingArray<float>& heights = it->second.myHeights;

			Effect* currEffect = currModel->GetEffect();
			if (currEffect != oldEffect)
			{
				oldEffect = currEffect;

				currEffect->SetViewProjectionMatrix(myCamera->GetViewProjection());
				currEffect->SetScaleVector({ 1.f, 1.f, 1.f });
				currEffect->SetCameraPosition(myCamera->GetOrientation().GetPos());
				currEffect->UpdateDirectionalLights(someLights);
			}

			if (currModel->SetGPUState(matrices, scales, heights))
			{
				tech = currEffect->GetTechnique(currModel->GetTechniqueName());
				tech->GetDesc(&techDesc);

				if (tech->IsValid() == false)
				{
					tech = currEffect->GetTechnique();
					DL_ASSERT("INVALID TECHNIQUE IN INSTANCEHELPER::RENDER:");
				}

				for (UINT j = 0; j < techDesc.Passes; ++j)
				{
					ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
					tech->GetPassByIndex(j)->Apply(0, context);
					context->DrawIndexedInstanced(currModel->GetIndexCount(), matrices.Size()
						, 0, currModel->GetVertexStart(), 0);

				}
				currModel->DeActivateSurfaces();
			}
				
			matrices.RemoveAll();
			scales.RemoveAll();
			heights.RemoveAll();
		}
	}
}