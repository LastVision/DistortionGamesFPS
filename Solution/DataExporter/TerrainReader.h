#pragma once
#include "IReader.h"
#include <fstream>
#include <XMLReader.h>

namespace Prism
{
	class Terrain;
}

class TerrainReader : public IReader
{
public:
	TerrainReader();
	~TerrainReader();

	void ReadFile(const std::string& aFilePath) override;

private:
	struct NavmeshCutBox
	{
		NavmeshCutBox(const CU::Vector3f& aPosition, const CU::Vector3f& aExtend, const CU::Vector3f& aRotation);
		CU::GrowingArray<CU::Vector2<float>> GetCutMesh() const;
		CU::Vector3f myPosition;
		CU::Vector3f myExtend;
		CU::Vector3f myRotation;
	};

	void ReadLevel(const std::string& aFilePath);
	void LoadTerrain(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadCutBoxes(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);

	void SaveHeigthMap(std::fstream& aStream);
	void SaveTerrain(std::fstream& aStream);

	Prism::Terrain* myTerrain;
	CU::GrowingArray<NavmeshCutBox*> myCutBoxes;
};

