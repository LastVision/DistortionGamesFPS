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

Entity* EntityFactory::CreateEntity(eEntityType aType, Prism::Scene* aScene, bool aClientSide, const CU::Vector3f& aPosition
	, const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (myInstance->myLoadedEntityData.find(aType) != myInstance->myLoadedEntityData.end())
	{
		Entity* newEntity = new Entity(myInstance->myLoadedEntityData.find(aType)->second, aScene, aClientSide, aPosition, aRotation
			, aScale);
		return newEntity;
	}
	DL_ASSERT("Entity not found.");
	return nullptr;
}

Entity* EntityFactory::CreateEntity(eEntityType aType, std::string aSubType, Prism::Scene* aScene, bool aClientSide, const CU::Vector3f& aPosition,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale)
{
	if (aType == eEntityType::PROP || aType == eEntityType::UNIT)
	{
		if (myInstance->myLoadedSubEntityData.find(aSubType) != myInstance->myLoadedSubEntityData.end())
		{
			Entity* newEntity = new Entity(myInstance->myLoadedSubEntityData.find(aSubType)->second, aScene, aClientSide, aPosition, aRotation
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
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT)
	{
		entityDocument.ForceReadAttribute(entityElement, "subType", entitySubType);
		newData.mySubType = CU::ToLower(entitySubType);
	}

	tinyxml2::XMLElement* entityPhysics = entityDocument.ForceFindFirstChild(rootElement, "Physics");
	std::string physicsType;
	entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "type"), "value", physicsType);

	if (CU::ToLower(physicsType) == "static")
	{
		newData.myPhysData.myPhysics = ePhysics::STATIC;
	}
	else if (CU::ToLower(physicsType) == "dynamic")
	{
		newData.myPhysData.myPhysics = ePhysics::DYNAMIC;

		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "min"), "x", newData.myPhysData.myPhysicsMin.x);
		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "min"), "y", newData.myPhysData.myPhysicsMin.y);
		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "min"), "z", newData.myPhysData.myPhysicsMin.z);

		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "max"), "x", newData.myPhysData.myPhysicsMax.x);
		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "max"), "y", newData.myPhysData.myPhysicsMax.y);
		entityDocument.ForceReadAttribute(entityDocument.ForceFindFirstChild(entityPhysics, "max"), "z", newData.myPhysData.myPhysicsMax.z);
	}
	else
	{
		DL_ASSERT(CU::Concatenate("Invalid phyics-type on %s %s", entityType.c_str(), entitySubType.c_str()));
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
		else if (elementName == CU::ToLower("ProjectileComponent"))
		{
			myComponentLoader->LoadProjectileComponent(entityDocument, e, newData.myProjecileData);
		}
		else if (elementName == CU::ToLower("HealthComponent"))
		{
			myComponentLoader->LoadHealthComponent(entityDocument, e, newData.myHealthData);
		}
		else
		{
			std::string errorMessage = "The component " + elementName +
				" is not Supported. Please check if you spelled correctly.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
	if (newData.myType == eEntityType::PROP || newData.myType == eEntityType::UNIT)
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