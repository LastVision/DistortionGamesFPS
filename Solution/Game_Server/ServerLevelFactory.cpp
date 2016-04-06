#include "stdafx.h"
#include <CommonHelper.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <MathHelper.h>
#include <NetworkComponent.h>
#include <NetMessagePressEText.h>
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>
#include <PollingStation.h>
#include "ServerLevel.h"
#include "ServerLevelFactory.h"
#include "ServerUnitManager.h"
#include "ServerNetworkManager.h"
#include <TriggerComponent.h>
#include <XMLReader.h>
#include <SpawnpointComponent.h>

ServerLevelFactory::ServerLevelFactory(const std::string& aLevelListPath)
	: SharedLevelFactory(aLevelListPath)
	, myMissionXMLPath("")
{
}

ServerLevelFactory::~ServerLevelFactory()
{
}

ServerLevel* ServerLevelFactory::LoadLevel(int aID)
{
	myIsLoadingLevel = true;
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
	myIsLoadingLevel = true;
	myCurrentLevel = new ServerLevel();
	ReadLevel(myLevelPaths[myCurrentID]);
	myCurrentLevel->Init(myMissionXMLPath);
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->InitThread();
#endif
	myIsLoadingLevel = false;
	return myCurrentLevel;
}


void ServerLevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "missionXML"), "path", myMissionXMLPath);

	LoadRooms(reader, levelElement);
	LoadProps(reader, levelElement);
	LoadDoors(reader, levelElement);
	//LoadUnits(reader, levelElement);
	LoadSpawnpoint(reader, levelElement);
	LoadTriggers(reader, levelElement);
	LoadPlayerStartPosition(reader, levelElement);

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



		PollingStation::GetInstance()->AddEntity(newEntity);
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

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::TRIGGER, triggerType, nullptr, false, triggerPosition, triggerRotation, triggerScale);

		if (newEntity->GetComponent<TriggerComponent>()->IsClientSide() == false)
		{
			newEntity->Reset();
			myCurrentLevel->AddEntity(newEntity);
			//if (newEntity->GetComponent<NetworkComponent>() != nullptr)
			//{
			//	myIDCount++;
			//	newEntity->GetComponent<NetworkComponent>()->SetNetworkID(myIDCount);
			//}
			if (newEntity->GetComponent<TriggerComponent>()->GetIsActiveFromStart() == false)
			{
				newEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			}
			else if (newEntity->GetComponent<TriggerComponent>()->IsPressable() == true)
			{
				myCurrentLevel->AddPressETrigger(newEntity);
			}
		}
		else
		{
			SAFE_DELETE(newEntity);
		}
	}
}

void ServerLevelFactory::LoadSpawnpoint(XMLReader& aReader, tinyxml2::XMLElement* anElement)
{
	ServerUnitManager::GetInstance()->CreateUnits(nullptr);

	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(anElement, "spawnpoint"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "spawnpoint"))
	{
		std::string spawnpointType;
		aReader.ForceReadAttribute(entityElement, "type", spawnpointType);
		spawnpointType = CU::ToLower(spawnpointType);

		CU::Vector3f serverPosition;

		unsigned int gid(UINT32_MAX);

		ReadGID(aReader, entityElement, gid);


		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		aReader.ForceReadAttribute(propElement, "x", "y", "z", serverPosition);

		Entity* newEntity = EntityFactory::CreateEntity(gid, eEntityType::SPAWNPOINT, spawnpointType, nullptr, false
			, serverPosition);
		newEntity->Reset();



		for (tinyxml2::XMLElement* e = aReader.FindFirstChild(entityElement, "boundtotrigger"); e != nullptr;
			e = aReader.FindNextElement(e, "boundtotrigger"))
		{
			int triggerGID;
			aReader.ForceReadAttribute(e, "value", triggerGID);
			if (triggerGID != -1)
			{
				newEntity->GetComponent<SpawnpointComponent>()->BindToTrigger(triggerGID);
			}
		}

		//Read bound to trigger

		myCurrentLevel->AddEntity(newEntity);
	}
}

void ServerLevelFactory::LoadPlayerStartPosition(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.ForceFindFirstChild(aElement, "startPosition"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "startPosition"))
	{
		CU::Vector3<float> position;
		unsigned int gid = 0;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "gid"), "value", gid);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "position"), "X", "Y", "Z", position);

		myCurrentLevel->AddPlayerStartPosition(gid, position);
	}
}