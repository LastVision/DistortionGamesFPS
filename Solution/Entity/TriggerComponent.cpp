#include "stdafx.h"

#include <AudioInterface.h>
#include "CollisionComponent.h"
#include "CommonHelper.h"
#include <EmitterMessage.h>
#include <Intersection.h>
#include "MathHelper.h"
#include "TriggerComponent.h"
#include <TriggerMessage.h>
#include <TutorialMessage.h>
#include "PollingStation.h"
#include <PostMaster.h>
#include <MinimapEventMessage.h>
#include "SoundComponent.h"
#include <InWorldTextMessage.h>
#include <NotificationMessage.h>

#include "../Game/FogOfWarMap.h"

TriggerComponent::TriggerComponent(Entity& aEntity, TriggerComponentData& aData)
	: Component(aEntity)
	, myType(aData.myType)
	, myPlayerUnits(GC::playerUnitCount)
	, myEnemyUnits(GC::enemyUnitCount)
	, myNeutralUnits(32)
	, myRadius(aData.myRadius)
	, myRadiusSquared(aData.myRadius * aData.myRadius)
	, myOwnershipRatio(0)
	, myGainingPointsOwner(eOwnerType::NEUTRAL)
	, myHasSentEventMessage(false)
{
	myOriginalPosition = myEntity.GetOrientation().GetPos();

}

TriggerComponent::~TriggerComponent()
{
	if (myType == eTriggerType::RESOURCE)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_ResourcePoint"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
	}
}

void TriggerComponent::Update(float)
{
	CheckUnitsForRemove(myPlayerUnits);
	CheckUnitsForRemove(myEnemyUnits);
	CheckUnitsForRemove(myNeutralUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER), myPlayerUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::ENEMY), myEnemyUnits);
	CheckUnitsForAdd(PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL), myNeutralUnits);

	if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()))
	{
		float height = CU::Math::Remap(myOwnershipRatio, 0.f, 100.f, -5.f, 0.f);
		myEntity.SetPosition({ myOriginalPosition.x, myOriginalPosition.y + height, myOriginalPosition.z });
		myEntity.SetTemporaryOwner(myGainingPointsOwner);
	}
}

void TriggerComponent::CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const
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

void TriggerComponent::CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits
	, CU::GrowingArray<Entity*>& someUnitsOut) const
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

eOwnerType TriggerComponent::ModifyOwnership(eOwnerType anOwner, float aModifyValue)
{
	if (anOwner == eOwnerType::NEUTRAL)
	{
		myOwnershipRatio -= aModifyValue;
	}
	else if (anOwner == myEntity.GetOwner() || myOwnershipRatio <= 0.f)
	{
		if (anOwner == eOwnerType::ENEMY)
		{
			if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()))
			{
				if (myHasSentEventMessage == false && myGainingPointsOwner == eOwnerType::PLAYER)
				{
					if (myType == eTriggerType::RESOURCE)
					{
						PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eRESOURCE_POINT));

					}
					else if (myType == eTriggerType::VICTORY)
					{
						PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eVICTORY_POINT));
					}
					else
					{
						DL_ASSERT("INVALID TYPE IN TRIGGER");
					}
					myHasSentEventMessage = true;
				}
			}
		}
		myOwnershipRatio += aModifyValue;
		myEntity.SetOwner(anOwner);
	}
	else
	{
		if (FogOfWarMap::GetInstance()->IsVisible(myEntity.GetPosition()) && myHasSentEventMessage == false && myGainingPointsOwner == eOwnerType::PLAYER)
		{
			if (myType == eTriggerType::RESOURCE)
			{
				PostMaster::GetInstance()->SendMessage(NotificationMessage("The enemy is taking over your resource point."));
				PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eRESOURCE_POINT));
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("TakingResourcePoint", 0);
			}
			else if (myType == eTriggerType::VICTORY)
			{
				PostMaster::GetInstance()->SendMessage(NotificationMessage("The enemy is taking over your victory point."));
				PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eVICTORY_POINT));
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("TakingVictoryPoint", 0);
			}
			else
			{
				DL_ASSERT("INVALID TYPE IN TRIGGER");
			}
			myHasSentEventMessage = true;
		}
		myOwnershipRatio -= aModifyValue;
	}

	myOwnershipRatio = CU::Clip(myOwnershipRatio, 0.f, 100.f);

	if (myOwnershipRatio == 0.f)
	{
		if (myGainingPointsOwner == eOwnerType::PLAYER)
		{
			if (myType == eTriggerType::RESOURCE)
			{
				PostMaster::GetInstance()->SendMessage(NotificationMessage("Resource point lost."));
				PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Lost", myEntity.GetPosition()));
				PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eRESOURCE_POINT));

			}
			else if (myType == eTriggerType::VICTORY)
			{
				PostMaster::GetInstance()->SendMessage(NotificationMessage("Victory point lost."));
				PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Lost", myEntity.GetPosition()));
				PostMaster::GetInstance()->SendMessage(MinimapEventMessage(myEntity.GetPosition(), MinimapEventType::eVICTORY_POINT));
			}
		}
		myHasSentEventMessage = false;
		myEntity.SetOwner(eOwnerType::NEUTRAL);
		myGainingPointsOwner = eOwnerType::NEUTRAL;
	}
	else if (myOwnershipRatio == 100.f)
	{

		if (myGainingPointsOwner != anOwner)
		{
			if (anOwner == eOwnerType::PLAYER)
			{
				if (myType == eTriggerType::VICTORY)
				{
					PostMaster::GetInstance()->SendMessage(EmitterMessage("victory_point_capture", myEntity.GetOrientation().GetPos()));
					PostMaster::GetInstance()->SendMessage(TutorialMessage(eTutorialAction::VICTORY_POINT));
					PostMaster::GetInstance()->SendMessage(NotificationMessage("Victory point captured."));
				}
				else if (myType == eTriggerType::RESOURCE)
				{
					PostMaster::GetInstance()->SendMessage(EmitterMessage("resource_point_capture", myEntity.GetOrientation().GetPos()));
					PostMaster::GetInstance()->SendMessage(TutorialMessage(eTutorialAction::RESOURCE_POINT));
					PostMaster::GetInstance()->SendMessage(NotificationMessage("Resource point captured."));
				}
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("victoryGain", 0);
				PostMaster::GetInstance()->SendMessage(InWorldTextMessage("Captured", myEntity.GetPosition()));
			}
			else if (anOwner == eOwnerType::ENEMY)
			{
				if (myType == eTriggerType::VICTORY)
				{
					PostMaster::GetInstance()->SendMessage(EmitterMessage("enemy_victory_point_capture", myEntity.GetOrientation().GetPos()));
				}
				else if (myType == eTriggerType::RESOURCE)
				{
					PostMaster::GetInstance()->SendMessage(EmitterMessage("enemy_resource_point_capture", myEntity.GetOrientation().GetPos()));
				}
			}
		}

		myEntity.SetOwner(anOwner);
		myGainingPointsOwner = anOwner;
		
	}

	return myGainingPointsOwner;
}

void TriggerComponent::StartSound()
{
	if (myType == eTriggerType::RESOURCE)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_ResourcePoint"
			, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
	}
}