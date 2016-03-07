#pragma once
#include "BaseModel.h"

struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;

namespace Prism
{
	class Effect;
	class Scene;
	class Texture;

	class DeferredRenderer : public BaseModel
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		void Render(Scene* aScene);
		void OnResize(float aWidth, float aHeight);

	private:
		struct RenderToScreenData
		{
			Effect* myEffect = nullptr;
			ID3DX11EffectShaderResourceVariable* mySource = nullptr;
		};

		struct AmbientPass
		{
			Effect* myEffect = nullptr;
			ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
			ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
			ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
			ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;
			ID3DX11EffectScalarVariable* myAmbientMultiplier = nullptr;
		};

		struct LightPass
		{
			Effect* myEffect = nullptr;
			ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
			ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
			ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
			ID3DX11EffectShaderResourceVariable* myMetalness = nullptr;
			ID3DX11EffectShaderResourceVariable* myAmbientOcclusion = nullptr;
			ID3DX11EffectShaderResourceVariable* myRoughness = nullptr;

			ID3DX11EffectVariable* myPointLightVariable;
			ID3DX11EffectMatrixVariable* myInvertedProjection;
			ID3DX11EffectMatrixVariable* myNotInvertedView;
		};

		struct GBufferData
		{
			Texture* myAlbedoTexture;
			Texture* myNormalTexture;
			Texture* myDepthTexture;

			Texture* myMetalnessTexture;
			Texture* myAmbientOcclusionTexture;
			Texture* myRoughnessTexture;
		};

		void InitFullscreenQuad();
		void CreateVertices();
		void ActivateBuffers();
		void Render(Effect* aEffect);
		void RenderDeferred(Scene* aScene);
		void RenderPointLights(Scene* aScene);

		void SetupAmbientData();
		void SetupLightData();
		void SetupGBufferData();

		
		Texture* myDepthStencilTexture;
		Texture* myCubemap;
		RenderToScreenData myRenderToScreenData;
		AmbientPass myAmbientPass;
		LightPass myLightPass;
		GBufferData myGBufferData;

		float myClearColor[4];
	};
}