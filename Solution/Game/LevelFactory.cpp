#include "stdafx.h"

#include <EntityFactory.h>
#include "Level.h"
#include "LevelFactory.h"
#include <Room.h>
#include <RoomManager.h>
#include <Scene.h>
#include <XMLReader.h>


LevelFactory::LevelFactory(const std::string& aLevelListPath)
	: myLevelPaths(4)
	, myCurrentID(0)
{
	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_Entity.xml");
	if (aLevelListPath != "")
	{
		ReadLeveList(aLevelListPath);
	}
}


LevelFactory::~LevelFactory()
{
}

Level* LevelFactory::LoadLevel(const int& aID)
{
	DL_ASSERT_EXP(aID <= myLevelPaths.Size(), "[LevelFactory] Trying to load a non-existing level! Check so the ID: " 
		+ std::to_string(aID) + " are a valid id in LI_level.xml");
	myCurrentID = aID;

	return LoadCurrentLevel();
}

Level* LevelFactory::LoadCurrentLevel()
{
	myCurrentLevel = new Level();
	ReadLevel(myLevelPaths[myCurrentID].myPath);
	return myCurrentLevel;
}

Level* LevelFactory::LoadNextLevel()
{
	if (IsLastLevel() == true)
	{
		return myCurrentLevel;
	}
	return LoadLevel(myCurrentID + 1);
}

void LevelFactory::ReadLeveList(const std::string& aLevelListPath)
{
	myLevelPaths.RemoveAll();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);
	std::string levelPath;

	int ID = -1;
	int lastID = ID - 1;
	
	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* element = reader.FindFirstChild(rootElement); element != nullptr; element = reader.FindNextElement(element))
	{
		lastID = ID;
		reader.ForceReadAttribute(element, "ID", ID);
		reader.ForceReadAttribute(element, "path", levelPath);
		myLevelPaths.Add(LevelPathInformation(ID, levelPath));

		DL_ASSERT_EXP(ID > lastID, "[LevelFactory] Wrong ID-number in LI_level.xml! The numbers should be counting up, in order.");
		DL_ASSERT_EXP(myCurrentID < 10, "[LevelFactory] Can't handle level ID with two digits.");
	}
	reader.CloseDocument();
}

void LevelFactory::ReadLevel(const std::string& aLevelPath)
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

void LevelFactory::LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.ForceFindFirstChild(aElement, "room"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "room"))
	{
		CU::Vector3f position;
		CU::Vector3f rotation;
		CU::Vector3f scale;

		ReadOrientation(aReader, entityElement, position, rotation, scale);

		DL_ASSERT_EXP(rotation.x == 0 && rotation.y == 0 && rotation.z == 0, "Room rotation non-zero.");

		myCurrentLevel->GetScene()->AddRoom(new Prism::Room(position, scale));
	}

	myCurrentLevel->GetScene()->GetRoomManager()->CalcPortals();
}

void LevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
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
		
		Entity* newEntity = EntityFactory::CreateEntity(eEntityType::PROP, propType, *myCurrentLevel->GetScene()
			, propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}

void LevelFactory::LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement)
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

		Entity* newEntity = EntityFactory::CreateEntity(eEntityType::UNIT, unitType, *myCurrentLevel->GetScene()
			, unitPosition, unitRotation, unitScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}

void LevelFactory::ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement,
	CU::Vector3f& aOutPosition, CU::Vector3f& aOutRotation, CU::Vector3f& aOutScale)
{
	tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(aElement, "position");
	aReader.ForceReadAttribute(propElement, "X", "Y", "Z", aOutPosition);

	propElement = aReader.ForceFindFirstChild(aElement, "rotation");
	aReader.ForceReadAttribute(propElement, "X", "Y", "Z", aOutRotation);

	propElement = aReader.ForceFindFirstChild(aElement, "scale");
	aReader.ForceReadAttribute(propElement, "X", "Y", "Z", aOutScale);
}