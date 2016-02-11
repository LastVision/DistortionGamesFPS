#include "stdafx.h"
#include "XMLFunctions.h"

static std::string locFilename;

tinyxml2::XMLElement* CU::XMLFunctions::GetDocument(const std::string& aFileName, tinyxml2::XMLDocument& aDocument)
{
	std::string cleanFileName = aFileName;
	tinyxml2::XMLElement* root;

	char charsToRemove[] = "\n\t";
	for (unsigned int i = 0; i < strlen(charsToRemove); i++)
	{
		cleanFileName.erase(std::remove(
			cleanFileName.begin(), cleanFileName.end(),
			charsToRemove[i]), cleanFileName.end());
	}

	CU::XMLFunctions::XMLErrorCheck(aDocument.LoadFile(cleanFileName.c_str()));

	root = aDocument.FirstChildElement();
	if (root == nullptr)
	{
		CU::XMLFunctions::XMLErrorCheck(tinyxml2::XML_ERROR_FILE_READ_ERROR);
	}

	locFilename = cleanFileName;

	return root;
}

void CU::XMLFunctions::XMLErrorCheck(const tinyxml2::XMLError anXMLError, const std::string &aMessage, const std::string &aFileName)
{
	std::string errorMessage;
	std::string errorCode;
	if (aFileName != "")
	{
		locFilename = aFileName;
	}
	switch (anXMLError)
	{
	case tinyxml2::XML_SUCCESS:
		errorCode = "XML_SUCCESS";
		break;
	case tinyxml2::XML_NO_ATTRIBUTE:
		errorCode = "XML_NO_ATTRIBUTE";
		break;
	case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
		errorCode = "XML_WRONG_ATTRIBUTE_TYPE";
		break;
	case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
		errorCode = "XML_ERROR_FILE_NOT_FOUND";
		break;
	case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		errorCode = "XML_ERROR_FILE_COULD_NOT_BE_OPENED";
		break;
	case tinyxml2::XML_ERROR_FILE_READ_ERROR:
		errorCode = "XML_ERROR_FILE_READ_ERROR";
		break;
	case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
		errorCode = "XML_ERROR_ELEMENT_MISMATCH";
		break;
	case tinyxml2::XML_ERROR_PARSING_ELEMENT:
		errorCode = "XML_ERROR_ELEMENT_MISMATCH";
		break;
	case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
		errorCode = "XML_ERROR_PARSING_ATTRIBUTE";
		break;
	case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
		errorCode = "XML_ERROR_IDENTIFYING_TAG";
		break;
	case tinyxml2::XML_ERROR_PARSING_TEXT:
		errorCode = "XML_ERROR_PARSING_TEXT";
		break;
	case tinyxml2::XML_ERROR_PARSING_CDATA:
		errorCode = "XML_ERROR_PARSING_CDATA";
		break;
	case tinyxml2::XML_ERROR_PARSING_COMMENT:
		errorCode = "XML_ERROR_PARSING_COMMENT";
		break;
	case tinyxml2::XML_ERROR_PARSING_DECLARATION:
		errorCode = "XML_ERROR_PARSING_DECLARATION";
		break;
	case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
		errorCode = "XML_ERROR_PARSING_UNKNOWN";
		break;
	case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
		errorCode = "XML_ERROR_EMPTY_DOCUMENT";
		break;
	case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
		errorCode = "XML_ERROR_MISMATCHED_ELEMENT";
		break;
	case tinyxml2::XML_ERROR_PARSING:
		errorCode = "XML_ERROR_PARSING";
		break;
	case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
		errorCode = "XML_CAN_NOT_CONVERT_TEXT";
		break;
	case tinyxml2::XML_NO_TEXT_NODE:
		errorCode = "XML_NO_TEXT_NODE";
		break;
	case tinyxml2::XML_ERROR_COUNT:
		errorCode = "XML_ERROR_COUNT";
		break;
	default:
		break;
	}
	if (errorCode != "XML_SUCCESS")
	{
		if (locFilename == "" && aMessage == "")
		{
			errorMessage = errorCode;
		}
		else if (aMessage == "")
		{
			errorMessage = "XML_NO_ATTRIBUTE\nFile: " + locFilename;
		}
		else
		{
			errorMessage = "XML_NO_ATTRIBUTE\nFile: " + locFilename + "\nMessage: " + aMessage;
		}
		DL_ASSERT(errorMessage.c_str());
	}
}

bool CU::XMLFunctions::XMLCompareName(const char *aFirstName, const char *aSecondName)
{
	if (strcmp(aFirstName, aSecondName) == 0)
	{
		return true;
	}
	return false;
}