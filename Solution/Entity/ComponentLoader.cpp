#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"
#include "HealthComponentData.h"
#include "NetworkComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"
#include "XMLReader.h"
#include "GameEnum.h"

void ComponentLoader::LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			aDocument.ForceReadAttribute(e, "modelPath", aOutputData.myModelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", aOutputData.myEffectPath);
		}
		else if (elementName == CU::ToLower("Animation"))
		{
			AnimationLoadData newAnimation;
			int animState;

			aDocument.ForceReadAttribute(e, "path", newAnimation.myAnimationPath);
			aDocument.ForceReadAttribute(e, "state", animState);
			aDocument.ForceReadAttribute(e, "loop", newAnimation.myLoopFlag);
			aDocument.ForceReadAttribute(e, "resetTime", newAnimation.myResetTimeOnRestart);
			newAnimation.myEntityState = static_cast<eEntityState>(animState);

			aOutputData.myAnimations.Insert(static_cast<int>(newAnimation.myEntityState), newAnimation);
		}
	}
}

void ComponentLoader::LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myAlwaysRender = false;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			aDocument.ForceReadAttribute(e, "modelPath", aOutputData.myModelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", aOutputData.myEffectPath);
		}
		else if (elementName == CU::ToLower("AlwaysRender"))
		{
			aOutputData.myAlwaysRender = true;
		}
	}
}

void ComponentLoader::LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("MaxHealth"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMaxHealth);
		}
	}
}

void ComponentLoader::LoadNetworkComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, NetworkComponentData& aOutputData)
{
	aDocument;
	aSourceElement;
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadProjectileComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ProjectileComponentData& aOutputData)
{
	aDocument;
	aSourceElement;
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myTriggerType = -1;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Trigger"))
		{
			std::string name = "";

			aDocument.ForceReadAttribute(e, "type", name);
			aDocument.ReadAttribute(e, "value", aOutputData.myValue);
			aDocument.ReadAttribute(e, "ID", aOutputData.myID);

			aOutputData.myTriggerType = ConvertToTriggerEnum(name);
		}
	}
}

void ComponentLoader::LoadInputComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, InputComponentData& aOutputData)
{
	aDocument;
	aSourceElement;

	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadShootingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ShootingComponentData& aOutputData)
{
	aDocument;
	aSourceElement;

	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadFirstPersonRenderComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, FirstPersonRenderComponentData& aOutputData)
{
	aDocument;
	aSourceElement;

	aOutputData.myExistsInEntity = true;
}

int ComponentLoader::ConvertToTriggerEnum(std::string aName)
{
	if (aName == "healthPack")
	{
		return static_cast<int>(eTriggerType::HEALTH_PACK);
	}
	else if (aName == "changeLevel")
	{
		return static_cast<int>(eTriggerType::LEVEL_CHANGE);
	}


	DL_ASSERT("[ComponentLoader] No trigger type in trigger component named " + aName);
	return static_cast<int>(eTriggerType::_COUNT);
}