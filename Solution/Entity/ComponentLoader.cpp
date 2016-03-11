#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"
#include "HealthComponentData.h"
#include "NetworkComponentData.h"
#include "PhysicsComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"
#include "UpgradeComponentData.h"
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

void ComponentLoader::LoadPhysicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PhysicsComponentData& aOutputData)
{
	std::string physicsType;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "type"), "value", physicsType);

	aOutputData.myExistsInEntity = true;

	if (CU::ToLower(physicsType) == "static")
	{
		aOutputData.myPhysicsType = ePhysics::STATIC;
	}
	else if (CU::ToLower(physicsType) == "dynamic")
	{
		aOutputData.myPhysicsType = ePhysics::DYNAMIC;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else if (CU::ToLower(physicsType) == "phantom" || CU::ToLower(physicsType) == "trigger")
	{
		aOutputData.myPhysicsType = ePhysics::PHANTOM;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else if (CU::ToLower(physicsType) == "capsule")
	{
		aOutputData.myPhysicsType = ePhysics::CAPSULE;
	}
	else if (CU::ToLower(physicsType) == "kinematic")
	{
		aOutputData.myPhysicsType = ePhysics::KINEMATIC;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else
	{
		//DL_ASSERT(CU::Concatenate("Invalid phyics-type on %s %s", entityType.c_str(), entitySubType.c_str()));
		DL_ASSERT("Failed to load PhysicsComponent");
	}
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
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Speed"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySpeed);
		}
		else if (elementName == CU::ToLower("rotationSpeed"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myRotationSpeed);
		}
		else if (elementName == CU::ToLower("sprintMultiplier"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySprintMultiplier);
		}
	}

	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadShootingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ShootingComponentData& aOutputData)
{
	aDocument;
	aSourceElement;

	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadUpgradeComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, UpgradeComponentData& aOutputData)
{
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("ClipSizeModifier"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myClipSizeModifier);
		}
		else if (elementName == CU::ToLower("AmmoTotalModifier"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myAmmoTotalModifier);
		}
		else if (elementName == CU::ToLower("DamageModifier"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myDamageModifier);
		}
		else if (elementName == CU::ToLower("ShootTimeModifier"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myShootTimeModifier);
		}
	}

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
	else if (aName == "upgrade")
	{
		return static_cast<int>(eTriggerType::UPGRADE);
	}

	DL_ASSERT("[ComponentLoader] No trigger type in trigger component named " + aName);
	return static_cast<int>(eTriggerType::_COUNT);
}