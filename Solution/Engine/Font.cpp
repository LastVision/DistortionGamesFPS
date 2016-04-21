#include "stdafx.h"
#include "Font.h"


Prism::Font::Font(const std::string& aFilePath, const CU::Vector2<int>& aTextureSize)
	: myFilePath(aFilePath)
	, myTextureSize(aTextureSize)
{
	ParseFontFile(myChars, myFilePath, myTextureSize.x, myTextureSize.y);
	std::string texturePath(aFilePath.begin(), aFilePath.end() - 4);
	texturePath += ".dds";
	myTexture = TextureContainer::GetInstance()->GetTexture(texturePath);
}

Prism::CharacterData Prism::Font::GetCharData(unsigned char aChar) const
{
	fontChar theChar = myChars[aChar];
	CharacterData data;

	data.myTopLeftUV = { float(theChar.x_) / myTextureSize.x, float(theChar.y_) / myTextureSize.y };
	
	data.myBottomRightUV = { float(theChar.x_ + theChar.width_) / myTextureSize.x
		, float(theChar.y_ + theChar.height_) / myTextureSize.y };

	data.mySize= { -float(theChar.width_)
		, float(theChar.height_) };
	data.myOffset = { -theChar.xoffset_, theChar.yoffset_ };
	data.myAdvanceX = -theChar.xadvance_;
	return data;
}