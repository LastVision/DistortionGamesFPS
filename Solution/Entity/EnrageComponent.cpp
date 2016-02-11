#include "stdafx.h"
#include "ActorComponent.h"
#include <AudioInterface.h>
#include "ControllerComponent.h"
#include "CollisionComponent.h"
#include "../Game/FogOfWarMap.h"
#include "EnrageComponent.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "InWorldTextMessage.h"
#include "Postmaster.h"
#include "PollingStation.h"
#include "PromotionComponent.h"
#include "SoundComponent.h"
#include <TriggerMessage.h>
#include <EmitterMessage.h>
EnrageComponent::EnrageComponent(Entity& anEntity, EnrageComponentData& aData)
	: Component(anEntity)
	, myCurrentCooldown(0.f)
	, myCurrentDuration(0.f)
	, myIsActive(false)
	, myData(aData)
	, myUnits(8)
	, myRadius(aData.myRadius)
	, myUnitActivationCount(aData.myActivationUnitCount)
	, myOriginalArmor(0.f)
	, myOriginalAttackDamage(0.f)
	, myOriginalRechargeTime(0.f)
	, myOriginalAttackRange2(0.f)
	, myOriginalMovementSpeed(0.f)
{
}

EnrageComponent::~EnrageComponent()
{
}

void EnrageComponent::Update(float aDeltaTime)
{
	if (myEntity.GetComponent<PromotionComponent>()->GetPromoted() == false)
	{
		return;
	}
	myCurrentCooldown -= aDeltaTime;

	CheckUnitsForRemove(myUnits);
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnits);
	}
	else if (myEntity.GetOwner() == eOwnerType::ENEMY)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnits);
	}
	else if (myEntity.GetOwner() == eOwnerType::NEUTRAL)
	{
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnits);
		CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnits);
	}

	if (myUnits.Size() >= myUnitActivationCount && myIsActive == false)
	{
		Activate();
	}

	if (myIsActive == true)
	{
		myCurrentDuration -= aDeltaTime;

		if (myCurrentDuration <= 0.f)
		{
			myIsActive = false;

			HealthComponent* health = myEntity.GetComponent<HealthComponent>();
			ActorComponent* actor = myEntity.GetComponent<ActorComponent>();

			health->SetArmor(myOriginalArmor);
			myEntity.SetMaxSpeed(myOriginalMovementSpeed);
			actor->SetAttackDamage(myOriginalAttackDamage);
			actor->SetAttackRange2(myOriginalAttackRange2);
			actor->SetRechargeTime(myOriginalRechargeTime);

		}
	}
}

void EnrageComponent::Activate()
{
	if (myCurrentCooldown <= 0.f)
	{
		HealthComponent* health = myEntity.GetComponent<HealthComponent>();
		ActorComponent* actor = myEntity.GetComponent<ActorComponent>();
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) == true)
		{
			PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Enrage!", myEntity.GetPosition(), { 1.f, 0.88f, 0.f, 1.f }));
		}

		myIsActive = true;
		myCurrentDuration = myData.myDuration;
		myCurrentCooldown = myData.myCooldown;

		myOriginalArmor = health->GetArmor();
		myOriginalMovementSpeed = myEntity.GetMaxSpeed();
		myOriginalAttackDamage = actor->GetAttackDamage();
		myOriginalAttackRange2 = actor->GetAttackRange2();
		myOriginalRechargeTime = actor->GetAttackSpeed();


		health->TakeDamageAndCheckSurvive(myData.myHealthModifier);
		health->SetArmor(myOriginalArmor + myData.myArmorModifier);
		myEntity.SetMaxSpeed(myOriginalMovementSpeed + myData.myMovementSpeedModifier);

		actor->SetAttackDamage(myOriginalAttackDamage + myData.myAttackDamageModifier);
		actor->SetAttackRange2(myOriginalAttackRange2 + myData.myAttackRange2Modifier);
		actor->SetRechargeTime(myOriginalRechargeTime + myData.myRechargeTimeModifier);

		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Enrage"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());

		PostMaster::GetInstance()->SendMessage(EmitterMessage("enrage", myEntity.GetId()));

	}
}

void EnrageComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const
{
	for (int i = 0; i < someUnits.Size(); ++i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()))
		{
			if (someUnitsOut.Find(current) < 0)
			{
				PostMaster::GetInstance()->SendMessage(
					TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::ENTER));

				someUnitsOut.Add(current);
			}
		}
	}
}

void EnrageComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
{
	for (int i = someUnits.Size() - 1; i >= 0; --i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle(myEntity.GetPosition(), myRadius
			, current->GetPosition(), current->GetComponent<CollisionComponent>()->GetRadius()) == false
			|| (someUnits[i]->GetState() == eEntityState::DIE && someUnits[i]->GetAlive() == false))
		{
			PostMaster::GetInstance()->SendMessage(
				TriggerMessage(&myEntity, current, TriggerMessage::eTriggerType::EXIT));

			someUnits.RemoveCyclicAtIndex(i);
		}
	}
}