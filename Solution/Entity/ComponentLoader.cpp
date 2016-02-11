#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"

void ComponentLoader::LoadActorComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ActorComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Speed"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMoveSpeed);
		}
		else if (elementName == CU::ToLower("VisionRange"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myVisionRange);
		}
		else if (elementName == CU::ToLower("AttackRange"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackRange);
		}
		else if (elementName == CU::ToLower("AttackDamage"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackDamage);
		}
		else if (elementName == CU::ToLower("AttackSpeed"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackRechargeTime);
		}
		else if (elementName == CU::ToLower("ChaseDistance"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myChaseDistance);
		}
		else if (elementName == CU::ToLower("ChaseDistanceNeutral"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myChaseDistanceNeutral);
		}
		else if (elementName == CU::ToLower("RangerOneShotCooldown"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myRangerOneShotCoolDown);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

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
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadBuidlingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BuildingComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;


	aOutputData.myBuildUnitTypes[0] = eUnitType::NOT_A_UNIT;
	aOutputData.myBuildUnitTypes[1] = eUnitType::NOT_A_UNIT;
	aOutputData.myBuildUnitTypes[2] = eUnitType::NOT_A_UNIT;

	int numberOfUnitType = 0;
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("MaxQueue"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMaxQueue);
		}
		else if (elementName == CU::ToLower("Unit"))
		{
			DL_ASSERT_EXP(numberOfUnitType <= 3, "You can't assign more than three unit types in a building.");
			std::string unitType;
			aDocument.ForceReadAttribute(e, "type", unitType);

			int unitCost = 0;
			aDocument.ForceReadAttribute(e, "cost", unitCost);
			aOutputData.myUnitCosts.Insert(numberOfUnitType, unitCost);

			float unitBuildTime = 0.f;
			aDocument.ForceReadAttribute(e, "buildTime", unitBuildTime);
			aOutputData.myUnitBuildTimes.Insert(numberOfUnitType, unitBuildTime);

			int supplyCost = 0;
			aDocument.ForceReadAttribute(e, "supplyCost", supplyCost);
			aOutputData.myUnitSupplyCosts.Insert(numberOfUnitType, supplyCost);

			eUnitType buildUnitType = EntityEnumConverter::ConvertStringToUnitType(CU::ToLower(unitType));
			DL_ASSERT_EXP(buildUnitType != eUnitType::NOT_A_UNIT, "The " + unitType + " type is not supported.");
			aOutputData.myBuildUnitTypes.Insert(numberOfUnitType, buildUnitType);
			numberOfUnitType++;
		}
		else if (elementName == CU::ToLower("Upgrades"))
		{
			for (tinyxml2::XMLElement* unitElem = aDocument.ForceFindFirstChild(e); unitElem != nullptr;
				unitElem = aDocument.FindNextElement(unitElem))
			{
				std::string unit = CU::ToLower(unitElem->Name());
				eUnitType unitType = EntityEnumConverter::ConvertStringToUnitType(unit);
				int unitIndex = static_cast<int>(unitType);
				int upgradeIndex = 0;

				for (tinyxml2::XMLElement* upgradeElem = aDocument.ForceFindFirstChild(unitElem); upgradeElem != nullptr;
					upgradeElem = aDocument.FindNextElement(upgradeElem))
				{
					Upgrade upgrade;
					upgrade.myInProgress = false;
					aDocument.ForceReadAttribute(upgradeElem, "buildTime", upgrade.myBuildTime);
					aDocument.ForceReadAttribute(upgradeElem, "cooldown", upgrade.myCooldown);
					aDocument.ForceReadAttribute(upgradeElem, "cost", upgrade.myCost);
					aDocument.ForceReadAttribute(upgradeElem, "attack", upgrade.myAttackModifier);
					aDocument.ForceReadAttribute(upgradeElem, "armor", upgrade.myArmorModifier);
					aDocument.ForceReadAttribute(upgradeElem, "attackspeed", upgrade.myAttackSpeedModifier);
					aDocument.ForceReadAttribute(upgradeElem, "movespeed", upgrade.myMoveSpeedModifier);

					aOutputData.myUnitUpgrades[unitIndex][upgradeIndex] = upgrade;

					++upgradeIndex;
				}
			}
		}
		else if (elementName == CU::ToLower("MineFieldRadius"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMineFieldRadius);
		}
		else if (elementName == CU::ToLower("UpgradesInQueue"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myUpgradesInQueue);
		}
		else 
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadCollisionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, CollisionComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement, "Diameter");
	if (e != nullptr)
	{
		aDocument.ForceReadAttribute(e, "value", aOutputData.myDiameter);
	}
	else
	{
		tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement, "Radius");
		aDocument.ForceReadAttribute(e, "value", aOutputData.myDiameter);
		aOutputData.myDiameter *= 2.f;
	}
}

void ComponentLoader::LoadControllerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ControllerComponentData& aOutputData)
{
	aDocument;
	aSourceElement;
	aOutputData.myExistsInEntity = true;
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
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	tinyxml2::XMLElement* e = aDocument.ForceFindFirstChild(aSourceElement, "Health");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myHealth);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Armor");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myArmor);
}

void ComponentLoader::LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Radius"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myRadius);
		}
		else if (elementName == "type")
		{
			std::string typeString;
			aDocument.ForceReadAttribute(e, "value", typeString);
			typeString = CU::ToLower(typeString);
			if (typeString == "resource")
			{
				aOutputData.myType = eTriggerType::RESOURCE;
			}
			else if (typeString == "victory")
			{
				aOutputData.myType = eTriggerType::VICTORY;
			}
			else
			{
				DL_ASSERT("Unknown trigger type.");
			}
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadEnrageComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, EnrageComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	tinyxml2::XMLElement* e = aDocument.ForceFindFirstChild(aSourceElement, "Radius");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myRadius);

	e = aDocument.ForceFindFirstChild(aSourceElement, "ActivationUnitsCount");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myActivationUnitCount);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Duration");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myDuration);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Cooldown");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myCooldown);

	e = aDocument.ForceFindFirstChild(aSourceElement, "ArmorModifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myArmorModifier);

	e = aDocument.ForceFindFirstChild(aSourceElement, "HealthModifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myHealthModifier);

	e = aDocument.ForceFindFirstChild(aSourceElement, "AttackDamageModifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackDamageModifier);

	e = aDocument.ForceFindFirstChild(aSourceElement, "AttackRange2Modifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackRange2Modifier);

	e = aDocument.ForceFindFirstChild(aSourceElement, "RechargeTimeModifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myRechargeTimeModifier);

	e = aDocument.ForceFindFirstChild(aSourceElement, "MovementSpeedModifier");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myMovementSpeedModifier);
}

void ComponentLoader::LoadGrenadeComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GrenadeComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	tinyxml2::XMLElement* e = aDocument.ForceFindFirstChild(aSourceElement, "Range");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myRange);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Radius");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myRadius);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Cooldown");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myCooldown);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Damage");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myDamage);

	e = aDocument.ForceFindFirstChild(aSourceElement, "Delay");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myDelay);

	e = aDocument.ForceFindFirstChild(aSourceElement, "DamageAllies");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myAlliesShouldTakeDamage);

}

void ComponentLoader::LoadPromotionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PromotionComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	tinyxml2::XMLElement* e = aDocument.ForceFindFirstChild(aSourceElement, "KillCount");
	aDocument.ForceReadAttribute(e, "value", aOutputData.myKillCount);
}

void ComponentLoader::LoadSelectionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SelectionComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myScale = 1.f;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Hover"))
		{
			aDocument.ForceReadAttribute(e, "selectedPath", aOutputData.mySelectedPath);
			aDocument.ForceReadAttribute(e, "hoveredPath", aOutputData.myHoveredPath);
			aDocument.ForceReadAttribute(e, "shaderPath", aOutputData.myEffectPath);
		}
		else if (elementName == CU::ToLower("Scale"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myScale);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}

}

void ComponentLoader::LoadSoundComponent(XMLReader&, tinyxml2::XMLElement*, SoundComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent)
{
	std::string errorMessage = "The element " + aElement + " in " + aParent +
		" is not supported. Please check so you wrote right else tell a programmer.";
	DL_ASSERT(errorMessage.c_str());
}
