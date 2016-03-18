#include "stdafx.h"
#include <CommonHelper.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <MathHelper.h>
#include "ServerLevel.h"
#include "ServerLevelFactory.h"
#include <PhysicsInterface.h>
#include <XMLReader.h>
#include <TriggerComponent.h>
#include <NetworkComponent.h>

ServerLevelFactory::ServerLevelFactory(const std::string& aLevelListPath)
	: SharedLevelFactory(aLevelListPath)
{
}

ServerLevelFactory::~ServerLevelFactory()
{
}

ServerLevel* ServerLevelFactory::LoadLevel(int aID)
{
	if (myLevelPaths.find(aID) == myLevelPaths.end())
	{
		DL_ASSERT(CU::Concatenate("[LevelFactory] Trying to load a non-existing level! Check so the ID: %s is a valid id in LI_level.xml"
			, std::to_string(aID).c_str()));
	}

	myCurrentID = aID;

	return LoadCurrentLevel();
}

ServerLevel* ServerLevelFactory::LoadCurrentLevel()
{
	myCurrentLevel = new ServerLevel();
	ReadLevel(myLevelPaths[myCurrentID]);
	myCurrentLevel->Init();
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->InitThread();
#endif
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
	LoadDoors(reader, levelElement);
	LoadUnits(reader, levelElement);
	LoadTriggers(reader, levelElement);

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
		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);
		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::PROP, propType, nullptr, false
			, propPosition, propRotation, propScale);
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
	}
}

void ServerLevelFactory::LoadDoors(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "door"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "door"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "doorType", propType);
		propType = CU::ToLower(propType);

		CU::Vector3f propPosition;
		CU::Vector3f propRotation;
		CU::Vector3f propScale;
		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);
		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::PROP, propType, nullptr, false
			, propPosition, propRotation, propScale);
		newEntity->Reset();

		myCurrentLevel->AddEntity(newEntity);
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
		unitType += "server";

		CU::Vector3f unitPosition;
		CU::Vector3f unitRotation;
		CU::Vector3f unitScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);

		ReadOrientation(aReader, entityElement, unitPosition, unitRotation, unitScale);

		unitRotation.x = CU::Math::DegreeToRad(unitRotation.x);
		unitRotation.y = CU::Math::DegreeToRad(unitRotation.y);
		unitRotation.z = CU::Math::DegreeToRad(unitRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::UNIT, unitType, nullptr, false
			, unitPosition, unitRotation, unitScale);
		newEntity->Reset();

		myCurrentLevel->AddEnemy(newEntity);
	}
}

void ServerLevelFactory::LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "trigger"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "trigger"))
	{
		std::string triggerType;
		aReader.ForceReadAttribute(entityElement, "triggerType", triggerType);
		triggerType = CU::ToLower(triggerType);

		CU::Vector3f triggerPosition;
		CU::Vector3f triggerRotation;
		CU::Vector3f triggerScale;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);

		ReadOrientation(aReader, entityElement, triggerPosition, triggerRotation, triggerScale);

		triggerRotation.x = CU::Math::DegreeToRad(triggerRotation.x);
		triggerRotation.y = CU::Math::DegreeToRad(triggerRotation.y);
		triggerRotation.z = CU::Math::DegreeToRad(triggerRotation.z);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::UNIT, triggerType, nullptr, false, triggerPosition, triggerRotation, triggerScale);

		if (newEntity->GetComponent<TriggerComponent>()->IsClientSide() == false)
		{
			newEntity->Reset();
			myCurrentLevel->AddEntity(newEntity);
			//if (newEntity->GetComponent<NetworkComponent>() != nullptr)
			//{
			//	myIDCount++;
			//	newEntity->GetComponent<NetworkComponent>()->SetNetworkID(myIDCount);
			//}
		}
		else
		{
			SAFE_DELETE(newEntity);
		}
	}
}