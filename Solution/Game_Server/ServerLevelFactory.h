#pragma once
#include <SharedLevelFactory.h>
class ServerLevelFactory : public SharedLevelFactory
{
public:
	ServerLevelFactory(const std::string& aLevelListPath);
	~ServerLevelFactory();

	ServerLevel* LoadLevel(int aID);
	ServerLevel* LoadCurrentLevel();

private:
	void ReadLevel(const std::string& aLevelPath) override;

	void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadDoors(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadSpawnpoint(XMLReader& aReader, tinyxml2::XMLElement* anElement) override;
	void LoadPlayerStartPosition(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;

	ServerLevel* myCurrentLevel;
	std::string myMissionXMLPath;
};

