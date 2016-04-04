#pragma once
#include <algorithm>
#include <DL_Debug.h>
#include "Matrix.h"
#include <string>
#include "Vector.h"
#include <ShlObj.h>
#include <sstream>
#include <fstream>
#include <streambuf>

namespace CU
{
	const std::string group("/Distortion Games/");
	//const std::string game("/Patrick of the Caribbean/");
	const std::string game("");

	static bool canSave;

	inline bool dirExists(const std::string& dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!

		return false;    // this is not a directory!
	}

	inline void BuildFoldersInPath(const std::string& aPath)
	{
		unsigned int slashIndex = aPath.find_first_of("/");

		while (slashIndex != std::string::npos)
		{
			std::string folder(aPath.begin(), aPath.begin() + slashIndex);
			CreateDirectory(folder.c_str(), NULL);
			slashIndex = aPath.find_first_of("/", slashIndex + 1);
		}
	}

	inline bool FileExists(const std::string& name) {
		std::ifstream f(name.c_str());
		if (f.good()) {
			f.close();
			return true;
		}
		f.close();
		return false;
	}

	inline void CreateMyDocumentsFolderPath()
	{
		char documents[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);
		if (hr != S_OK)
		{
			canSave = false;
			DL_ASSERT("Can't get document folder!");
		}

		if (dirExists(documents + group + game) == false)
		{
			BuildFoldersInPath(documents + group + game);
		}
	}

	inline void CreateFileIfNotExists(const std::string& aFilePath)
	{
		if (FileExists(aFilePath) == false)
		{
			std::ofstream outFile(aFilePath.c_str());
			//outFile << "apa";
			outFile.close();
		}
	}

	inline std::string GetMyDocumentFolderPath()
	{
		char documents[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);
		if (hr != S_OK)
		{
			DL_ASSERT("Can't get document folder!");
		}
		std::stringstream ss;
		ss << documents;
		ss << group << game;
		return ss.str();
	}

	inline std::string ToLower(const std::string& aString)
	{
		std::string data = aString;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}

	inline std::string ToLower(const char* aString)
	{
		std::string data = aString;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}

	inline float Clip(float aNumber, float aLower, float aUpper)
	{
		return fmax(aLower, fmin(aNumber, aUpper));
	}

	inline int ClipInt(int aNumber, int aLower, int aUpper)
	{
		return max(aLower, min(aNumber, aUpper));
	}

	inline std::string GetSubString(std::string aStringToReadFrom, char aCharToFind, bool aReadAfterChar)
	{
		std::string toReturn;
		if (aReadAfterChar == false)
		{
			toReturn = aStringToReadFrom.substr(0, aStringToReadFrom.find(aCharToFind));
		}
		else if (aReadAfterChar == true)
		{
			toReturn = aStringToReadFrom.substr(aStringToReadFrom.find_first_of(aCharToFind) + 1);
		}

		return toReturn;
	}

	inline std::string GetSubString(const std::string& aStringToReadFrom, const std::string& aToFind, bool aReadCharactersBeforeToFind
		, int someCharsToSkip = 0)
	{
		if (aStringToReadFrom.find(aToFind) != std::string::npos)
		{
			if (aReadCharactersBeforeToFind == true)
			{
				return aStringToReadFrom.substr(0, aStringToReadFrom.find(aToFind));
			}

			return aStringToReadFrom.substr(aStringToReadFrom.find(aToFind) + someCharsToSkip);

		}

		return aStringToReadFrom;
	}

	inline bool CheckSubString(const std::string& aString, const std::string& aToFind)
	{
		if (aString.find(aToFind) != aString.npos)
			return true;

		return false;
	}



	//If OptionalExtension is blank, the outputstring will have the same extension as the input string
	//OptionalExtension needs to be entered without a period, "xml", NOT ".xml"
	inline std::string GetGeneratedDataFolderFilePath(const std::string& aFilePath, const std::string& anOptionalNewExtension = "")
	{
		std::string pathWithoutData(aFilePath.begin() + 5, aFilePath.end());

		if (anOptionalNewExtension != "")
		{
			int extensionIndex = pathWithoutData.find_last_of(".");
			pathWithoutData = std::string(pathWithoutData.begin(), pathWithoutData.begin() + extensionIndex + 1);
			pathWithoutData += anOptionalNewExtension;
		}

		std::string generatedDataFilePath = "GeneratedData/";
		generatedDataFilePath += pathWithoutData;

		return generatedDataFilePath;
	}

	//If OptionalExtension is blank, the outputstring will have the same extension as the input string
	//OptionalExtension needs to be entered without a period, "xml", NOT ".xml"
	inline std::string GetRealDataFolderFilePath(const std::string& aFilePath, const std::string& anOptionalNewExtension = "")
	{
		std::string pathWithoutData(aFilePath.begin() + 14, aFilePath.end());

		if (anOptionalNewExtension != "")
		{
			int extensionIndex = pathWithoutData.find_last_of(".");
			pathWithoutData = std::string(pathWithoutData.begin(), pathWithoutData.begin() + extensionIndex + 1);
			pathWithoutData += anOptionalNewExtension;
		}

		std::string realDataFilePath = "Data/";
		realDataFilePath += pathWithoutData;

		return realDataFilePath;
	}

	inline std::string Concatenate(const char* aFormattedString, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, aFormattedString);
		vsprintf_s(buffer, aFormattedString, args);
		perror(buffer);
		va_end(args);

		return buffer;
	}

	inline float StringToFloat(const std::string& aString)
	{
		const char* value = aString.c_str();
		char* end;
		float floatValue = strtof(value, &end);

		DL_ASSERT_EXP(value != end, "Error reading float value: " + aString);
		return floatValue;
	}

	inline int StringToInt(const std::string& aString)
	{
		const char* value = aString.c_str();
		char* end;
		int intValue = strtol(value, &end, 10);

		DL_ASSERT_EXP(value != end, "Error reading int value: " + aString);
		return intValue;
	}

	inline void TrimWhiteSpacesAtBeginAndEnd(std::string& aString)
	{
		if (aString.length() <= 0)
		{
			return;
		}

		unsigned int begin = 0;
		while (aString[begin] == ' ')
		{
			++begin;
		}

		unsigned int end = aString.length();
		if (end > begin)
		{
			while (aString[end - 1] == ' ')
			{
				--end;
			}
		}
		if (begin != 0 || end != aString.length())
		{
			aString = std::string(aString.begin() + begin, aString.begin() + end);
		}
	}


	inline Matrix44<float> GetOrientation(Matrix44<float>& aMatrixToRotate, const Vector3<float>& aRotationInDegrees)
	{
		//to radians:
		Vector3<float> rotation(aRotationInDegrees);
		rotation *= 0.0174532925f;

		aMatrixToRotate = Matrix44<float>::CreateRotateAroundZ(rotation.z) * aMatrixToRotate;
		aMatrixToRotate = Matrix44<float>::CreateRotateAroundY(rotation.y) * aMatrixToRotate;
		aMatrixToRotate = Matrix44<float>::CreateRotateAroundX(rotation.x) * aMatrixToRotate;

		return aMatrixToRotate;
	}

	inline bool IsValidTextureSize(UINT aValue)
	{
		return aValue == 2 || aValue == 4 || aValue == 8 || aValue == 16 || aValue == 32 || aValue == 64 || aValue == 128
			|| aValue == 256 || aValue == 512 || aValue == 1024 || aValue == 2048 || aValue == 4096 || aValue == 8192
			|| aValue == 1024 * 6;
	}

	inline const std::string ReadFileToString(const std::string& aFilePath)
	{
		std::string toReturn;
		std::ifstream file(aFilePath);

		file.seekg(0, std::ios::end);
		toReturn.reserve(static_cast<unsigned int>(file.tellg()));
		file.seekg(0, std::ios::beg);

		toReturn.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		file.close();
		return toReturn;
	}
}