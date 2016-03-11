#pragma once

#include "CharacterData.h"
#include <string.h>

namespace Prism
{ 
	class Font;
	class FontProxy
	{
	public:
		FontProxy(const std::string& aFilePath);
		~FontProxy();

		Texture* GetTexture() const;
		CharacterData GetCharData(unsigned char aChar) const;
		const std::string& GetFilePath() const;

		bool IsLoaded() const;
		void SetFont(Font* aFont);

	private:
		void operator=(FontProxy&) = delete;

		Font* myFont;
		std::string myFilePath;
	};
}