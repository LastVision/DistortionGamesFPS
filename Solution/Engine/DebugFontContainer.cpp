#include "stdafx.h"
#include "DebugFontContainer.h"
#include "DebugFont.h"

Prism::DebugFont* Prism::DebugFontContainer::GetFont(const std::string& aPath)
{
	auto it = myFonts.find(aPath);

	if (it == myFonts.end())
	{
		LoadFont(aPath);
	}

	return myFonts[aPath];
}

void Prism::DebugFontContainer::LoadFont(const std::string& aPath)
{
	DebugFont* newFont = new DebugFont();
	newFont->Init(aPath.c_str());

	myFonts[aPath] = newFont;
}