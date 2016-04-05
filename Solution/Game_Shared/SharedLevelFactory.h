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

	//SharedLevel* LoadLevel(const int& aID);
	//virtual SharedLevel* LoadCurrentLevel() = 0;
	//SharedLevel* LoadNextLevel();

	//bool IsLastLevel();
	const std::string& GetLevelPath(const int aLevelID) const;

	const CU::Vector3<float> GetMinPoint() const;
	const CU::Vector3<float> GetMaxPoint() const;

	const bool& GetIsLoadingLevel() const;

protected:
	void ReadLeveList(const std::string& aLevelListPath);
	virtual void ReadLevel(const std::string& aLevelPath);

	virtual void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadDoors(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadPlayerStartPosition(XMLReader& aReader, tinyxml2::XMLElement* aElement) = 0;
	virtual void LoadSpawnpoint(XMLReader& aReader, tinyxml2::XMLElement* anElement);

	void ReadGID(XMLReader& aReader, tinyxml2::XMLElement* aElement, unsigned int& aOutGID);
	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement,
		CU::Vector3f& aOutPosition, CU::Vector3f& aOutRotation, CU::Vector3f& aOutScale);

	int myCurrentID;

	bool myIsLoadingLevel;

	std::unordered_map<int, std::string> myLevelPaths;

	CU::Vector3<float> myMinPoint;
	CU::Vector3<float> myMaxPoint;
};

//inline bool SharedLevelFactory::IsLastLevel()
//{
//	return myCurrentID >= myLevelPaths.Size();
//}


inline const CU::Vector3<float> SharedLevelFactory::GetMinPoint() const
{
	return myMinPoint;
}

inline const CU::Vector3<float> SharedLevelFactory::GetMaxPoint() const
{
	return myMaxPoint;
}

inline const std::string& SharedLevelFactory::GetLevelPath(const int aLevelID) const
{
	return myLevelPaths.at(aLevelID);
}

inline const bool& SharedLevelFactory::GetIsLoadingLevel() const
{
	return myIsLoadingLevel;
}