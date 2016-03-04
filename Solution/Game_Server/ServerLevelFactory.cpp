#include "stdafx.h"
#include <CommonHelper.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <MathHelper.h>
#include "ServerLevel.h"
#include "ServerLevelFactory.h"
#include <XMLReader.h>

ServerLevelFactory::ServerLevelFactory(const std::string& aLevelListPath)
	: SharedLevelFactory(aLevelListPath)
{
}

ServerLevelFactory::~ServerLevelFactory()
{
}

SharedLevel* ServerLevelFactory::LoadCurrentLevel()
{
	myCurrentLevel = new ServerLevel();
	ReadLevel(myLevelPaths[myCurrentID].myPath);
	return myCurrentLevel;
}


void ServerLevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	LoadRooms(reader, levelElement);
	LoadProps(reader, levelElement);
	LoadUnits(reader, levelElement);

	reader.CloseDocument();
}

void ServerLevelFactory::LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	aReader;
	aElement;
}

void ServerLevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "prop"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "propType", propType);
		propType = CU::ToLower(propType);

		CU::Vector3f propPosition;
		CU::Vector3f propRotation;
		CU::Vector3f propScale;

		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		/*Entity* newEntity = EntityFactory::CreateEntity(eEntityType::PROP, propType, *myCurrentLevel->GetScene()
			, propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);*/
	}
}

void ServerLevelFactory::LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "unit"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "unit"))
	{
		std::string unitType;
		aReader.ForceReadAttribute(entityElement, "unitType", unitType);
		unitType = CU::ToLower(unitType);

		CU::Vector3f unitPosition;
		CU::Vector3f unitRotation;
		CU::Vector3f unitScale;

		ReadOrientation(aReader, entityElement, unitPosition, unitRotation, unitScale);

		unitRotation.x = CU::Math::DegreeToRad(unitRotation.x);
		unitRotation.y = CU::Math::DegreeToRad(unitRotation.y);
		unitRotation.z = CU::Math::DegreeToRad(unitRotation.z);

		/*Entity* newEntity = EntityFactory::CreateEntity(eEntityType::UNIT, unitType, *myCurrentLevel->GetScene()
			, unitPosition, unitRotation, unitScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);*/
	}
}