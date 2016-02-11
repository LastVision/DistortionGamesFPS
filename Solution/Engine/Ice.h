#pragma once

#include "BaseModel.h"

namespace Prism
{
	class Camera;
	struct VertexIndexWrapper;
	struct VertexDataWrapper;
	struct VertexPosNormUVBiTang;

	class Ice : public BaseModel
	{
	public:
		Ice(Effect* aEffect, const CU::Vector2<float>& aSize, float aHeight, const std::string& aFilePath);
		~Ice();

		void Render(const Camera& aCamera);

		void SetTextures();
		const float GetHeight() const;

	private:
		void operator=(Ice&) = delete;
		void CreateTextureFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind);
		void CreateVertices();
		void CreateTextures();

		ID3DX11EffectShaderResourceVariable* mySplatAlbedo;
		ID3DX11EffectShaderResourceVariable* mySplatMetalness;
		ID3DX11EffectShaderResourceVariable* mySplatRoughness;
		ID3DX11EffectShaderResourceVariable* mySplatNormal;
		ID3DX11EffectShaderResourceVariable* mySplatAmbientOcclusion;
		
		ID3D11ShaderResourceView* myAlbedoTextures[2];
		ID3D11ShaderResourceView* myMetalnessTextures[2];
		ID3D11ShaderResourceView* myRoughnessTextures[2];
		ID3D11ShaderResourceView* myNormalTextures[2];
		ID3D11ShaderResourceView* myAmbientOcclusionTextures[2];

		const CU::Vector2<float> mySize;
		const float myHeight;

		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;
	};

	inline const float Ice::GetHeight() const
	{
		return myHeight;
	}

}