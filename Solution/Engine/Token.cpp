#include "stdafx.h"
#include "Token.h"


Prism::Token::Token(int aTokenId, const std::string& aTokenAsString)
	: myTokenId(aTokenId)
	, myTokenAsString(aTokenAsString)
	, myMatchPosition(0)
{
}