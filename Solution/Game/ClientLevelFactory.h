#pragma once
#include <GrowingArray.h>
#include <SharedLevelFactory.h>

class SharedLevel;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ClientLevelFactory : public SharedLevelFactory
{
public:
	ClientLevelFactory(const std::string& aLevelListPath);
	~ClientLevelFactory();

	SharedLevel* LoadCurrentLevel() override;

private:
	void ReadLevel(const std::string& aLevelPath) override;

	void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aElement);
};