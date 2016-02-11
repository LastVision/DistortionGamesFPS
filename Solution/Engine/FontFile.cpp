#include "stdafx.h"
#include <CommonHelper.h>
#include "FontFile.h"
#include "textfile.h"
#include "textparser.h"
#include <sstream>

// "VeraMoBI.ttf_font.txt"
// char id=32    x=486   y=505   width=4     height=4     xoffset=-1.500    yoffset=1.500     xadvance=30.688      page=0  chnl=0
enum T_TokenID
{
	TOKEN_NA,
	TOKEN_ID = 1,
	TOKEN_X,
	TOKEN_Y,
	TOKEN_WIDTH,
	TOKEN_HEIGHT,
	TOKEN_XOFFSET,
	TOKEN_YOFFSET,
	TOKEN_XADVANCE,
	TOKEN_PAGE,
	TOKEN_CHNL,
};

bool Prism::ParseFontFile(std::vector<fontChar> &fontChars, const std::string &fontfilename, uint texwidth, uint texheight)
{
	unsigned char buffer;
	TextParser parser;
	parser.Add((int)TOKEN_ID, "id=");
	parser.Add((int)TOKEN_X, "x=");
	parser.Add((int)TOKEN_Y, "y=");
	parser.Add((int)TOKEN_WIDTH, "width=");
	parser.Add((int)TOKEN_HEIGHT, "height=");
	parser.Add((int)TOKEN_XOFFSET, "xoffset=");
	parser.Add((int)TOKEN_YOFFSET, "yoffset=");
	parser.Add((int)TOKEN_XADVANCE, "xadvance=");
	parser.Add((int)TOKEN_PAGE, "page=");
	parser.Add((int)TOKEN_CHNL, "chnl=");
	parser.Reset();

	uint id = 0;
	uint x = 0;
	uint y = 0;
	uint width = 0;
	uint height = 0;

	float	xoffset = 0;
	float	yoffset = 0;
	float	xadvance = 0;
	uint	chnl = 0;
	uint page = 0;

	// grab the file data, store it and free the new'd block
	char * pFontFile;
	pFontFile = TextFileRead(fontfilename.c_str());
	if (!pFontFile)
	{
		return false;
	}

	std::string allofit(pFontFile);
	delete(pFontFile);

	// process the data
	for (int i = 0; i< 256; i++)
	{
		fontChars.push_back(fontChar());
	}
	std::string::const_iterator it = allofit.begin();
	std::string objectStr;
	while (it != allofit.end())
	{
		buffer = (*it);
		T_TokenID currentToken = (T_TokenID)parser.CheckForToken(buffer);
		if (NO_TOKEN != currentToken && buffer == '=')
		{
			++it; // there should never be an '=' before the end of the file ;)
			buffer = (*it);
		}

		switch (currentToken)
		{
		case NO_TOKEN:
			break; // do nothing but continue searching for a token

		case TOKEN_ID:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			id = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_X:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			x = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_Y:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			y = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_WIDTH:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			width = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_HEIGHT:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			height = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_XOFFSET:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			xoffset = static_cast<float>(CU::StringToFloat(objectStr));
		}
			break;

		case TOKEN_YOFFSET:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			yoffset = static_cast<float>(CU::StringToFloat(objectStr));
		}
			break;

		case TOKEN_XADVANCE:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			xadvance = static_cast<float>(CU::StringToFloat(objectStr));
		}
			break;

		case TOKEN_PAGE:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			page = CU::StringToInt(objectStr);
		}
			break;

		case TOKEN_CHNL:
		{
			// place your code to handle a token TOKEN_XXX here
			objectStr.clear();
			while (it != allofit.end() && buffer != ' ')
			{
				objectStr += buffer;
				++it;   if (it != allofit.end())   buffer = (*it);
			}
			chnl = CU::StringToInt(objectStr);

			// whatever we've got we can now add to the fontChars :) ... probably
			fontChars[id] = fontChar(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl, texwidth, texheight);
		}
			break;

		default:
		{
			return false;
		}
		} // switch ( CheckForToken(buffer) )

		if (it != allofit.end())
			++it;
	}
	return true;
}
