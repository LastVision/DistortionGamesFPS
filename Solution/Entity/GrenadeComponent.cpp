#include "stdafx.h"

#include <AudioInterface.h>
#include "CollisionComponent.h"
#include "EmitterMessage.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "../Game/FogOfWarMap.h"
#include "HealthComponent.h"
#include "Intersection.h"
#include "KillUnitMessage.h"
#include "Postmaster.h"
#include "PollingStation.h"
#include "TriggerMessage.h"
#include "SoundComponent.h"

GrenadeComponent::GrenadeComponent(Entity& anEntity, GrenadeComponentData& aGrenadeData)
	: Component(anEntity)
	, myOriginalDelay(aGrenadeData.myDelay)
	, myOriginalCooldown(aGrenadeData.myCooldown)
	, myRange2(aGrenadeData.myRange * aGrenadeData.myRange)
	, myRadius(aGrenadeData.myRadius)
	, myDamage(aGrenadeData.myDamage)
	, myDelay(0)
	, myUnits(GC::playerUnitCount)
	, myHasExploded(true)
	, myCurrentCooldown(0.f)
{
	myPosition = { myEntity.GetPosition().x, 1.f, myEntity.GetPosition().y };
}


GrenadeComponent::~GrenadeComponent()
{
}

void GrenadeComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
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

void GrenadeComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
	, CU::GrowingArray<Entity*>& someUnitsOut) const
{
	for (int i = 0; i < someUnits.Size(); ++i)
	{
		Entity* current = someUnits[i];
		if (CU::Intersection::CircleVsCircle({ myPosition.x, myPosition.z }, myRadius
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

void GrenadeComponent::Update(float aDeltaTime)
{
	CheckUnitsForRemove(myUnits);

	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myUnits);
	

	myCurrentCooldown -= aDeltaTime;
	myDelay -= aDeltaTime;

	if (myHasExploded == false)
	{
		if (myDelay <= 0.f)
		{
			Explosion();
		}
	}

}

void GrenadeComponent::ThrowGrenade(const CU::Vector3f& aTargetPosition)
{
	myDelay = myOriginalDelay;
	myPosition = aTargetPosition;
	myCurrentCooldown = myOriginalCooldown;
	myHasExploded = false;
}

const float& GrenadeComponent::GetCooldown() const
{
	return myCurrentCooldown;
}

const float& GrenadeComponent::GetMaxCooldown() const
{
	return myOriginalCooldown;
}

void GrenadeComponent::Explosion()
{
	CU::Vector2<float> my2DPosition = { myPosition.x, myPosition.z };
	PostMaster::GetInstance()->SendMessage(EmitterMessage("Grenade", myPosition));
	if (myEntity.GetUnitType() == eUnitType::GRUNT)
	{
		if (FogOfWarMap::GetInstance()->IsVisible(my2DPosition) == true)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_GrenadeExplosion"
				, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
		}
	}
	bool hasSurvived = true;
	if (myUnits.Size() > 0)
	{
		for (int i = 0; i < myUnits.Size(); ++i)
		{
			//			float length2 = CU::Length2(myUnits[i]->GetPosition() - my2DPosition);
			if (myUnits[i]->GetOwner() != myEntity.GetOwner())
			{
				hasSurvived = myUnits[i]->GetComponent<HealthComponent>()->TakeDamageAndCheckSurvive(myDamage);
			}
			if (hasSurvived == false)
			{
				PostMaster::GetInstance()->SendMessage(KillUnitMessage(static_cast<int>(myUnits[i]->GetUnitType()),
					static_cast<int>(myUnits[i]->GetOwner()), myUnits[i]->GetComponent<ControllerComponent>()->GetTargetPosition(), 
					static_cast<int>(myEntity.GetOwner())));
			}
		}
	}
	myHasExploded = true;
}
