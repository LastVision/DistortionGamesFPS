#include "stdafx.h"

#include "CommonHelper.h"
#include "FileWatcher.h"
#include "Texture.h"
#include "TextureContainer.h"

namespace Prism
{
	TextureContainer* TextureContainer::myInstance = nullptr;

	TextureContainer* TextureContainer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new TextureContainer();
		}

		return myInstance;
	}

	void TextureContainer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	TextureContainer::~TextureContainer()
	{
		for (auto it = myTextures.begin(); it != myTextures.end(); ++it)
		{
			delete it->second;
		}

		myTextures.clear();
	}

	Texture* TextureContainer::GetTexture(const std::string& aFileName)
	{
		auto it = myTextures.find(aFileName);

		if (it == myTextures.end())
		{
			LoadTexture(aFileName);
		}

		return myTextures[aFileName];
	}

	void TextureContainer::LoadTexture(const std::string& aFileName)
	{
		Texture* newTex = new Texture();
		if (aFileName.empty() == false)
		{
			const char* fileEnding = &aFileName[aFileName.size() - 3];
			std::string stringEnding(fileEnding);
			CU::ToLower(stringEnding);
			std::string errorMessage = "Texture file-format not .DDS/.dds in [" + aFileName + "].";
			DL_ASSERT_EXP(stringEnding == "dds", errorMessage.c_str());
		}
		newTex->LoadTexture(aFileName);

		myTextures[aFileName] = newTex;

#ifdef DLL_EXPORT
		WATCH_FILE(aFileName, Prism::TextureContainer::ReloadTexture);
#endif
	}

	void TextureContainer::ReloadTexture(const std::string& aFileName)
	{
		myTextures[aFileName]->Release();
		myTextures[aFileName]->LoadTexture(aFileName);
	}
}