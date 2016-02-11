#include "stdafx.h"
#include "DebugFont.h"
#include "TextureContainer.h"


Prism::DebugFont::DebugFont()
	: myCharSize(32.f, 32.f)
	, myTexture(nullptr)
{
}

bool Prism::DebugFont::Init(const char* aFontPath)
{
	myTexture = TextureContainer::GetInstance()->GetTexture(aFontPath);

	return true;
}

Prism::DebugFont::CharacterData Prism::DebugFont::GetCharData(char aChar)
{
	int x = aChar % 16;
	int y = aChar / 16;

	
	CU::Vector2<float> topLeft(x * myCharSize.x, y * myCharSize.y);
	CU::Vector2<float> bottomLeft(topLeft.x + myCharSize.x, topLeft.y + myCharSize.y);


	CharacterData data;
	data.myTopLeftUV = topLeft / 512.f;
	data.myBottomRightUV = bottomLeft / 512.f;

	return data;
}