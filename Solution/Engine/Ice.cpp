#include "stdafx.h"
#include "Camera.h"
#include <d3dx11effect.h>
#include "Ice.h"
#include <MathHelper.h>
#include "Surface.h"
#include "TextureHelper.h"
#include "VertexIndexWrapper.h"
#include "VertexDataWrapper.h"

namespace Prism
{
	Ice::Ice(Effect* aEffect, const CU::Vector2<float>& aSize, float aHeight, const std::string& aFilePath)
		: mySize(aSize)
		, myHeight(aHeight)
	{
		myEffect = aEffect;

		mySplatAlbedo = myEffect->GetEffect()->GetVariableByName("myAlbedoTextures")->AsShaderResource();
		if (mySplatAlbedo->IsValid() == false)
		{
			DL_ASSERT("IceAlbedos Failed to Load.");
		}
		mySplatMetalness = myEffect->GetEffect()->GetVariableByName("myMetalnessTextures")->AsShaderResource();
		if (mySplatMetalness->IsValid() == false)
		{
			DL_ASSERT("IceMetalness Failed to Load.");
		}
		mySplatRoughness = myEffect->GetEffect()->GetVariableByName("myRoughnessTextures")->AsShaderResource();
		if (mySplatRoughness->IsValid() == false)
		{
			DL_ASSERT("IceRoughness Failed to Load.");
		}
		mySplatNormal = myEffect->GetEffect()->GetVariableByName("myNormalTextures")->AsShaderResource();
		if (mySplatNormal->IsValid() == false)
		{
			DL_ASSERT("IceNormal Failed to Load.");
		}
		mySplatAmbientOcclusion = myEffect->GetEffect()->GetVariableByName("myAmbientOcclusionTextures")->AsShaderResource();
		if (mySplatAmbientOcclusion->IsValid() == false)
		{
			DL_ASSERT("IceOcclusion Failed to Load.");
		}

		CreateTextures();

		D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};


		InitInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), "Ice::InputLayout");
		InitVertexBuffer(sizeof(IceVertex), D3D11_USAGE_IMMUTABLE, 0);

		InitIndexBuffer();
		InitSurface("AlbedoTexture", aFilePath);

		InitBlendState("Ice::BlendState");

		ZeroMemory(myInitData, sizeof(*myInitData));

		CreateVertices();

	}

	Ice::~Ice()
	{
		SAFE_RELEASE(myAlbedoTextures[0]);
		SAFE_RELEASE(myAlbedoTextures[1]);
		//SAFE_RELEASE(myAlbedoTextures[2]);
		//SAFE_RELEASE(myAlbedoTextures[3]);
		//SAFE_RELEASE(myAlbedoTextures[4]);

		SAFE_RELEASE(myMetalnessTextures[0]);
		SAFE_RELEASE(myMetalnessTextures[1]);
		//SAFE_RELEASE(myMetalnessTextures[2]);
		//SAFE_RELEASE(myMetalnessTextures[3]);
		//SAFE_RELEASE(myMetalnessTextures[4]);

		SAFE_RELEASE(myRoughnessTextures[0]);
		SAFE_RELEASE(myRoughnessTextures[1]);
		//SAFE_RELEASE(myRoughnessTextures[2]);
		//SAFE_RELEASE(myRoughnessTextures[3]);
		//SAFE_RELEASE(myRoughnessTextures[4]);

		SAFE_RELEASE(myNormalTextures[0]);
		SAFE_RELEASE(myNormalTextures[1]);
		//SAFE_RELEASE(myNormalTextures[2]);
		//SAFE_RELEASE(myNormalTextures[3]);
		//SAFE_RELEASE(myNormalTextures[4]);

		SAFE_RELEASE(myAmbientOcclusionTextures[0]);
		SAFE_RELEASE(myAmbientOcclusionTextures[1]);
		//SAFE_RELEASE(myAmbientOcclusionTextures[2]);
		//SAFE_RELEASE(myAmbientOcclusionTextures[3]);
		//SAFE_RELEASE(myAmbientOcclusionTextures[4]);

		SAFE_DELETE(myIndexBaseData);
		SAFE_DELETE(myVertexBaseData);
	}

	void Ice::Render(const Camera& aCamera)
	{
		myEffect->SetWorldMatrix(CU::Matrix44<float>());
		myEffect->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
		BaseModel::Render(false);
	}

	void Ice::SetTextures()
	{
		mySplatAlbedo->SetResourceArray(&myAlbedoTextures[0], 0, 2);
		mySplatMetalness->SetResourceArray(&myMetalnessTextures[0], 0, 2);
		mySplatRoughness->SetResourceArray(&myRoughnessTextures[0], 0, 2);
		mySplatNormal->SetResourceArray(&myNormalTextures[0], 0, 2);
		mySplatAmbientOcclusion->SetResourceArray(&myAmbientOcclusionTextures[0], 0, 2);
	}

	void Ice::CreateTextureFromFile(const std::string& aFilePath, ID3D11ShaderResourceView** aResourceToBind)
	{

		HRESULT hr = TextureHelper::CreateShaderResourceViewFromFile(aFilePath, aResourceToBind);

		Engine::GetInstance()->SetDebugName((*aResourceToBind), "Ice::aResourceToBind");
		DL_ASSERT_EXP(hr == S_OK, "Failed to CreateTextureFromFile in Ice. [" + aFilePath + "]");
	}

	void Ice::CreateVertices()
	{
		CU::GrowingArray<IceVertex> vertices(4);
		CU::GrowingArray<int> indices(6);
		
		
		IceVertex vertex;
		vertex.myPosition = { 0, myHeight, 0, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 0, 1 };
		vertices.Add(vertex);

		vertex.myPosition = { mySize.x, myHeight, 0, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 1, 1 };
		vertices.Add(vertex);

		vertex.myPosition = { mySize.x, myHeight, mySize.y, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 1, 0 };
		vertices.Add(vertex);

		vertex.myPosition = { 0, myHeight, mySize.y, 0 };
		vertex.myNormal = { 0, 1, 0, 0 };
		vertex.myUV = { 0, 0 };
		vertices.Add(vertex);


		CU::Matrix33<float> rotationMatrix;
		rotationMatrix = rotationMatrix * CU::Matrix33<float>::CreateRotateAroundX(CU_PI / 2.f);
		rotationMatrix = rotationMatrix * CU::Matrix33<float>::CreateRotateAroundZ(CU_PI / 2.f);

		for (int i = 0; i < vertices.Size(); ++i)
		{
			//vertices[i].myTangent = CU::GetNormalized(CU::Cross(vertices[i].myNormal, vertices[i].myNormal * rotationMatrix));
			//vertices[i].myBiNormal = CU::GetNormalized(CU::Cross(vertices[i].myTangent, vertices[i].myNormal));

			vertices[i].myTangent = CU::Vector4<float>(1.f, 0.f, 0.f, 0);
			vertices[i].myBiNormal = CU::Vector4<float>(0.f, 0.f, 1.f, 0);
		}


		indices.Add(3);
		indices.Add(1);
		indices.Add(0);

		indices.Add(2);
		indices.Add(1);
		indices.Add(3);

		SetupVertexBuffer(vertices.Size(), sizeof(IceVertex), reinterpret_cast<char*>(&vertices[0])
			, "Ice::VertexBuffer");
		SetupIndexBuffer(indices.Size(), reinterpret_cast<char*>(&indices[0]), "Ice::IndexBuffer");

		mySurfaces[0]->SetVertexCount(vertices.Size());
		mySurfaces[0]->SetIndexCount(indices.Size());
	}

	void Ice::CreateTextures()
	{
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_base/T_ice_splat_base_albedo.dds", &myAlbedoTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_r/T_ice_splat_r_albedo.dds", &myAlbedoTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_albedo.dds", &myAlbedoTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_albedo.dds", &myAlbedoTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_albedo.dds", &myAlbedoTextures[4]);

		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_base/T_ice_splat_base_metalness.dds", &myMetalnessTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_r/T_ice_splat_r_metalness.dds", &myMetalnessTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_metalness.dds", &myMetalnessTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_metalness.dds", &myMetalnessTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_metalness.dds", &myMetalnessTextures[4]);

		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_base/T_ice_splat_base_roughness.dds", &myRoughnessTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_r/T_ice_splat_r_roughness.dds", &myRoughnessTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_roughness.dds", &myRoughnessTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_roughness.dds", &myRoughnessTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_roughness.dds", &myRoughnessTextures[4]);

		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_base/T_ice_splat_base_normal.dds", &myNormalTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_r/T_ice_splat_r_normal.dds", &myNormalTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_normal.dds", &myNormalTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_normal.dds", &myNormalTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_normal.dds", &myNormalTextures[4]);

		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_base/T_ice_splat_base_ao.dds", &myAmbientOcclusionTextures[0]);
		CreateTextureFromFile("Data/Resource/Texture/Terrain/Ice/Ice_splat_r/T_ice_splat_r_ao.dds", &myAmbientOcclusionTextures[1]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_g/T_splat_g_ao.dds", &myAmbientOcclusionTextures[2]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_b/T_splat_b_ao.dds", &myAmbientOcclusionTextures[3]);
		//CreateTextureFromFile("Data/Resource/Texture/Terrain/SplatMap/splat_a/T_splat_a_ao.dds", &myAmbientOcclusionTextures[4]);
	}
}