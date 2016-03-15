#include "stdafx.h"

#include "CommonHelper.h"
#include <d3dx11effect.h>
#include "Effect.h"
#include "Surface.h"
#include "Texture.h"
#include "TextureContainer.h"

namespace Prism
{
	Surface::Surface()
		: myPrimitiveTopologyType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		, myVertexStart(0)
		, myVertexCount(0)
		, myIndexStart(0)
		, myIndexCount(0)
		, myEffect(nullptr)
	{
		myOwnerAlbedoTextures[0] = nullptr;
		myOwnerAlbedoTextures[1] = nullptr;
		myOwnerAlbedoTextures[2] = nullptr;

		myResourceViews.Init(8);
		myShaderVariables.Init(8);

		myFilePaths.Init(8);
		myShaderResourceNames.Init(8);
		myEmissive = false;
	}

	Surface::~Surface()
	{
		myResourceViews.RemoveAll();
		myShaderVariables.RemoveAll();

		myFilePaths.RemoveAll();
		myShaderResourceNames.RemoveAll();
	}

	void Surface::SetEffect(Effect* aEffect)
	{
		myEffect = aEffect;

		for (int i = 0; i < myShaderResourceNames.Size(); ++i)
		{
			ID3DX11EffectShaderResourceVariable* shaderVar = myEffect->GetEffect()->GetVariableByName(
				myShaderResourceNames[i].c_str())->AsShaderResource();

			myShaderVariables.Add(shaderVar);
		}
	}

	bool Surface::SetTexture(const std::string& aResourceName, const std::string& aFileName, bool aUseSRGB)
	{
		aUseSRGB;

		DL_ASSERT_EXP(aFileName != ""
			, CU::Concatenate("Shader resource ( %s ) tried to use invalid filePath", aResourceName.c_str()));

		Texture* tex = TextureContainer::GetInstance()->GetTexture(aFileName);

		if (aResourceName == "AlbedoTexture")
		{
			int index = aFileName.find("_neutral.dds");
			if (index != std::string::npos)
			{
				std::string playerAlbedo(aFileName.begin(), aFileName.begin() + index);
				playerAlbedo += "_player.dds";

				std::string enemyAlbedo(aFileName.begin(), aFileName.begin() + index);
				enemyAlbedo += "_enemy.dds";

				myOwnerAlbedoTextures[0] = tex;
				myOwnerAlbedoTextures[1] = TextureContainer::GetInstance()->GetTexture(playerAlbedo);
				myOwnerAlbedoTextures[2] = TextureContainer::GetInstance()->GetTexture(enemyAlbedo);
			}
		}

		if (aResourceName == "EmissiveTexture")
		{
			myEmissive = true;
		}

		myResourceViews.Add(tex->GetShaderView());
		myFilePaths.Add(aFileName);
		myShaderResourceNames.Add(aResourceName);

		return true;
	}

	bool Surface::SetTexture(const std::string& aResourceName, ID3D11ShaderResourceView* aResource)
	{
		ID3DX11EffectShaderResourceVariable* shaderVar = myEffect->GetEffect()->GetVariableByName(aResourceName.c_str())->AsShaderResource();
		if (shaderVar->IsValid() == false)
		{
			RESOURCE_LOG("Failed to get ShaderResource");
			return false;
		}

		myResourceViews.Add(aResource);
		myShaderVariables.Add(shaderVar);
		myShaderResourceNames.Add(aResourceName);
		return true;
	}

	void Surface::ReloadSurface()
	{
		myResourceViews.RemoveAll();
		myShaderVariables.RemoveAll();

		for (int i = 0; i < myFilePaths.Size(); ++i)
		{
			Texture* tex = TextureContainer::GetInstance()->GetTexture(myFilePaths[i]);
			ID3DX11EffectShaderResourceVariable* shaderVar = myEffect->GetEffect()->GetVariableByName(myShaderResourceNames[i].c_str())->AsShaderResource();

			if (shaderVar->IsValid() == false)
			{
				std::string errorMsg = "Failed to get ShaderResource: " + myShaderResourceNames[i];
				RESOURCE_LOG(errorMsg.c_str());
			}

			myResourceViews.Add(tex->GetShaderView());
			myShaderVariables.Add(shaderVar);
		}
	}

	bool Surface::VerifyTextures(const std::string& aModelPath)
	{
		if (aModelPath.find("skySphere") != std::string::npos)
		{
			return true;
		}

		bool diffuse = false;
		bool albedo = false;
		bool normal = false;
		bool roughness = false;
		bool metal = false;
		bool ao = false;
		bool emissiveness = false;

		for (int i = 0; i < myShaderResourceNames.Size(); ++i)
		{
			if (myShaderResourceNames[i] == "DiffiuseTexture")
			{
				diffuse = true;
			}
			else if (myShaderResourceNames[i] == "AlbedoTexture")
			{
				albedo = true;
			}
			else if (myShaderResourceNames[i] == "NormalTexture")
			{
				normal = true;
			}
			else if (myShaderResourceNames[i] == "RoughnessTexture")
			{
				roughness = true;
			}
			else if (myShaderResourceNames[i] == "MetalnessTexture")
			{
				metal = true;
			}
			else if (myShaderResourceNames[i] == "AOTexture")
			{
				ao = true;
			}
			else if (myShaderResourceNames[i] == "EmissiveTexture")
			{
				emissiveness = true;
			}
		}


		if (diffuse == true) return true;

#ifndef _DEBUG
		DL_ASSERT_EXP(albedo == true, CU::Concatenate("Albedo missing from %s", aModelPath.c_str()));
		DL_ASSERT_EXP(normal == true, CU::Concatenate("NormalMap missing from %s", aModelPath.c_str()));
		DL_ASSERT_EXP(roughness == true, CU::Concatenate("Roughness missing from %s", aModelPath.c_str()));
		DL_ASSERT_EXP(metal == true, CU::Concatenate("Metalness missing from %s", aModelPath.c_str()));
		DL_ASSERT_EXP(ao == true, CU::Concatenate("AmbientOcclusion missing from %s", aModelPath.c_str()));
		DL_ASSERT_EXP(emissiveness == true, CU::Concatenate("Emissiveness missing from %s", aModelPath.c_str()));
#endif

		return true;
	}

	void Surface::Activate()
	{
		Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(myPrimitiveTopologyType);

		for (int i = 0; i < myShaderVariables.Size(); ++i)
		{
			DL_ASSERT_EXP(myResourceViews[i] != nullptr, "Surface contains an invalid ResourceView");

			myShaderVariables[i]->SetResource(myResourceViews[i]);
		}
	}

	void Surface::DeActivate()
	{
		for (int i = 0; i < myShaderVariables.Size(); ++i)
		{
			myShaderVariables[i]->SetResource(nullptr);
		}
	}
}