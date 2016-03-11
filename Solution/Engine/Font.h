#pragma once

#include "CharacterData.h"
#include <Vector.h>
#include <vector>
#include "FontFile.h"
#include <string>
#include "Texture.h"

namespace Prism
{
	class Font
	{
	public:
		Font(const std::string& aFilePath, const CU::Vector2<int>& aTextureSize);
		Texture* GetTexture() const;
		CharacterData GetCharData(unsigned char aChar) const;

	private:
		void operator=(Font&) = delete;
		const std::string myFilePath;
		const CU::Vector2<int> myTextureSize;
		std::vector<fontChar> myChars;

		Texture* myTexture;
	};
}

inline Prism::Texture* Prism::Font::GetTexture() const
{
	return myTexture;
}