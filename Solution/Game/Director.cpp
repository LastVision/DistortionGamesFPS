#include "stdafx.h"

#include <ActorComponent.h>
#include <ArtifactMessage.h>
#include <AudioInterface.h>
#include <BuildingComponent.h>
#include "Console.h"
#include "Director.h"
#include <Entity.h>
#include <EmitterMessage.h>
#include "FogOfWarMap.h"
#include <HealthComponent.h>
#include <InWorldTextMessage.h>
#include <KillUnitMessage.h>
#include <KilledPromotedMessage.h>
#include <NotificationMessage.h>
#include <PostMaster.h>
#include <ResourceMessage.h>
#include <VictoryMessage.h>
#include <SpawnUnitMessage.h>
#include <PollingStation.h>
#include <PromotionComponent.h>
#include <SelectionComponent.h>
#include <UpgradeUnitMessage.h>
#include <XMLReader.h>

Director::Director(eOwnerType aOwnerType, const Prism::Terrain& aTerrain)
	: myOwner(aOwnerType)
	, myTerrain(aTerrain)
	, myDeadUnits(32)
	, myTimeMultiplier(1.f)
	, myVictoryPoints(0)
	, myGunpowder(0)
	, myArtifacts(0)
	, myUnitCap(0)
	, myUnitCount(0)
	, myHasUnlockedRanger(false)
	, myHasUnlockedTank(false)
	, myHasUnlockedResource(false)
	, myIncomeTimer(1.f)
	, myCurrentIncomeTimer(1.f)
	, myBaseIncome(0)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_game.xml");
	tinyxml2::XMLElement* rootElement = reader.FindFirstChild("root");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(rootElement, "unitCap"), "value", myUnitCap);
	reader.CloseDocument();

	myUnits.Init(myUnitCap);
	myActiveUnits.Init(myUnitCap);

	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::VICTORY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::UPGRADE_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::KILL_UNIT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::KILLED_PROMOTION, this);
}

Director::~Director()
{
	myUnits.DeleteAll();
	SAFE_DELETE(myBuilding);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::RESOURCE, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::VICTORY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::UPGRADE_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::KILL_UNIT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::KILLED_PROMOTION, this);
}

void Director::Update(float aDeltaTime)
{
	ResourceGainUnlockCheck();
	for (int i = 0; i < myDeadUnits.Size(); ++i)
	{
		myDeadUnits[i]->Update(aDeltaTime);
	}

	for (int i = myActiveUnits.Size()-1; i >= 0; --i)
	{
		myActiveUnits[i]->Update(aDeltaTime);

		if (myActiveUnits[i]->GetAlive() == false)
		{
			myDeadUnits.Add(myActiveUnits[i]);
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}
	if (myHasUnlockedResource == true)
	{
		myCurrentIncomeTimer -= aDeltaTime;
		if (myCurrentIncomeTimer <= 0.f)
		{
			myCurrentIncomeTimer = myIncomeTimer;
			myGunpowder += myBaseIncome;
		}
	}

	if (myBuilding != nullptr)
	{
		myBuilding->Update(aDeltaTime);
	}
	DEBUG_PRINT(myArtifacts);
}

void Director::RenderHealthBars(const Prism::Camera& aCamera)
{
	for (int i = 0; i < myActiveUnits.Size(); i++)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myActiveUnits[i]->GetPosition()))
		{
			if (myActiveUnits[i]->GetAlive() == true)
			{
				myActiveUnits[i]->GetComponent<HealthComponent>()->RenderHealthBar(aCamera);
			}
			if (myActiveUnits[i]->GetAlive() == true)
			{
				myActiveUnits[i]->GetComponent<PromotionComponent>()->RenderPromotion(aCamera);
			}
		}
		
	}
}

void Director::CleanUp()
{
	for (int i = myDeadUnits.Size() - 1; i >= 0; --i)
	{
		if (myDeadUnits[i]->GetShouldBeRemoved() == true)
		{
			myDeadUnits[i]->RemoveFromScene();
			myDeadUnits.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myActiveUnits.Size() - 1; i >= 0; --i)
	{
		if (myActiveUnits[i]->GetShouldBeRemoved() == true)
		{
			myActiveUnits[i]->RemoveFromScene();
			myActiveUnits.RemoveCyclicAtIndex(i);
		}
	}
}

bool Director::SpawnUnit(eUnitType aUnitType)
{
	if (aUnitType == eUnitType::RANGER && myHasUnlockedRanger == false) return false;
	if (aUnitType == eUnitType::TANK && myHasUnlockedTank == false) return false;
	if (myOwner == eOwnerType::PLAYER && myBuilding->GetComponent<BuildingComponent>()->IsQueueFull() == true)
	{
		PostMaster::GetInstance()->SendMessage(NotificationMessage("Queue is full."));
		return false;
	}

	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();
	if (myOwner == eOwnerType::PLAYER && myUnitCount + building->GetTotalQueueSupplyCost() + building->GetUnitSupplyCost(aUnitType) > myUnitCap)
	{
		PostMaster::GetInstance()->SendMessage(NotificationMessage("Not enough supply."));
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Supply", 0);
		return false;
	}

	if (myGunpowder >= building->GetUnitCost(aUnitType))
	{		
		myGunpowder -= building->GetUnitCost(aUnitType);
		building->BuildUnit(aUnitType);
		return true;
	}
	else
	{
		if (myOwner == eOwnerType::PLAYER)
		{
			PostMaster::GetInstance()->SendMessage(NotificationMessage("Not enough gun powder."));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Fuel", 0);
		}
	}
	return false;
}

bool Director::UpgradeUnit(eUnitType aUnitType)
{
	BuildingComponent* building = myBuilding->GetComponent<BuildingComponent>();
	//if (building->IsQueueFull() == true)
	//{
	//	return false;
	//}

	if (myOwner == eOwnerType::PLAYER && building->CanUpgrade(aUnitType) == false)
	{
		PostMaster::GetInstance()->SendMessage(NotificationMessage("Upgrade on cooldown."));
		return false;
	}

	if (myArtifacts >= building->GetUpgradeCost(aUnitType))
	{
		myArtifacts -= building->GetUpgradeCost(aUnitType);
		building->UpgradeUnit(aUnitType);
		return true;
	}
	else if (myOwner == eOwnerType::PLAYER)
	{
		PostMaster::GetInstance()->SendMessage(NotificationMessage("Not enough artifacts."));
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Not_Enough_Artifacts", 0);
	}

	return false;
}

void Director::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (static_cast<eOwnerType>(aMessage.myOwnerType) != myOwner) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER && myHasUnlockedRanger == false) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK && myHasUnlockedTank == false) return;

	int unitSupplyCost = myBuilding->GetComponent<BuildingComponent>()->GetUnitSupplyCost(static_cast<eUnitType>(aMessage.myUnitType));

	if (myUnitCount + unitSupplyCost <= myUnitCap)
	{
		Entity* spawnedUnit = nullptr;

		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == static_cast<eUnitType>(aMessage.myUnitType) && myUnits[i]->GetAlive() == false)
			{
				//myUnits[i]->SetState(eEntityState::IDLE);
				if (IsAlreadyActive(myUnits[i]) == true)
				{
					continue;
				}
				if (myDeadUnits.Find(myUnits[i]) > -1)
				{
					spawnedUnit = myUnits[i]; // is allowed to spawn dying unit, but not preferred
					continue;
				}
				
				spawnedUnit = myUnits[i];
				break;
			}
		}

		if (spawnedUnit != nullptr)
		{
			myUnitCount += unitSupplyCost;
			spawnedUnit->Spawn(aMessage.mySpawnPoint, aMessage.myRallyPoint);
			myActiveUnits.Add(spawnedUnit);

			int deadIndex = myDeadUnits.Find(spawnedUnit);
			if (deadIndex > -1)
			{
				myDeadUnits.RemoveCyclicAtIndex(deadIndex);
			}
			PollingStation::GetInstance()->RegisterEntity(spawnedUnit);
		}
		else
		{
			std::string msg;
			switch (myOwner)
			{
			case eOwnerType::PLAYER:
				msg = "PLAYER tried to spawn unit when at UnitCap";
				break;
			case eOwnerType::ENEMY:
				msg = "ENEMY tried to spawn unit when at UnitCap";
				break;
			case eOwnerType::NEUTRAL:
				msg = "NEUTRAL tried to spawn unit when at UnitCap";
				break;
			default:
				break;
			}

			Console::GetInstance()->AddHistory(msg, eHistoryType::WARNING);
		}
	}
}

void Director::ReceiveMessage(const ResourceMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myGunpowder += aMessage.myResourceModifier;
		if (myGunpowder < 0)
		{
			myGunpowder = 0;
		}
	}
}

void Director::ReceiveMessage(const VictoryMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myVictoryPoints += aMessage.myVictoryModifier;
		if (myVictoryPoints < 0)
		{
			myVictoryPoints = 0;
		}
	}
}

void Director::ReceiveMessage(const UpgradeUnitMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		const Upgrade& upgrade = aMessage.myUpgrade;
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			if (myUnits[i]->GetUnitType() == aMessage.myUnit)
			{
				HealthComponent* comp = myUnits[i]->GetComponent<HealthComponent>();
				if (comp != nullptr)
				{
					comp->SetArmor(comp->GetArmor() + upgrade.myArmorModifier);
				}

				ActorComponent* actor = myUnits[i]->GetComponent<ActorComponent>();
				if (actor != nullptr)
				{
					actor->SetAttackDamage(actor->GetAttackDamage() + upgrade.myAttackModifier);
					actor->SetRechargeTime(actor->GetAttackSpeed() + upgrade.myAttackSpeedModifier);
				}

				if (myOwner == eOwnerType::PLAYER)
				{
					if (myUnits[i]->GetAlive() == true)
					{
						PostMaster::GetInstance()->SendMessage(EmitterMessage("on_unit_upgrade", myUnits[i]->GetOrientation().GetPos()));
					}
				}

			}
		}

		if (myOwner == eOwnerType::PLAYER)
		{
			std::string message = "";
			std::string soundEvent;
			switch (aMessage.myUnit)
			{
			case eUnitType::GRUNT:
				message = "Grunt";
				break;
			case eUnitType::RANGER:
				message = "Ranger";
				break;
			case eUnitType::TANK:
				message = "Tank";
				break;
			}
			soundEvent = message + "_Upgrade";
			message.append(" upgraded to level " + std::to_string(aMessage.myUpgradeLevel + 1) + ".");
			PostMaster::GetInstance()->SendMessage(NotificationMessage(message));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent(soundEvent.c_str(), 0);
		}
	}
}

void Director::ReceiveMessage(const KillUnitMessage& aMessage)
{
	if (static_cast<eOwnerType>(aMessage.myOwnerType) != myOwner)
	{
		if (aMessage.myAttackerOwnerType == myOwner)
		{
			if (myHasUnlockedRanger == false && static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER)
			{
				myHasUnlockedRanger = true;
				if (myOwner == eOwnerType::PLAYER)
				{
					PostMaster::GetInstance()->SendMessage(NotificationMessage("You have unlocked ranger."));
				}
			}
			else if (myHasUnlockedTank == false && static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK)
			{
				myHasUnlockedTank = true;
				if (myOwner == eOwnerType::PLAYER)
				{
					PostMaster::GetInstance()->SendMessage(NotificationMessage("You have unlocked tank."));
				}
			}
		}
	}
	else if (myOwner != eOwnerType::NEUTRAL)
	{
		myUnitCount -= myBuilding->GetComponent<BuildingComponent>()->GetUnitSupplyCost(static_cast<eUnitType>(aMessage.myUnitType));

		DL_ASSERT_EXP(myUnitCount >= 0, "[Director] Current unit count is negative");
		if (myUnitCount < 0)
		{
			myUnitCount = 0;
		}

	}
}

void Director::ReceiveMessage(const ArtifactMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myArtifacts += aMessage.myArtifactModifier;
	}
}

void Director::ReceiveMessage(const KilledPromotedMessage& aMessage)
{
	if (aMessage.myOwner == myOwner)
	{
		myGunpowder += aMessage.myGunpowderModifier;

		if (aMessage.myOwner == eOwnerType::PLAYER)
		{
			std::string message = "+" + std::to_string(aMessage.myGunpowderModifier) + "gp";

			PostMaster::GetInstance()->SendMessage(InWorldTextMessage(message, aMessage.myUnitPosition));
		}
	}
}

bool Director::IsAlreadyActive(Entity* aUnit)
{
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i] == aUnit)
		{
			return true;
		}
	}
	return false;
}

void Director::ResourceGainUnlockCheck()
{
	if (myHasUnlockedResource == false && myBuilding != nullptr)
	{
		myHasUnlockedResource = myBuilding->GetComponent<BuildingComponent>()->GetHasSpawnedAtLeastOnce();
	}
}

const BuildingComponent& Director::GetBuildingComponent() const
{
	return *myBuilding->GetComponent<BuildingComponent>();
}