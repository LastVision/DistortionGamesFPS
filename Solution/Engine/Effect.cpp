#include "stdafx.h"

#include <d3dx11effect.h>
#include <D3D10effect.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>
#include <DL_Debug.h>
#include "EffectContainer.h"
#include "EffectListener.h"
#include "Effect.h"
#include "Engine.h"
#include "Texture.h"

namespace Prism
{
	
	Effect::Effect()
		: myTechnique(nullptr)
		, myCameraPosition(nullptr)
		, myProjectionMatrix(nullptr)
		, myViewMatrix(nullptr)
		, myWorldMatrix(nullptr)
		, myViewProjectionMatrix(nullptr)
		, myShadowMVP(nullptr)
		, myEyePosition(nullptr)
		, myTotalTime(nullptr)
		, myPlayerVariable(nullptr)
		, myColor(nullptr)
		, myScaleVector(nullptr)
		, myAmbientHue(nullptr)
		, myDirectionalLight(nullptr)
		, myPointLight(nullptr)
		, mySpotLight(nullptr)
		, myTexture(nullptr)
		, myFogOfWarTexture(nullptr)
		, myShadowDepthTexture(nullptr)
		, mySpritePosAndScale(nullptr)
		, mySpriteOrientation(nullptr)
		, myStreakDiffuse(nullptr)
		, myStreakSizeDelta(nullptr)
		, myStreakStartAlpha(nullptr)
		, myStreakAlphaDelta(nullptr)
		, myBonesArray(nullptr)
		, myFileName("not initilized")
		, myEffectListeners(512)
		, myEffect(nullptr)
	{
	}

	Effect::~Effect()
	{
		myEffect->Release();
	}

	bool Effect::Init(const std::string& aEffectFile)
	{
		//if (myEffect != nullptr)
		//{
		//	myEffect->Release();
		//	myEffect = nullptr;
		//}

		if (ReloadShader(aEffectFile) == false)
		{
			return false;
		}

		for (int i = 0; i < myEffectListeners.Size(); ++i)
		{
			myEffectListeners[i]->OnEffectLoad();
		}

		return true;

	}

	ID3DX11EffectTechnique* Effect::GetTechnique(bool aIsDepthRender, const std::string& aName)
	{
		if (aIsDepthRender == true)
		{
			std::string name = aName;
			name += "_DEPTHONLY";
			return myEffect->GetTechniqueByName(name.c_str());
		}
		return myEffect->GetTechniqueByName(aName.c_str());
	}

	void Effect::SetScaleVector(const CU::Vector3<float>& aScaleVector)
	{
		myScaleVector->SetFloatVector(&aScaleVector.x);
	}

	void Effect::SetWorldMatrix(const CU::Matrix44<float>& aWorldMatrix)
	{
		myWorldMatrix->SetMatrix(&aWorldMatrix.myMatrix[0]);
	}

	void Effect::SetViewMatrix(const CU::Matrix44<float>& aViewMatrix)
	{
		myViewMatrix->SetMatrix(&aViewMatrix.myMatrix[0]);
	}

	void Effect::SetProjectionMatrix(const CU::Matrix44<float>& aProjectionMatrix)
	{
		myProjectionMatrix->SetMatrix(&aProjectionMatrix.myMatrix[0]);
	}

	void Effect::SetViewProjectionMatrix(const CU::Matrix44<float>& aMatrix)
	{
		myViewProjectionMatrix->SetMatrix(&aMatrix.myMatrix[0]);
	}

	void Effect::SetBlendState(ID3D11BlendState* aBlendState, float aBlendFactor[4], const unsigned int aSampleMask)
	{
		Engine::GetInstance()->GetContex()->OMSetBlendState(aBlendState, aBlendFactor, aSampleMask);
	}

	void Effect::SetTexture(Texture* aTexture)
	{
		myTexture->SetResource(aTexture ? aTexture->GetShaderView() : nullptr);
	}

	void Effect::SetAmbientHue(CU::Vector4f aVector)
	{
		if (myAmbientHue != nullptr)
		{
			myAmbientHue->SetFloatVector(&aVector.x);
		}
	}

	void Effect::SetBones(const CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES>& someBones)
	{
		myBonesArray->SetMatrixArray(&someBones[0].myMatrix[0], 0, MAX_NR_OF_BONES);
	}

	void Effect::SetFogOfWarTexture(Texture* aFogOfWarTexture)
	{
		myFogOfWarTexture->SetResource(aFogOfWarTexture ? aFogOfWarTexture->GetShaderView() : nullptr);
	}

	void Effect::SetShadowDepthTexture(Texture* aLightDepthTexture)
	{
		myShadowDepthTexture->SetResource(aLightDepthTexture->GetDepthStencilShaderView());
	}

	void Effect::SetShadowMVP(const CU::Matrix44<float>& aMatrix)
	{
		myShadowMVP->SetMatrix(&aMatrix.myMatrix[0]);
	}

	void Effect::SetPosAndScale(const CU::Vector2<float>& aPos
		, const CU::Vector2<float>& aScale)
	{
		DL_ASSERT_EXP(mySpritePosAndScale != nullptr
			, "Effect2D: Tried to SetPosAndScale but mySpritePosAndScale is nullptr");

		mySpritePosAndScaleVector.x = aPos.x;
		mySpritePosAndScaleVector.y = aPos.y;

		mySpritePosAndScaleVector.z = aScale.x;
		mySpritePosAndScaleVector.w = aScale.y;

		mySpritePosAndScale->SetFloatVector(&mySpritePosAndScaleVector.x);
	}

	void Effect::SetColor(const CU::Vector4<float>& aColor)
	{
		myColor->SetFloatVector(&aColor.x);
	}

	void Effect::SetSpriteOrientation(const CU::Matrix44<float>& aOrientation)
	{
		mySpriteOrientation->SetMatrix(&aOrientation.myMatrix[0]);
	}

	void Effect::UpdateDirectionalLights(
		const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData)
	{
		if (myDirectionalLight != nullptr)
		{
			myDirectionalLight->SetRawValue(&someDirectionalLightData[0], 0,
				sizeof(DirectionalLightData) * NUMBER_OF_DIRECTIONAL_LIGHTS);
		}
	}

	void Effect::UpdatePointLights(const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData)
	{
		if (myPointLight != nullptr)
		{
			myPointLight->SetRawValue(&somePointLightData[0], 0, sizeof(PointLightData) * NUMBER_OF_POINT_LIGHTS);
		}
	}

	void Effect::UpdateSpotLights(const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData)
	{
		if (mySpotLight != nullptr)
		{
			mySpotLight->SetRawValue(&someSpotLightData[0], 0, sizeof(SpotLightData) * NUMBER_OF_SPOT_LIGHTS);
		}
	}

	void Effect::UpdateTime(const float aDeltaTime)
	{
		if (myTotalTime != nullptr)
		{
			float newTime = 0.f;
			myTotalTime->GetFloat(&newTime);
			newTime += aDeltaTime;
			myTotalTime->SetFloat(newTime);
		}
	}

	void Effect::SetStreakTexture(Texture* aTexture)
	{
		myStreakDiffuse->SetResource(aTexture->GetShaderView());
	}

	void Effect::SetStreakSizeDelta(float aSizeDelta)
	{
		myStreakSizeDelta->SetFloat(aSizeDelta);
	}

	void Effect::SetStreakStartAlpha(float aStartAlpha)
	{
		myStreakStartAlpha->SetFloat(aStartAlpha);
	}

	void Effect::SetStreakAlphaDelta(float anAlphaDelta)
	{
		myStreakAlphaDelta->SetFloat(anAlphaDelta);
	}

	void Effect::SetPlayerVariable(int someVariable)
	{
		if (myPlayerVariable != nullptr)
		{
			myPlayerVariable->SetInt(someVariable);
		}
	}

	bool Effect::ReloadShader(const std::string& aFile)
	{
		myFileName = aFile;

		HRESULT hr;
		unsigned int shaderFlags = 0;

#if defined (DEBUG) || defined(_DEBUG)
		shaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob* compiledShader = nullptr;
		ID3D10Blob* compilationMsgs = nullptr;

		hr = D3DX11CompileFromFile(myFileName.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compiledShader
			, &compilationMsgs, 0);

	

		if (hr != S_OK)
		{
			if (compilationMsgs != nullptr)
			{
				DL_MESSAGE_BOX((char*)compilationMsgs->GetBufferPointer(), "Effect Error", MB_ICONWARNING);
				myEffect = EffectContainer::GetInstance()->GetEffect(
					"Data/Resource/Shader/S_effect_debug.fx")->myEffect;
			}
			else
			{
				std::string errorMessage = "[Effect]: Could not find the effect " + myFileName;
				DL_ASSERT(errorMessage.c_str());
			}
		}
		if (compilationMsgs != nullptr)
		{
			compilationMsgs->Release();
		}

		if (hr == S_OK)
		{
			hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL
				, Engine::GetInstance()->GetDevice(), &myEffect);

			if (FAILED(hr))
			{
				DL_MESSAGE_BOX("Cant Create Effect", "Effect Error", MB_ICONWARNING);
				return false;
			}

		}

		if (compiledShader != nullptr)
		{
			compiledShader->Release();
		}


		//myTechnique = myEffect->GetTechniqueByName("Render");
		//if (myTechnique->IsValid() == false)
		//{
		//	DL_MESSAGE_BOX("Failed to get Technique", "Effect Error", MB_ICONWARNING);
		//	return false;
		//}

		myScaleVector = myEffect->GetVariableByName("Scale")->AsVector();
		if (myScaleVector->IsValid() == false)
		{
			myScaleVector = nullptr;
		}

		myWorldMatrix = myEffect->GetVariableByName("World")->AsMatrix();
		if (myWorldMatrix->IsValid() == false)
		{
			DL_MESSAGE_BOX("Failed to get WorldMatrix", "Effect Error", MB_ICONWARNING);
			return false;
		}

		myViewMatrix = myEffect->GetVariableByName("View")->AsMatrix();
		if (myViewMatrix->IsValid() == false)
		{
			DL_MESSAGE_BOX("Failed to get ViewMatrix", "Effect Error", MB_ICONWARNING);
			return false;
		}

		myProjectionMatrix = myEffect->GetVariableByName("Projection")->AsMatrix();
		if (myProjectionMatrix->IsValid() == false)
		{
			DL_MESSAGE_BOX("Failed to get ProjectionMatrix", "Effect Error", MB_ICONWARNING);
			return false;
		}

		myViewProjectionMatrix = myEffect->GetVariableByName("ViewProjection")->AsMatrix();
		if (myViewProjectionMatrix->IsValid() == false)
		{
			myViewProjectionMatrix = nullptr;
		}

		myTotalTime = nullptr;
		myTotalTime = myEffect->GetVariableByName("TotalTime")->AsScalar();
		if (myTotalTime->IsValid() == false)
		{
			myTotalTime = nullptr;
		}

		myPlayerVariable = nullptr;
		myPlayerVariable = myEffect->GetVariableByName("PlayerVariable")->AsScalar();
		if (myPlayerVariable->IsValid() == false)
		{
			myPlayerVariable = nullptr;
		}

		myDirectionalLight = myEffect->GetVariableByName("DirectionalLights");
		if (myDirectionalLight->IsValid() == false)
		{
			myDirectionalLight = nullptr;
		}

		myPointLight = myEffect->GetVariableByName("PointLights");
		if (myPointLight->IsValid() == false)
		{
			myPointLight = nullptr;
		}

		mySpotLight = myEffect->GetVariableByName("SpotLights");
		if (mySpotLight->IsValid() == false)
		{
			mySpotLight = nullptr;
		}

		myTexture = myEffect->GetVariableByName("DiffuseTexture")->AsShaderResource();
		if (myTexture->IsValid() == false)
		{
			myTexture = nullptr;
		}

		myFogOfWarTexture = myEffect->GetVariableByName("FogOfWarTexture")->AsShaderResource();
		if (myFogOfWarTexture->IsValid() == false)
		{
			myFogOfWarTexture = nullptr;
		}

		myShadowDepthTexture = myEffect->GetVariableByName("ShadowDepth")->AsShaderResource();
		if (myShadowDepthTexture->IsValid() == false)
		{
			myShadowDepthTexture = nullptr;
		}

		myShadowMVP = myEffect->GetVariableByName("ShadowMVP")->AsMatrix();
		if (myShadowMVP->IsValid() == false)
		{
			myShadowMVP = nullptr;
		}

		mySpritePosAndScale = myEffect->GetVariableByName("SpritePositionAndScale")->AsVector();
		if (mySpritePosAndScale->IsValid() == false)
		{
			mySpritePosAndScale = nullptr;
		}

		myColor = myEffect->GetVariableByName("Color")->AsVector();
		if (myColor->IsValid() == false)
		{
			myColor = nullptr;
		}

		mySpriteOrientation = myEffect->GetVariableByName("SpriteOrientation")->AsMatrix();
		if (mySpriteOrientation->IsValid() == false)
		{
			mySpriteOrientation = nullptr;
		}

		myStreakDiffuse = myEffect->GetVariableByName("DiffuseTexture")->AsShaderResource();
		if (myStreakDiffuse->IsValid() == false)
		{
			myStreakDiffuse = nullptr;
		}

		myStreakSizeDelta = myEffect->GetVariableByName("StreakSizeDelta")->AsScalar();
		if (myStreakSizeDelta->IsValid() == false)
		{
			myStreakSizeDelta = nullptr;
		}

		myStreakStartAlpha = myEffect->GetVariableByName("StreakStartAlpha")->AsScalar();
		if (myStreakStartAlpha->IsValid() == false)
		{
			myStreakStartAlpha = nullptr;
		}

		myStreakAlphaDelta = myEffect->GetVariableByName("StreakAlphaDelta")->AsScalar();
		if (myStreakAlphaDelta->IsValid() == false)
		{
			myStreakAlphaDelta = nullptr;
		}

		myCameraPosition = myEffect->GetVariableByName("cameraPosition")->AsVector();
		if (myCameraPosition->IsValid() == false)
		{
			myCameraPosition = nullptr;
		}

		myAmbientHue = myEffect->GetVariableByName("AmbientHue")->AsVector();
		if (myAmbientHue->IsValid() == false)
		{
			myAmbientHue = nullptr;
		}

		myBonesArray = myEffect->GetVariableByName("Bones")->AsMatrix();
		if (myBonesArray->IsValid() == false)
		{
			myBonesArray = nullptr;
		}

		return true;
	}

	void Effect::SetCameraPosition(const CU::Vector3<float>& aCameraPos)
	{
		if (myCameraPosition != nullptr)
		{
			myCameraPosition->SetFloatVector(static_cast<const float*>(&aCameraPos.x));
		}
	}
}