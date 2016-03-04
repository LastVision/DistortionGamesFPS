#include "stdafx.h"

#include <EntityFactory.h>
#include "ClientLevel.h"
#include "ClientLevelFactory.h"
#include <Room.h>
#include <RoomManager.h>
#include <Scene.h>
#include <XMLReader.h>


ClientLevelFactory::ClientLevelFactory(const std::string& aLevelListPath)
	: SharedLevelFactory(aLevelListPath)
{

}


ClientLevelFactory::~ClientLevelFactory()
{
}



SharedLevel* ClientLevelFactory::LoadCurrentLevel()
{
	myCurrentLevel = new ClientLevel();
	ReadLevel(myLevelPaths[myCurrentID].myPath);
	return myCurrentLevel;
}

void ClientLevelFactory::ReadLevel(const std::string& aLevelPath)
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

void ClientLevelFactory::LoadRooms(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	int i = 0;
	for (tinyxml2::XMLElement* entityElement = aReader.ForceFindFirstChild(aElement, "room"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "room"))
	{
		CU::Vector3f position;
		CU::Vector3f rotation;
		CU::Vector3f scale;
		ReadOrientation(aReader, entityElement, position, rotation, scale);
		DL_ASSERT_EXP(rotation.x == 0 && rotation.y == 0 && rotation.z == 0, "Room rotation non-zero.");

		std::string name;
		aReader.ForceReadAttribute(entityElement, "name", name);
		std::string type;
		aReader.ForceReadAttribute(entityElement, "type", type);



		DL_ASSERT_EXP(type == "room" || type == "connector", CU::Concatenate("Room type can't be: %s", type.c_str()));

		Prism::eRoomType typeEnum(Prism::eRoomType::ROOM);

		if (type == "connector")
		{
			typeEnum = Prism::eRoomType::CONNECTOR;
		}

		static_cast<ClientLevel*>(myCurrentLevel)->GetScene()->AddRoom(new Prism::Room(position, scale, i, name, typeEnum));
		++i;
	}

	static_cast<ClientLevel*>(myCurrentLevel)->GetScene()->GetRoomManager()->CalcPortals();
}

void ClientLevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
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
		
		Entity* newEntity = EntityFactory::CreateEntity(eEntityType::PROP, propType, static_cast<ClientLevel*>(myCurrentLevel)->GetScene()
			, true, propPosition, propRotation, propScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}

void ClientLevelFactory::LoadUnits(XMLReader& aReader, tinyxml2::XMLElement* aElement)
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

		Entity* newEntity = EntityFactory::CreateEntity(eEntityType::UNIT, unitType, static_cast<ClientLevel*>(myCurrentLevel)->GetScene()
			, true, unitPosition, unitRotation, unitScale);
		newEntity->AddToScene();
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}