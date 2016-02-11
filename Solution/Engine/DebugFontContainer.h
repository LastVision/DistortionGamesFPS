#pragma once
#include <unordered_map>

namespace Prism
{
	class DebugFont;
	class DebugFontContainer
	{
	public:
		DebugFont* GetFont(const std::string& aPath);

	private:
		void LoadFont(const std::string& aPath);

		std::unordered_map<std::string, DebugFont*> myFonts;
	};
}