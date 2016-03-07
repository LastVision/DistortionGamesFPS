#include "stdafx.h"

#include <CommonHelper.h>
#include <d3dx11effect.h>
#include "Engine.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "SpotLightShadow.h"
#include "Texture.h"
#include "TextureContainer.h"

namespace Prism
{
	EffectContainer* EffectContainer::myInstance = nullptr;

	EffectContainer* EffectContainer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new EffectContainer();
		}

		return myInstance;
	}

	void EffectContainer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	EffectContainer::~EffectContainer()
	{
		for (auto it = myEffects.begin(); it != myEffects.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}

		myEffects.clear();
	}

	Effect* EffectContainer::GetEffect(const std::string& aFilePath)
	{
		auto it = myEffects.find(aFilePath);

		if (it == myEffects.end())
		{
			LoadEffect(aFilePath);

			myFilewatcher.WatchFileChangeWithDependencies(aFilePath, std::bind(&EffectContainer::ReloadShader, this, std::placeholders::_1));
		}

		return myEffects[aFilePath];
	}

	void EffectContainer::LoadEffect(const std::string& aFilePath)
	{
		VerifyShader(aFilePath);
		GetCSOPath(aFilePath);
		Effect* newEffect = new Effect();

		if (newEffect->Init(aFilePath) == false)
		{
			std::stringstream ss;
			ss << "Failed to Init Effect: " << aFilePath.c_str() << std::endl;
			DL_MESSAGE_BOX(ss.str().c_str(), "EffectContainer::LoadEffect", MB_ICONWARNING);
			return;
		}

		if (myCubeMap != "")
		{
			Texture* tex = TextureContainer::GetInstance()->GetTexture(myCubeMap);
			ID3DX11EffectShaderResourceVariable* shaderVar = newEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

			if (shaderVar->IsValid())
			{
				shaderVar->SetResource(tex->GetShaderView());
			}
		}

		DL_ASSERT_EXP(newEffect != nullptr, "newEffect is nullpter in LoadEffect, HOW?");

		myEffects[aFilePath] = newEffect;
		myEffectArrays.Add(newEffect);
	}

	void EffectContainer::VerifyShader(const std::string& aFilePath)
	{
		DL_ASSERT_EXP(aFilePath == "Data/Resource/Shader/S_effect_pbl.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_pbl_animated.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_font.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_sprite.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_debug.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_no_texture.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_no_texture_animated.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_no_texture.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_line3d.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_cube3d.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_particle.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_combine.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_down_sample.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_bloom.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_render_to_texture.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_cube_colored.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_pbldebug.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_3dgui.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_deferred_ambient.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_deferred_light_mesh.fx"
			|| aFilePath == "Data/Resource/Shader/S_effect_pbl_deferred.fx"
			, CU::Concatenate("Found invalid Shader: %s", aFilePath.c_str()));
	}

	void EffectContainer::ReloadShader(const std::string& aFilePath)
	{
		if (myEffects.find(aFilePath) != myEffects.end())
		{
			myEffects[aFilePath]->Init(aFilePath);
		}
	}

	std::string EffectContainer::GetCSOPath(const std::string& aFXPath)
	{
		int startIndex = aFXPath.find_last_of('/');
		int endIndex = aFXPath.find_last_of('.');

		std::string path(aFXPath.begin() + startIndex + 1, aFXPath.begin() + endIndex);

		path = "Data/Generated/" + path + ".cso";

		return path;
	}

	void EffectContainer::Update(const float aDeltaTime)
	{
		myFilewatcher.FlushChanges();

		for (int i = 0; i < myEffectArrays.Size(); ++i)
		{
			myEffectArrays[i]->UpdateTime(aDeltaTime);
		}
	}

	void EffectContainer::SetCubeMap(const std::string& aFilePath)
	{
		if (aFilePath != myCubeMap)
		{
			myCubeMap = aFilePath;
			for (int i = 0; i < myEffectArrays.Size(); ++i)
			{
				Texture* tex = TextureContainer::GetInstance()->GetTexture(myCubeMap);
				ID3DX11EffectShaderResourceVariable* shaderVar = myEffectArrays[i]->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

				if (shaderVar->IsValid())
				{
					shaderVar->SetResource(tex->GetShaderView());
				}
			}
		}
	}

	void EffectContainer::SetShadowDepth(SpotLightShadow* aShadowSpotLight)
	{
		for (int i = 0; i < myEffectArrays.Size(); ++i)
		{
			myEffectArrays[i]->SetShadowMVP(aShadowSpotLight->GetMVP());
			myEffectArrays[i]->SetShadowDepthTexture(aShadowSpotLight->GetTexture());
		}
	}
}