#pragma once
#include "BaseModel.h"

struct ID3DX11EffectVariable;
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
		};

		struct LightPass
		{
			Effect* myEffect = nullptr;
			ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
			ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
			ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
			ID3DX11EffectVariable* myPointLightVariable;
			ID3DX11EffectMatrixVariable* myInvertedProjection;
			ID3DX11EffectMatrixVariable* myNotInvertedView;
		};

		void InitFullscreenQuad();
		void CreateVertices();
		void ActivateBuffers();
		void RenderTextureToScreen(Texture* aTexture);
		void Render(Effect* aEffect);
		void RenderDeferred(Scene* aScene);
		void RenderPointLights(Scene* aScene);

		void SetupAmbientData();
		void SetupLightData();

		Texture* myAlbedoTexture;
		Texture* myNormalTexture;
		Texture* myDepthTexture;
		Texture* myDepthStencilTexture;
		Texture* myCubemap;
		RenderToScreenData myRenderToScreenData;
		AmbientPass myAmbientPass;
		LightPass myLightPass;

		float myClearColor[4];
	};
}