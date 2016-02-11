#pragma once
#include <StaticArray.h>


namespace Prism
{

	class Effect;
	class Texture;

	class SplatMapContainer
	{
	public:
		SplatMapContainer(Effect* aEffect);
		~SplatMapContainer();

		void SetTextures();


	private:
		void CreateTextureFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind);

		Effect* myEffect;
		ID3DX11EffectShaderResourceVariable* mySplatAlbedo;
		ID3DX11EffectShaderResourceVariable* mySplatMetalness;
		ID3DX11EffectShaderResourceVariable* mySplatRoughness;
		ID3DX11EffectShaderResourceVariable* mySplatNormal;
		ID3DX11EffectShaderResourceVariable* mySplatAmbientOcclusion;

		//CU::StaticArray<ID3D11ShaderResourceView*, 5> myAlbedoTextures;
		//CU::StaticArray<ID3D11ShaderResourceView*, 5> myMetalnessTextures;
		//CU::StaticArray<ID3D11ShaderResourceView*, 5> myRougnessTextures;
		//CU::StaticArray<ID3D11ShaderResourceView*, 5> myNormalTextures;
		//CU::StaticArray<ID3D11ShaderResourceView*, 5> myAmbientOcclusionTextures;


		ID3D11ShaderResourceView* myAlbedoTextures[5];
		ID3D11ShaderResourceView* myMetalnessTextures[5];
		ID3D11ShaderResourceView* myRoughnessTextures[5];
		ID3D11ShaderResourceView* myNormalTextures[5];
		ID3D11ShaderResourceView* myAmbientOcclusionTextures[5];
	};	

}