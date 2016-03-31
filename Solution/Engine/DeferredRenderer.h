#pragma once
#include "BaseModel.h"
#include "EffectListener.h"
#include "CubeMapGenerator.h"

struct D3D11_VIEWPORT;
struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;

namespace Prism
{
	class Effect;
	class Scene;
	class Texture;

	struct RenderToScreenData : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* mySource = nullptr;
	};

	struct AmbientPass : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectShaderResourceVariable* mySSAORandomTextureVariable = nullptr;
		ID3DX11EffectScalarVariable* myAmbientMultiplier = nullptr;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;

		ID3DX11EffectShaderResourceVariable* mycAr = nullptr;
		ID3DX11EffectShaderResourceVariable* mycAg = nullptr;
		ID3DX11EffectShaderResourceVariable* mycAb = nullptr;
		ID3DX11EffectShaderResourceVariable* mycBr = nullptr;
		ID3DX11EffectShaderResourceVariable* mycBg = nullptr;
		ID3DX11EffectShaderResourceVariable* mycBb = nullptr;
		ID3DX11EffectShaderResourceVariable* mycC = nullptr;

		ID3DX11EffectVectorVariable* mySHGridSizeVariable;
		ID3DX11EffectVectorVariable* mySHGridOffsetVariable;

		CU::Vector3<float> mySHGridSize;
		CU::Vector3<float> mySHGridOffset;
		Texture* mySSAORandomTexture;
	};

	struct LightPass : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
		ID3DX11EffectShaderResourceVariable* myMetalness = nullptr;
		ID3DX11EffectShaderResourceVariable* myAmbientOcclusion = nullptr;
		ID3DX11EffectShaderResourceVariable* myRoughness = nullptr;
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectVariable* myPointLightVariable;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;
	};


	class DeferredRenderer : public BaseModel
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		void Render(Scene* aScene);
		void RenderCubeMap(Scene* aScene, ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepth,
			D3D11_VIEWPORT* aViewPort);
		void OnResize(float aWidth, float aHeight);

		void GenerateSHData(Scene* aScene
			, const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint, const std::string& aName);
		void SetCubeMap(const std::string& aFilePath);

		Texture* GetFinishedTexture();
		Texture* GetEmissiveTexture();

	private:
		struct GBufferData
		{
			Texture* myAlbedoTexture;
			Texture* myNormalTexture;
			Texture* myDepthTexture;

			Texture* myMetalnessTexture;
			Texture* myAmbientOcclusionTexture;
			Texture* myRoughnessTexture;
			Texture* myEmissiveTexture;
		};

		void InitFullscreenQuad();
		void CreateVertices();
		void ActivateBuffers();
		void Render(Effect* aEffect);
		void RenderDeferred(Scene* aScene);
		void RenderCubemapDeferred(Scene* aScene, ID3D11RenderTargetView* aTarget, ID3D11DepthStencilView* aDepth);
		void RenderPointLights(Scene* aScene);
		void RenderAmbientPass(Scene* aScene);
		void SetAmbientData(bool aClearTextures);

		void SetupAmbientData();
		void SetupLightData();
		void SetupGBufferData();

		void ClearGBuffer();
		void SetGBufferAsTarget();
		void SetLightPassData(const Camera& aCamera);
		void RemoveLightPassData();

		CubeMapGenerator* myCubeMapGenerator;
		SHTextures mySHTextures;
		Texture* myDepthStencilTexture;
		Texture* myCubemap;
		Texture* myFinishedTexture;
		RenderToScreenData myRenderToScreenData;
		AmbientPass myAmbientPass;
		LightPass myLightPass;
		GBufferData myGBufferData;
		D3D11_VIEWPORT* myViewPort;

		float myClearColor[4];
	};
}