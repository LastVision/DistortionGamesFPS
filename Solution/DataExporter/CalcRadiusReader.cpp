#include "CalcRadiusReader.h"
#include <GrowingArray.h>
#include <DL_Debug.h>
#include <fstream>
#include <iostream>
#include <CommonHelper.h>

void CalcRadiusReader::ReadFile(const std::string& aFilePath)
{
	if (aFilePath.compare(aFilePath.size() - 4, 4, ".fbx") == 0
		|| aFilePath.compare(aFilePath.size() - 4, 4, ".FBX") == 0)
	{
		std::cout << "Reading FBX: " + aFilePath << std::endl;

		CU::GrowingArray<CU::Vector3<float>> vertices(524288);
		myFactory.LoadModelForRadiusCalc(aFilePath.c_str(), vertices);
		float distance = CalcMaxDistance(vertices);

		auto planetPos = aFilePath.find("planet");

		DL_ASSERT_EXP(distance < 1000 || planetPos != std::string::npos, " FBX too huge (vertex position failed).");

		WriteXml(aFilePath, distance);
	}
}

float CalcRadiusReader::CalcMaxDistance(const CU::GrowingArray<CU::Vector3<float>>& someVertices) const
{
	float max = 0;
	int index = 0;
	for (int i = 0; i < someVertices.Size(); ++i)
	{
		float newMax = fmaxf(max, CU::Length2(someVertices[i]));
		if (newMax > max)
		{
			index = i;
			max = newMax;
		}
	}

	return CU::Length(someVertices[index]);
}

void CalcRadiusReader::WriteXml(const std::string& aFbxPath, float aRadius) const
{
	std::fstream file;
	std::string xmlPath = CU::GetGeneratedDataFolderFilePath(aFbxPath, "xml");
	CU::BuildFoldersInPath(xmlPath);
	//std::string xmlPath(aFbxPath.begin(), aFbxPath.end() - 4);
	//xmlPath += ".xml";
	file.open(xmlPath, std::ios::out);

	file << "<root>\n\t<radius value=\"" << aRadius << "\"/>\n</root>";

	file.close();
	std::cout << " radius: " << aRadius << std::endl;
}