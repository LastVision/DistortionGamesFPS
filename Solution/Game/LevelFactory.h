#pragma once

class Level;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class LevelFactory
{
public:
	LevelFactory();
	~LevelFactory();

	Level* LoadCurrentLevel();

private:
	void ReadLevel(const std::string& aLevelPath);

	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	Level* myCurrentLevel;
};

