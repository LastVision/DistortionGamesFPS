//#include <CommonHelper.h>
//#include <Defines.h>
////#include <HeightMap.h>
//#include <MathHelper.h>
//#include <NavMesh.h>
//#include "TerrainReader.h"
//#include <Terrain.h>
//#include <XMLReader.h>
//#include <Vertices.h>
//#include <iostream>
//
//TerrainReader::NavmeshCutBox::NavmeshCutBox(const CU::Vector3f& aPosition, const CU::Vector3f& aExtend, const CU::Vector3f& aRotation)
//	: myPosition(aPosition)
//	, myExtend(aExtend / 2.f)
//	, myRotation(aRotation)
//{
//}
//
//CU::GrowingArray<CU::Vector2<float>> TerrainReader::NavmeshCutBox::GetCutMesh() const
//{
//	CU::GrowingArray<CU::Vector2<float>> points(4);
//
//	CU::Vector2<float> pos(myPosition.x, myPosition.z);
//
//	points.Add({ -myExtend.x, -myExtend.z });
//	points.Add({ -myExtend.x, +myExtend.z });
//	points.Add({ +myExtend.x, +myExtend.z });
//	points.Add({ +myExtend.x, -myExtend.z });
//
//	CU::Matrix33<float> rotationMatrix(CU::Matrix33<float>::CreateRotateAroundZ(-myRotation.y));
//
//	for (int i = 0; i < points.Size(); ++i)
//	{
//		points[i] = points[i] * rotationMatrix;
//		points[i] += pos;
//	}
//	return points;
//}
//
//
//
//
//TerrainReader::TerrainReader()
//	: myTerrain(nullptr)
//	, myCutBoxes(2048)
//{
//}
//
//
//TerrainReader::~TerrainReader()
//{
//}
//
//void TerrainReader::ReadFile(const std::string& aFilePath)
//{
//	XMLReader reader;
//	reader.OpenDocument(aFilePath);
//	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
//
//	levelElement = reader.ForceFindFirstChild(levelElement, "level");
//
//	while (levelElement != nullptr)
//	{
//		std::string path;
//		reader.ForceReadAttribute(levelElement, "path", path);
//		std::cout << "Converting: " << path << std::endl;
//
//		ReadLevel(path);
//
//		levelElement = reader.FindNextElement(levelElement, "level");
//	}
//
//	reader.CloseDocument();
//}
//
//void TerrainReader::ReadLevel(const std::string& aFilePath)
//{
//	XMLReader reader;
//	reader.OpenDocument(aFilePath);
//	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
//	levelElement = reader.ForceFindFirstChild(levelElement, "scene");
//
//	LoadTerrain(reader, levelElement);
//	LoadCutBoxes(reader, levelElement);
//
//
//	myTerrain->CreateNavMesh();
//	for (int i = 0; i < myCutBoxes.Size(); ++i)
//	{
//		myTerrain->GetNavMesh()->Cut(myCutBoxes[i]->GetCutMesh());
//	}
//	/*for (int i = 0; i < myCurrentLevel->myEntities.Size(); ++i)
//	{
//	myTerrain->GetNavMesh()->Cut(myCurrentLevel->myEntities[i]->GetCutMesh());
//	}*/
//	myTerrain->GetNavMesh()->CalcHeights(myTerrain);
//
//
//	std::string navPath = CU::GetGeneratedDataFolderFilePath(aFilePath, "nav");
//	CU::BuildFoldersInPath(navPath);
//
//
//	std::fstream file;
//	file.open(navPath, std::ios::out | std::ios::binary);
//	int terrainVersion = TERRAIN_VERSION;
//	file.write((char*)&terrainVersion, sizeof(int));
//
//	SaveHeigthMap(file);
//	SaveTerrain(file);
//
//	myTerrain->GetNavMesh()->Save(file);
//
//	file.close();
//
//
//	reader.CloseDocument();
//
//
//	delete myTerrain;
//	myCutBoxes.DeleteAll();
//}
//
//void TerrainReader::LoadTerrain(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
//{
//	std::string heightMap;
//
//	tinyxml2::XMLElement* terrainElement = aReader.FindFirstChild(aLevelElement, "terrain");
//	aReader.ForceReadAttribute(terrainElement, "heightmap", heightMap);
//
//	myTerrain = new Prism::Terrain(heightMap, { 256.f, 256.f }, 10.f);
//}
//
//void TerrainReader::LoadCutBoxes(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
//{
//	for (tinyxml2::XMLElement* e = aReader.FindFirstChild(aLevelElement); e != nullptr;
//		e = aReader.FindNextElement(e))
//	{
//		std::string elementName = CU::ToLower(e->Name());
//		if (elementName == "navmeshcutbox")
//		{
//			tinyxml2::XMLElement* boxElement = aReader.ForceFindFirstChild(e, "position");
//			CU::Vector3<float> boxPosition;
//			aReader.ForceReadAttribute(boxElement, "X", boxPosition.x);
//			aReader.ForceReadAttribute(boxElement, "Y", boxPosition.y);
//			aReader.ForceReadAttribute(boxElement, "Z", boxPosition.z);
//
//			boxElement = aReader.ForceFindFirstChild(e, "rotation");
//			CU::Vector3<float> boxRotation;
//			aReader.ForceReadAttribute(boxElement, "X", boxRotation.x);
//			aReader.ForceReadAttribute(boxElement, "Y", boxRotation.y);
//			aReader.ForceReadAttribute(boxElement, "Z", boxRotation.z);
//
//			boxElement = aReader.ForceFindFirstChild(e, "scale");
//			CU::Vector3<float> boxScale;
//			aReader.ForceReadAttribute(boxElement, "X", boxScale.x);
//			aReader.ForceReadAttribute(boxElement, "Y", boxScale.y);
//			aReader.ForceReadAttribute(boxElement, "Z", boxScale.z);
//
//			boxRotation.x = CU::Math::DegreeToRad(boxRotation.x);
//			boxRotation.y = CU::Math::DegreeToRad(boxRotation.y);
//			boxRotation.z = CU::Math::DegreeToRad(boxRotation.z);
//
//			NavmeshCutBox* newCutBox = new NavmeshCutBox(boxPosition, boxScale, boxRotation);
//			myCutBoxes.Add(newCutBox);
//		}
//	}
//}
//
//void TerrainReader::SaveHeigthMap(std::fstream& aStream)
//{
//	aStream.write((char*)&myTerrain->myHeightMap->myWidth, sizeof(int));
//	aStream.write((char*)&myTerrain->myHeightMap->myDepth, sizeof(int));
//	aStream.write((char*)myTerrain->myHeightMap->myData, sizeof(char) * myTerrain->myHeightMap->myWidth *
//		myTerrain->myHeightMap->myDepth);
//}
//
//void TerrainReader::SaveTerrain(std::fstream& aStream)
//{
//	aStream.write((char*)&myTerrain->mySize.x, sizeof(float) * 2);
//	aStream.write((char*)&myTerrain->myHeight, sizeof(float));
//
//
//	int vertexCount = myTerrain->myVertices.Size();
//	aStream.write((char*)&vertexCount, sizeof(int));
//	aStream.write((char*)&myTerrain->myVertices[0], sizeof(myTerrain->myVertices[0]) * vertexCount);
//
//
//	int indexCount = myTerrain->myIndices.Size();
//	aStream.write((char*)&indexCount, sizeof(int));
//	aStream.write((char*)&myTerrain->myIndices[0], sizeof(int) * indexCount);
//}