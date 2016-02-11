#pragma once

#include <GrowingArray.h>

#define NO_TOKEN	0

namespace Prism
{
	class Token;

	class TextParser
	{
	public:
		TextParser() : myTokens(255) {};
		~TextParser();
		bool		Add(int tokenID, std::string tokenAsStr);
		int			CheckForToken(unsigned char currentChar);
		void		Reset();
	private:
		CU::GrowingArray<Token*> myTokens;
	};
}