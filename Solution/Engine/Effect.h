#pragma once

#include "Defines.h"
#include <GrowingArray.h>
#include "LightStructs.h"
#include <Matrix.h>
#include <string>
#include <StaticArray.h>

struct ID3D11BlendState;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectVectorVariable;
struct ID3DX11EffectShaderResourceVariable;

namespace Prism
{
	class EffectListener;
	class Texture;

	class Effect
	{
	public:
		Effect();
		~Effect();

		ID3DX11Effect* GetEffect();
		ID3DX11EffectTechnique* GetTechnique(const std::string& aName = "Render");
		const std::string& GetFileName() const;
		bool Init(const std::string& aEffectFile);
		void SetScaleVector(const CU::Vector3<float>& aScaleVector);
		void SetWorldMatrix(const CU::Matrix44<float>& aWorldMatrix);
		void SetViewMatrix(const CU::Matrix44<float>& aViewMatrix);
		void SetProjectionMatrix(const CU::Matrix44<float>& aProjectionMatrix);
		void SetViewProjectionMatrix(const CU::Matrix44<float>& aMatrix);
		void SetBlendState(ID3D11BlendState* aBlendState, float aBlendFactor[4], const unsigned int aSampleMask = 0xFFFFFFFF);
		void SetTexture(Texture* aTexture);
		void SetPosAndScale(const CU::Vector2<float>& aPos
			, const CU::Vector2<float>& aScale = { 1.f, 1.f });
		void SetColor(const CU::Vector4<float>& aColor);
		void SetSpriteOrientation(const CU::Matrix44<float>& aOrientation);

		void UpdateDirectionalLights(
			const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData);
		void UpdatePointLights(const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData);
		void UpdateSpotLights(const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData);
		void UpdateTime(const float aDeltaTime);

		void SetStreakAlphaDelta(float aAlphaDelta);
		void SetStreakSizeDelta(float aSizeDelta);
		void SetStreakStartAlpha(float aStartAlpha);
		void SetStreakTexture(Texture* aTexture);

		void SetPlayerVariable(int someVariable);

		void AddListener(EffectListener* aListener);
		bool ReloadShader(const std::string& aFile);

		void SetCameraPosition(const CU::Vector3<float>& aCameraPos);

		void SetAmbientHue(CU::Vector4f aVector);
		void SetBones(const CU::StaticArray<CU::Matrix44<float>, MAX_NR_OF_BONES>& someBones);

		void SetFogOfWarTexture(Texture* aFogOfWarTexture);
		void SetShadowDepthTexture(Texture* aShadowDepthTexture);
		void SetShadowMVP(const CU::Matrix44<float>& aMatrix);

		void SetGradiantValue(float aValue);
		void SetGradiantDirection(const CU::Vector2<float>& aDirection);

	private:
		ID3DX11Effect* myEffect;
		ID3DX11EffectTechnique* myTechnique;

		ID3DX11EffectVectorVariable* myCameraPosition;
		ID3DX11EffectMatrixVariable* myProjectionMatrix;
		ID3DX11EffectMatrixVariable* myViewMatrix;
		ID3DX11EffectMatrixVariable* myWorldMatrix;
		ID3DX11EffectMatrixVariable* myViewProjectionMatrix;
		ID3DX11EffectMatrixVariable* myShadowMVP;

		ID3DX11EffectScalarVariable* myEyePosition;
		ID3DX11EffectScalarVariable* myTotalTime;
		ID3DX11EffectScalarVariable* myPlayerVariable;

		ID3DX11EffectVectorVariable* myColor;
		ID3DX11EffectVectorVariable* myScaleVector;

		ID3DX11EffectVectorVariable* myAmbientHue;

		ID3DX11EffectVariable* myDirectionalLight;
		ID3DX11EffectVariable* myPointLight;
		ID3DX11EffectVariable* mySpotLight;

		ID3DX11EffectShaderResourceVariable* myTexture;
		ID3DX11EffectShaderResourceVariable* myFogOfWarTexture;
		ID3DX11EffectShaderResourceVariable* myShadowDepthTexture;

		ID3DX11EffectVectorVariable* mySpritePosAndScale;
		ID3DX11EffectMatrixVariable* mySpriteOrientation;

		ID3DX11EffectShaderResourceVariable* myStreakDiffuse;
		ID3DX11EffectScalarVariable* myStreakSizeDelta;
		ID3DX11EffectScalarVariable* myStreakStartAlpha;
		ID3DX11EffectScalarVariable* myStreakAlphaDelta;

		ID3DX11EffectMatrixVariable* myBonesArray;

		CU::Vector4<float> mySpritePosAndScaleVector;

		std::string myFileName;

		CU::GrowingArray<EffectListener*> myEffectListeners;

		ID3DX11EffectScalarVariable* myGradiantValue;
		ID3DX11EffectVectorVariable* myGradiantDirection;
	};


	inline ID3DX11Effect* Effect::GetEffect()
	{
		return myEffect;
	}

	inline const std::string& Effect::GetFileName() const
	{
		return myFileName;
	}


	inline void Effect::AddListener(EffectListener* aListener)
	{
		myEffectListeners.Add(aListener);
	}
}