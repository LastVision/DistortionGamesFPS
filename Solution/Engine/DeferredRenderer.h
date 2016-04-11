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
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
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

	struct PointLightPass : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectVariable* myPointLightVariable;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;
	};

	struct SpotLightPass : public EffectListener
	{
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectVariable* mySpotLightVariable;
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
			D3D11_VIEWPORT* aViewPort, bool aUseAmbientPass);
		void OnResize(float aWidth, float aHeight);

		void GenerateCubemap(Scene* aScene, const std::string& aName);
		void GenerateSHData(Scene* aScene
			, const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint, const std::string& aName);
		void LoadSHData(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint, const std::string& aName);

		void SetCubeMap(const std::string& aFilePath);

		Texture* GetFinishedTexture();
		Texture* GetEmissiveTexture();
		Texture* GetDepthStencilTexture();
		Texture* GetArmDepthStencilTexture();

	private:
		struct GBufferData
		{
			Texture* myAlbedoTexture; //DEPTH is stored in W
			Texture* myNormalTexture; //METALNESS is stored in W
			Texture* myEmissiveTexture;  //ROUGHNESS is stored in W
		};

		void InitFullscreenQuad();
		void CreateVertices();
		void ActivateBuffers();
		void Render(Effect* aEffect);
		void RenderDeferred(Scene* aScene);
		void RenderCubemapDeferred(Scene* aScene, ID3D11RenderTargetView* aTarget, ID3D11DepthStencilView* aDepth, bool aUseAmbientPass);
		void RenderPointLights(Scene* aScene, bool aUseRoomManager = true);
		void RenderSpotLights(Scene* aScene, bool aUseRoomManager = true);
		void RenderSpotLightsTextureProjection(Scene* aScene, bool aUseRoomManager = true);
		void RenderAmbientPass(Scene* aScene);
		void SetAmbientData(bool aClearTextures);

		void SetupAmbientData();
		void SetupLightData();
		void SetupGBufferData();

		void ClearGBuffer();
		void SetGBufferAsTarget(Texture* aDepthTexture);
		void SetPointLightData(const Camera& aCamera);
		void RemovePointLightData();
		void SetSpotLightData(const Camera& aCamera);
		void RemoveSpotLightData();

		void SetSpotLightTextureProjectionData(const Camera& aCamera);
		void RemoveSpotLightTextureProjectionData();

		CubeMapGenerator* myCubeMapGenerator;
		SHTextures mySHTextures;
		Texture* myDepthStencilTexture;
		Texture* myArmDepthStencilTexture;
		Texture* myCubemap;
		Texture* myFinishedTexture;
		RenderToScreenData myRenderToScreenData;
		AmbientPass myAmbientPass;
		PointLightPass myPointLightPass;
		SpotLightPass mySpotLightPass;
		SpotLightPass mySpotLightTextureProjectionPass;
		GBufferData myGBufferData;
		D3D11_VIEWPORT* myViewPort;

		float myClearColor[4];
	};
}