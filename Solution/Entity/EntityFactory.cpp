#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityFactory.h"
#include "EntityEnumConverter.h"
#include <string>


EntityFactory* EntityFactory::myInstance = nullptr;

EntityFactory* EntityFactory::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityFactory();
	}
	return myInstance;
}

void EntityFactory::Destroy()
{
	SAFE_DELETE(myInstance);
}

void EntityFactory::LoadEntities(const char* aEntityListXML)
{
	XMLReader entityListDocument;
	entityListDocument.OpenDocument(aEntityListXML);

	tinyxml2::XMLElement* rootElement = entityListDocument.ForceFindFirstChild("root");

	for (tinyxml2::XMLElement* e = entityListDocument.FindFirstChild(rootElement); e != nullptr;
		e = entityListDocument.FindNextElement(e))
	{
		std::string entityPath;
		entityListDocument.ForceReadAttribute(e, "src", entityPath);
		LoadEntity(entityPath.c_str());
	}

	entityListDocument.CloseDocument();
}

Entity* EntityFactory::CreateEntity(unsigned int aGID, eEntityType aType, Prism::Scene* aScene, bool aClientSide, const CU::Vector3f& aPosition
	, const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (myInstance->myLoadedEntityData.find(aType) != myInstance->myLoadedEntityData.end())
	{
		Entity* newEntity = new Entity(aGID, myInstance->myLoadedEntityData.find(aType)->second, aScene, aClientSide, aPosition, aRotation
			, aScale);
		return newEntity;
	}
	DL_ASSERT("Entity not found.");
	return nullptr;
}

Entity* EntityFactory::CreateEntity(unsigned int aGID, eEntityType aType, std::string aSubType, Prism::Scene* aScene, bool aClientSide, const CU::Vector3f& aPosition,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (aType == eEntityType::PROP || aType == eEntityType::UNIT || aType == eEntityType::TRIGGER || aType == eEntityType::PLAYER)
	{
		if (myInstance->myLoadedSubEntityData.find(aSubType) != myInstance->myLoadedSubEntityData.end())
		{
			Entity* newEntity = new Entity(aGID, myInstance->myLoadedSubEntityData.find(aSubType)->second, aScene, aClientSide, aPosition, aRotation
				, aScale);
			newEntity->mySubType = aSubType;
			return newEntity;
		}
	}
	std::string errorMessage = "SubType " + aSubType + " not found.";
	DL_ASSERT(errorMessage);
	return nullptr;
}

void EntityFactory::LoadEntity(const char* aEntityPath)
{
	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	tinyxml2::XMLElement* rootElement = entityDocument.ForceFindFirstChild("root");
	tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");

	EntityData newData;
	std::string entityType;
	std::string entitySubType;
	entityDocument.ForceReadAttribute(entityElement, "type", entityType);
	newData.myType = EntityEnumConverter::ConvertStringToEntityType(CU::ToLower(entityType));
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT || newData.myType == eEntityType::TRIGGER || newData.myType == eEntityType::PLAYER)
	{
		entityDocument.ForceReadAttribute(entityElement, "subType", entitySubType);
		newData.mySubType = CU::ToLower(entitySubType);
	}


	for (tinyxml2::XMLElement* e = entityDocument.FindFirstChild(entityElement); e != nullptr;
		e = entityDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("AnimationComponent"))
		{
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You have a GraphicsComponent so you can't have a AnimationComponent");
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You already have a AnimationComponent");

			myComponentLoader->LoadAnimationComponent(entityDocument, e, newData.myAnimationData);
		}
		else if (elementName == CU::ToLower("GraphicsComponent"))
		{
			if (newData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You have a AnimationComponent so you can't have a GraphicsComponent");
			if (newData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You already have a GraphicsComponent");

			myComponentLoader->LoadGraphicsComponent(entityDocument, e, newData.myGraphicsData);
		}
		else if (elementName == CU::ToLower("GrenadeComponent"))
		{
			myComponentLoader->LoadGrenadeComponent(entityDocument, e, newData.myProjecileData);
		}
		else if (elementName == CU::ToLower("NetworkComponent"))
		{
			myComponentLoader->LoadNetworkComponent(entityDocument, e, newData.myNetworkData);
		}
		else if (elementName == CU::ToLower("HealthComponent"))
		{
			myComponentLoader->LoadHealthComponent(entityDocument, e, newData.myHealthData);
		}
		else if (elementName == CU::ToLower("TriggerComponent"))
		{
			myComponentLoader->LoadTriggerComponent(entityDocument, e, newData.myTriggerData);
		}
		else if (elementName == CU::ToLower("InputComponent"))
		{
			myComponentLoader->LoadInputComponent(entityDocument, e, newData.myInputData);
		}
		else if (elementName == CU::ToLower("ShootingComponent"))
		{
			myComponentLoader->LoadShootingComponent(entityDocument, e, newData.myShootingData);
		}
		else if (elementName == CU::ToLower("UpgradeComponent"))
		{
			myComponentLoader->LoadUpgradeComponent(entityDocument, e, newData.myUpgradeData);
		}
		else if (elementName == CU::ToLower("FirstPersonRenderComponent"))
		{
			myComponentLoader->LoadFirstPersonRenderComponent(entityDocument, e, newData.myFirstPersonRenderData);
		}
		else if (elementName == CU::ToLower("PhysicsComponent"))
		{
			myComponentLoader->LoadPhysicsComponent(entityDocument, e, newData.myPhysicsData);
		}
		else
		{
			std::string errorMessage = "The component " + elementName +
				" is not Supported. Please check if you spelled correctly.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT || newData.myType == eEntityType::TRIGGER || newData.myType == eEntityType::PLAYER)
	{
		myLoadedSubEntityData[newData.mySubType] = newData;
	}
	else
	{
		myLoadedEntityData[newData.myType] = newData;
	}

	entityDocument.CloseDocument();
}

EntityFactory::EntityFactory()
{
	myComponentLoader = new ComponentLoader();
}


EntityFactory::~EntityFactory()
{
	SAFE_DELETE(myComponentLoader);
}