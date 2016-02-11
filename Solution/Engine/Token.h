#pragma once

#include <string>

namespace Prism
{
	class Token
	{
	public:
		Token(int aTokenId, const std::string& aTokenAsString);
		virtual ~Token() {};
		int myTokenId;
		int myMatchPosition;
		std::string myTokenAsString;
	};
}