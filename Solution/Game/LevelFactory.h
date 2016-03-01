#pragma once
#include <GrowingArray.h>

class Level;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

struct LevelPathInformation
{
	LevelPathInformation() : myID(-1), myPath("No Level") {};
	LevelPathInformation(const int aID, const std::string& aPath) : myID(aID), myPath(aPath) {};
	int myID;
	std::string myPath;
};

class LevelFactory
{
public:
	LevelFactory(const std::string& aLevelListPath);
	~LevelFactory();

	Level* LoadLevel(const int& aID);
	Level* LoadCurrentLevel();
	Level* LoadNextLevel();

	bool IsLastLevel();

private:
	void ReadLeveList(const std::string& aLevelListPath);
	void ReadLevel(const std::string& aLevelPath);

	void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement, 
		CU::Vector3f& aOutPosition, CU::Vector3f& aOutRotation, CU::Vector3f& aOutScale);

	Level* myCurrentLevel;
	int myCurrentID;

	CU::GrowingArray<LevelPathInformation> myLevelPaths;
};

inline bool LevelFactory::IsLastLevel()
{
	return myCurrentID >= myLevelPaths.Size();
}
