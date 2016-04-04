#pragma once

#include "BaseModel.h"
#include <D3DX11.h>
#include "EffectListener.h"

struct ID3DX11EffectShaderResourceVariable;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;

namespace Prism
{
	class Effect;
	class Texture;

	struct CombineData : public EffectListener
	{
		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* mySourceA = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepthA = nullptr;
		ID3DX11EffectShaderResourceVariable* mySourceB = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepthB = nullptr;

		void OnEffectLoad() override;

	};

	struct RenderToTextureData : public EffectListener
	{
		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* mySource = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepth = nullptr;

		void OnEffectLoad() override;
	};

	struct BloomData : public EffectListener
	{
		Effect* myBloomEffect;
		ID3DX11EffectShaderResourceVariable* myBloomVariable;
		ID3DX11EffectScalarVariable* myTexelWidthVariable;
		ID3DX11EffectScalarVariable* myTexelHeightVariable;
		Texture* myMiddleMan;
		Texture* myFinalTexture;

		Effect* myDownSampleEffect;
		ID3DX11EffectShaderResourceVariable* myDownSampleVariable;

		Texture* myDownSampleTextures[2];

		void OnEffectLoad() override;
	};

	class FullScreenHelper : public BaseModel
	{
	public:
		FullScreenHelper();
		~FullScreenHelper();

		void Process(Texture* aSource, Texture* aTarget, Texture* aEmissiveTexture, int aEffect, Texture* aFogOfWarTexture);
		void CopyTexture(Texture* aSource, Texture* aTarget);
		void CombineTextures(Texture* aSourceA, Texture* aSourceB, Texture* aTarget, bool aUseDepth);
		void FullScreenHelper::CombineTextures(Texture* aSourceA, Texture* aDepthA
			, Texture* aSourceB, Texture* aDepthB, Texture* aTarget);
		void RenderToScreen(Texture* aSource);
		void RenderToScreen(Texture* aSource, Texture* aDepth);

		void OnResize(float aWidth, float aHeight);

	private:
		
		void CreateCombineData();
		void CreateRenderToTextureData();
		void CreateBloomData();
		void CreateVertices();
		void ActivateBuffers();

		void Render(Effect* aEffect);
		void Render(Effect* aEffect, const std::string& aTechnique);

		void DownSample(Texture* aTarget, Texture* aSource, float aWidth, float aHeight, const std::string& aTechnique);
		void BloomEffect(Texture* aSource, const std::string& aTechnique);
		void DoBloom(Texture* aSource, Texture* aTarget);

		CombineData myCombineData;
		RenderToTextureData myRenderToTextureData;
		BloomData myBloomData;
		Texture* myProcessingTexture;
		D3D11_VIEWPORT myViewPort;
		float myClearColor[4];
	};
}

