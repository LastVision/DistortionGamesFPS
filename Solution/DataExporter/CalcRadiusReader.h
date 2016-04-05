#pragma once
#include <string>
#include <FBXFactory.h>
#include "IReader.h"

class CalcRadiusReader : public IReader
{
public:
	CalcRadiusReader();
	~CalcRadiusReader();
	void ReadFile(const std::string& aFilePath) override;

private:
	float CalcMaxDistance(const CU::GrowingArray<CU::Vector3<float>>& someVertices) const;
	void WriteXml(const std::string& aFbxPath, float aRadius) const;
	Prism::FBXFactory myFactory;

	CU::GrowingArray<std::string> myErrors;
	CU::GrowingArray<CU::Vector3<float>> myVertices;
};

