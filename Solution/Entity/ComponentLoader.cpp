#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"
#include "HealthComponentData.h"
#include "NetworkComponentData.h"
#include "PhysicsComponentData.h"
#include "BulletComponentData.h"
#include "ShootingComponentData.h"
#include "TriggerComponentData.h"
#include "UpgradeComponentData.h"
#include "XMLReader.h"
#include "GameEnum.h"

void ComponentLoader::LoadAIComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AIComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "VisionRange"), "value", aOutputData.myVisionRange);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "ArriveStopRange"), "value", aOutputData.myArriveStopRange);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Speed"), "value", aOutputData.mySpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "AttackAnimationTime"), "value", aOutputData.myAttackAnimationTime);
}

void ComponentLoader::LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aOutputData.myRoomType = LoadRoomType(aDocument, aSourceElement);

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

	aOutputData.myRoomType = LoadRoomType(aDocument, aSourceElement);

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
			DL_ASSERT("Legacy XML");
		}
#ifdef RELEASE_BUILD
		else if (elementName == CU::ToLower("Debug"))
		{
			aOutputData.myExistsInEntity = false;
		}
#endif
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

void ComponentLoader::LoadGrenadeComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GrenadeComponentData& aOutputData)
{
	aDocument;
	aSourceElement;
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myIsOneTime = false; 
	aOutputData.myIsPressable = false;
	aOutputData.myTriggerType = -1;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Trigger"))
		{
			std::string name = "";

			aDocument.ForceReadAttribute(e, "type", name);
			aDocument.ReadAttribute(e, "value", aOutputData.myValue);
			aDocument.ReadAttribute(e, "oneTimeTrigger", aOutputData.myIsOneTime);
			aDocument.ForceReadAttribute(e, "isClientSide", aOutputData.myIsClientSide);
			aDocument.ReadAttribute(e, "activeFromStart", aOutputData.myIsActiveFromStart);
			aDocument.ReadAttribute(e, "isPressable", aOutputData.myIsPressable);

			aOutputData.myTriggerType = ConvertToTriggerEnum(name);

		}
		else if (elementName == CU::ToLower("Marker"))
		{
				aDocument.ReadAttribute(e, "positionx", aOutputData.myPosition.x);
				aDocument.ReadAttribute(e, "positiony", aOutputData.myPosition.y);
				aDocument.ReadAttribute(e, "positionz", aOutputData.myPosition.z);
				aDocument.ForceReadAttribute(e, "show", aOutputData.myShowMarker);
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
		else if (elementName == CU::ToLower("maxSprintEnergy"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMaxSprintEnergy);
		}
		else if (elementName == CU::ToLower("sprintIncreasePerSecond"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySprintIncrease);
		}
		else if (elementName == CU::ToLower("sprintDecreasePerSecond"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySprintDecrease);
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

void ComponentLoader::LoadSpawnpointComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SpawnpointComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myUnitTypes.Init(8);
	aDocument;
	aSourceElement;
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("SpawnUnitType"))
		{
			std::string toAdd;
			aDocument.ForceReadAttribute(e, "type", toAdd);
			aOutputData.myUnitTypes.Add(toAdd);
		}
		else if (elementName == CU::ToLower("SpawnpointLifetime"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySpawnpointLifetime);
		}
		else if (elementName == CU::ToLower("SpawnInterval"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySpawnInterval);
		}
		else if (elementName == CU::ToLower("SpawnPerInterval"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.mySpawnPerInterval);
		}
		else if (elementName == CU::ToLower("SpawnCount"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myUnitCount);
		}
	}
}

void ComponentLoader::LoadUpgradeComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, UpgradeComponentData& aOutputData)
{
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Weapon"))
		{
			std::string type = "";
			aDocument.ForceReadAttribute(e, "type", type);
			aOutputData.myWeaponType = EntityEnumConverter::ConvertStringToWeaponType(type);
		}
		else if (elementName == CU::ToLower("ClipSizeModifier"))
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
		else if (elementName == CU::ToLower("MinSpreadRotation"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMinSpreadRotation);
		}
		else if (elementName == CU::ToLower("MaxSpreadRotation"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMaxSpreadRotation);
		}
	}

	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::LoadBulletComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BulletComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Damage"), "value", aOutputData.myDamage);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Speed"), "value", aOutputData.mySpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Lifetime"), "value", aOutputData.myLifetime);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "minspreadrotation"), "value", aOutputData.myMinRotation);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "maxspreadrotation"), "value", aOutputData.myMaxRotation);
}

void ComponentLoader::LoadFirstPersonRenderComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, FirstPersonRenderComponentData& aOutputData)
{
	aDocument;
	aSourceElement;

	aOutputData.myExistsInEntity = true;
}

eObjectRoomType ComponentLoader::LoadRoomType(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement)
{
	std::string typeStr;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "RoomType"), "value", typeStr);

	if (typeStr == "dynamic")
	{
		return eObjectRoomType::DYNAMIC;
	}
	else if (typeStr == "static")
	{
		return eObjectRoomType::STATIC;
	}
	else if (typeStr == "alwaysRender")
	{
		return eObjectRoomType::ALWAYS_RENDER;
	}
	else
	{
		DL_ASSERT(CU::Concatenate("Unknown room type: %s", typeStr.c_str()));
		return eObjectRoomType::STATIC;
	}
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
	else if (aName == "unlock")
	{
		return static_cast<int>(eTriggerType::UNLOCK);
	}
	else if (aName == "lock")
	{
		return static_cast<int>(eTriggerType::LOCK);
	}
	else if (aName == "startMission")
	{
		return static_cast<int>(eTriggerType::MISSION);
	}
	else if (aName == "respawn")
	{
		return static_cast<int>(eTriggerType::RESPAWN);
	}
	else if (aName == "spawntrigger")
	{
		return static_cast<int>(eTriggerType::ENEMY_SPAWN);
	}
	else if (aName == "marker")
	{
		return static_cast<int>(eTriggerType::MARKER);
	}

	DL_ASSERT("[ComponentLoader] No trigger type in trigger component named " + aName);
	return static_cast<int>(eTriggerType::_COUNT);
}

