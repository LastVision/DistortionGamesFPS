#pragma once
#include <SharedLevelFactory.h>
class ServerLevelFactory : public SharedLevelFactory
{
public:
	ServerLevelFactory(const std::string& aLevelListPath);
	~ServerLevelFactory();
	SharedLevel* LoadCurrentLevel() override;

private:
	void ReadLevel(const std::string& aLevelPath) override;

	void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
};

