#include "stdafx.h"

#include "Font.h"
#include "FontProxy.h"


namespace Prism
{
	FontProxy::FontProxy(const std::string& aFilePath)
		: myFont(nullptr)
	{
		std::string texturePath(aFilePath.begin(), aFilePath.end() - 4);
		texturePath += ".dds";
		myFilePath = texturePath;
	}


	FontProxy::~FontProxy()
	{
		SAFE_DELETE(myFont);
	}

	Texture* FontProxy::GetTexture() const
	{
		if (IsLoaded() == true)
		{
			return myFont->GetTexture();
		}

		return nullptr;
	}

	CharacterData FontProxy::GetCharData(unsigned char aChar) const
	{
		if (IsLoaded() == true)
		{
			return myFont->GetCharData(aChar);
		}

		return CharacterData();
	}

	const std::string& FontProxy::GetFilePath() const
	{
		return myFilePath;
	}

	bool FontProxy::IsLoaded() const
	{
		return myFont != nullptr;
	}

	void FontProxy::SetFont(Font* aFont)
	{
		myFont = aFont;
	}
}