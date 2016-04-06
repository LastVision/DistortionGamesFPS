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

	ClientLevel* LoadLevel(int aID, GUI::Cursor* aCursor);
	ClientLevel* LoadCurrentLevel(GUI::Cursor* aCursor);

private:
	void ReadLevel(const std::string& aLevelPath) override;

	void LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadDoors(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;
	void LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadParticles(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadText(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadPlayerStartPosition(XMLReader& aReader, tinyxml2::XMLElement* aElement) override;

	ClientLevel* myCurrentLevel;

	std::string myWeaponSettingsPath;
};
