#include "stdafx.h"
#include <d3dx11effect.h>
#include "SplatMapContainer.h"
#include "TextureHelper.h"

namespace Prism
{
	SplatMapContainer::SplatMapContainer(Effect* aEffect)
		: myEffect(aEffect)
	{
		mySplatAlbedo = myEffect->GetEffect()->GetVariableByName("myAlbedoTextures")->AsShaderResource();
		if (mySplatAlbedo->IsValid() == false)
		{
			DL_ASSERT("SplatAlbedos Failed to Load.");
		}
		mySplatMetalness = myEffect->GetEffect()->GetVariableByName("myMetalnessTextures")->AsShaderResource();
		if (mySplatMetalness->IsValid() == false)
		{
			DL_ASSERT("SplatMetalness Failed to Load.");
		}
		mySplatRoughness = myEffect->GetEffect()->GetVariableByName("myRoughnessTextures")->AsShaderResource();
		if (mySplatRoughness->IsValid() == false)
		{
			DL_ASSERT("SplatRoughness Failed to Load.");
		}
		mySplatNormal = myEffect->GetEffect()->GetVariableByName("myNormalTextures")->AsShaderResource();
		if (mySplatNormal->IsValid() == false)
		{
			DL_ASSERT("SplatNormal Failed to Load.");
		}
		mySplatAmbientOcclusion = myEffect->GetEffect()->GetVariableByName("myAmbientOcclusionTextures")->AsShaderResource();
		if (mySplatAmbientOcclusion->IsValid() == false)
		{
			DL_ASSERT("SplatOcclusion Failed to Load.");
		}

		
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_base/T_splat_base_albedo.dds", &myAlbedoTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_r/T_splat_r_albedo.dds", &myAlbedoTextures[1]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_albedo.dds", &myAlbedoTextures[2]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_albedo.dds", &myAlbedoTextures[3]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_albedo.dds", &myAlbedoTextures[4]);

		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_base/T_splat_base_metalness.dds", &myMetalnessTextures[0]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_r/T_splat_r_metalness.dds", &myMetalnessTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_metalness.dds", &myMetalnessTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_metalness.dds", &myMetalnessTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_metalness.dds", &myMetalnessTextures[4]);
		//
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_base/T_splat_base_roughness.dds", &myRoughnessTextures[0]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_r/T_splat_r_roughness.dds", &myRoughnessTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_roughness.dds", &myRoughnessTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_roughness.dds", &myRoughnessTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_roughness.dds", &myRoughnessTextures[4]);
		
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_base/T_splat_base_normal.dds", &myNormalTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_r/T_splat_r_normal.dds", &myNormalTextures[1]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_normal.dds", &myNormalTextures[2]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_normal.dds", &myNormalTextures[3]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_normal.dds", &myNormalTextures[4]);
		
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_base/T_splat_base_ao.dds", &myAmbientOcclusionTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_r/T_splat_r_ao.dds", &myAmbientOcclusionTextures[1]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_ao.dds", &myAmbientOcclusionTextures[2]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_ao.dds", &myAmbientOcclusionTextures[3]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_ao.dds", &myAmbientOcclusionTextures[4]);

	}

	SplatMapContainer::~SplatMapContainer()
	{
		SAFE_RELEASE(myAlbedoTextures[0]);
		SAFE_RELEASE(myAlbedoTextures[1]);
		SAFE_RELEASE(myAlbedoTextures[2]);
		SAFE_RELEASE(myAlbedoTextures[3]);
		SAFE_RELEASE(myAlbedoTextures[4]);

		//SAFE_RELEASE(myMetalnessTextures[0]);
		//SAFE_RELEASE(myMetalnessTextures[1]);
		//SAFE_RELEASE(myMetalnessTextures[2]);
		//SAFE_RELEASE(myMetalnessTextures[3]);
		//SAFE_RELEASE(myMetalnessTextures[4]);
		//
		//SAFE_RELEASE(myRoughnessTextures[0]);
		//SAFE_RELEASE(myRoughnessTextures[1]);
		//SAFE_RELEASE(myRoughnessTextures[2]);
		//SAFE_RELEASE(myRoughnessTextures[3]);
		//SAFE_RELEASE(myRoughnessTextures[4]);

		SAFE_RELEASE(myNormalTextures[0]);
		SAFE_RELEASE(myNormalTextures[1]);
		SAFE_RELEASE(myNormalTextures[2]);
		SAFE_RELEASE(myNormalTextures[3]);
		SAFE_RELEASE(myNormalTextures[4]);

		SAFE_RELEASE(myAmbientOcclusionTextures[0]);
		SAFE_RELEASE(myAmbientOcclusionTextures[1]);
		SAFE_RELEASE(myAmbientOcclusionTextures[2]);
		SAFE_RELEASE(myAmbientOcclusionTextures[3]);
		SAFE_RELEASE(myAmbientOcclusionTextures[4]);
	}


	void SplatMapContainer::SetTextures()
	{
		mySplatAlbedo->SetResourceArray(&myAlbedoTextures[0], 0, 5);
		mySplatMetalness->SetResourceArray(&myMetalnessTextures[0], 0, 5);
		mySplatRoughness->SetResourceArray(&myRoughnessTextures[0], 0, 5);
		mySplatNormal->SetResourceArray(&myNormalTextures[0], 0, 5);
		mySplatAmbientOcclusion->SetResourceArray(&myAmbientOcclusionTextures[0], 0, 5);
	}

	void SplatMapContainer::CreateTextureFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind)
	{

		HRESULT hr = TextureHelper::CreateShaderResourceViewFromFile(aFilePath, aResourceToBind);

		Engine::GetInstance()->SetDebugName((*aResourceToBind), "SplatMapContainer::aResourceToBind");
		DL_ASSERT_EXP(hr == S_OK, "Failed to CreateTextureFromFile in splatcontainer. [" + aFilePath + "]");
	}
}