#pragma once
#include "IReader.h"
#include <fstream>
#include <XMLReader.h>

namespace Prism
{
	class Terrain;
}

class LevelReader : public IReader
{
public:
	LevelReader();
	~LevelReader();

	void ReadFile(const std::string& aFilePath) override;

private:
	void ReadLevel(const std::string& aFilePath);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void AddUnique(const std::string& aName, int aCount);
	void SyncLevels();
	void SaveLevel();

	std::unordered_map<std::string, int> myProps;
	std::unordered_map<std::string, int> myFinalProps;
};

