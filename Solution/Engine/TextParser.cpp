#include "stdafx.h"
#include "TextParser.h"
#include "Token.h"

Prism::TextParser::~TextParser()
{
	myTokens.DeleteAll();
}

bool Prism::TextParser::Add(int tokenID, std::string tokenAsStr)
{
	if (tokenAsStr == "" || tokenID == NO_TOKEN)
	{
		return false;
	}

	Token* pToken = new Token(tokenID, tokenAsStr);

	if (pToken == NULL)
	{
		return false;
	}

	myTokens.Add(pToken);

	return true;
}

int Prism::TextParser::CheckForToken(unsigned char currentChar)
{
	int i = myTokens.Size();
	while (i--) // foreach token
	{
		Token dummyToken = *myTokens[i];
		std::string dummy = myTokens[i]->myTokenAsString;

		if (currentChar == myTokens[i]->myTokenAsString[myTokens[i]->myMatchPosition])
		{ // if the currentChar matches the requested character of the current token,...
			myTokens[i]->myMatchPosition++; // ...increase the "match-position" counter
			if (myTokens[i]->myMatchPosition == (int)myTokens[i]->myTokenAsString.size())
			{ // if the counter equals the length of the current token, we found a token. Thus,...
				//...reset the counters of all the other tokens and...
				this->Reset();

				// ...return the token found
				return myTokens[i]->myTokenId;
			}
		}
		else // if the currentChar does *not* match the requested character of the current token,...
		{
			myTokens[i]->myMatchPosition = 0; // reset the corresponding counter
		}
	}
	return NO_TOKEN;
}

void Prism::TextParser::Reset()
{
	unsigned int i = myTokens.Size();//-1 + 1;
	while (i--)
	{
		myTokens[i]->myMatchPosition = 0;
	}
}