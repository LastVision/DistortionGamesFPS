#pragma once
#include <Vector.h>

namespace Prism
{
	class Texture;

	class DebugFont
	{
	public:
		struct CharacterData
		{
			CU::Vector2<float> myTopLeftUV;
			CU::Vector2<float> myBottomRightUV;
		};

		DebugFont();

		bool Init(const char* aFontPath);
		CharacterData GetCharData(char aChar);

		const CU::Vector2<float>& GetCharSize() const;
		Texture* GetTexture();

	private:
		CU::Vector2<float> myCharSize;
		Texture* myTexture;
	};
}

inline const CU::Vector2<float>& Prism::DebugFont::GetCharSize() const
{
	return myCharSize;
}

inline Prism::Texture* Prism::DebugFont::GetTexture()
{
	return myTexture;
}