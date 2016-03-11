#pragma once
#include <GrowingArray.h>
#include <unordered_map>

class SharedLevel;
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

class SharedLevelFactory
{
public:
	SharedLevelFactory(const std::string& aLevelListPath);
	~SharedLevelFactory();

	SharedLevel* LoadLevel(const int& aID);
	virtual SharedLevel* LoadCurrentLevel() = 0;
	//SharedLevel* LoadNextLevel();

	//bool IsLastLevel();

protected:
	void ReadLeveList(const std::string& aLevelListPath);
	virtual void ReadLevel(const std::string& aLevelPath);

	virtual void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;

	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement,
		CU::Vector3f& aOutPosition, CU::Vector3f& aOutRotation, CU::Vector3f& aOutScale);

	SharedLevel* myCurrentLevel;
	int myCurrentID;

	std::unordered_map<int, std::string> myLevelPaths;
};

//inline bool SharedLevelFactory::IsLastLevel()
//{
//	return myCurrentID >= myLevelPaths.Size();
//}
