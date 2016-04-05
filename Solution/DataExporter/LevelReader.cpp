#include <CommonHelper.h>
#include <Defines.h>
#include <MathHelper.h>
#include "LevelReader.h"
#include <XMLReader.h>
#include <Vertices.h>
#include <iostream>
#include <MemoryMacros.h>

LevelReader::LevelReader()
{
}


LevelReader::~LevelReader()
{
}

void LevelReader::ReadFile(const std::string& aFilePath)
{
	XMLReader reader;
	reader.OpenDocument(aFilePath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");

	levelElement = reader.ForceFindFirstChild(levelElement, "level");

	while (levelElement != nullptr)
	{
		std::string path;
		reader.ForceReadAttribute(levelElement, "path", path);

		ReadLevel(path);

		levelElement = reader.FindNextElement(levelElement, "level");
	}

	reader.CloseDocument();


	SaveLevel();
}

void LevelReader::ReadLevel(const std::string& aFilePath)
{
	XMLReader reader;
	reader.OpenDocument(aFilePath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	LoadProps(reader, levelElement);

	SyncLevels();

	reader.CloseDocument();
}

void LevelReader::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	tinyxml2::XMLElement* propElement = aReader.FindFirstChild(aLevelElement, "prop");
	for (; propElement != nullptr; propElement = aReader.FindNextElement(propElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(propElement, "propType", propType);

		if (myProps.find(propType) == myProps.end())
		{
			myProps[propType] = 0;
		}
		++myProps[propType];
	}

	tinyxml2::XMLElement* artifactElement = aReader.FindFirstChild(aLevelElement, "artifact");
	for (; artifactElement != nullptr; artifactElement = aReader.FindNextElement(artifactElement, "artifact"))
	{
		std::string type;
		aReader.ForceReadAttribute(artifactElement, "type", type);

		if (myProps.find(type) == myProps.end())
		{
			myProps[type] = 0;
		}
		++myProps[type];
	}

	AddUnique("SM_flag_a", 20);
	AddUnique("SM_Victory_Point_Indicator", 20);
	AddUnique("SM_Resource_Point_Indicator", 20);
	AddUnique("totem", 2);
	AddUnique("SM_totem", 2);

	AddUnique("SM_muzzleflash", 64 * 3);
	AddUnique("SM_muzzleflash2", 64 * 3);
	AddUnique("SM_muzzleflash3", 64 * 3);
	AddUnique("SM_muzzleflash4", 64 * 3);
	AddUnique("SM_muzzleflash5", 64 * 3);

	AddUnique("SM_selectionRing", 64 * 3);
	AddUnique("SM_selectionRing_Hovered", 64 * 3);
	AddUnique("SM_selectionRingBase", 2);
	AddUnique("SM_selectionRingBase_Hovered", 2);
	AddUnique("SM_shadowBlob", 64 * 3);

	AddUnique("SM_capture_point_pole", 32);
	AddUnique("SM_minefield_flag_a", 128);
	AddUnique("SM_rallypoint", 2);
}

void LevelReader::AddUnique(const std::string& aName, int aCount)
{
	if (myProps.find(aName) == myProps.end())
	{
		myProps[aName] = aCount;
	}
	else
	{
		myProps[aName] += aCount;
	}
}

void LevelReader::SyncLevels()
{
	for (auto it = myProps.begin(); it != myProps.end(); ++it)
	{
		if (myFinalProps.find(it->first) == myFinalProps.end())
		{
			myFinalProps[it->first] = 0;
		}

		int current = it->second;
		int finalCount = myFinalProps[it->first];

		if (current > finalCount)
		{
			myFinalProps[it->first] = current;
		}
	}

	myProps.clear();
}

void LevelReader::SaveLevel()
{
	std::string outputPath = "GeneratedData/modelcount.bin";
	CU::BuildFoldersInPath(outputPath);


	std::fstream file;
	file.open(outputPath, std::ios::out);
	
	for (auto it = myFinalProps.begin(); it != myFinalProps.end(); ++it)
	{
		file << it->first << " " << it->second << std::endl;
	}

	file.close();
}