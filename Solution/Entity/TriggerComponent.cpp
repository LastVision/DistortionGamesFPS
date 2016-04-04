#include "stdafx.h"
#include "CollisionNote.h"
#include <GameStateMessage.h>
#include <NetMessageDisplayMarker.h>
#include <NetMessageDisplayRespawn.h>
#include <NetMessageSetActive.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <RespawnMessage.h>
#include "TriggerComponent.h"
#include "TriggerComponentData.h"
#include <SharedNetworkManager.h>
#include <SetActiveMessage.h>

TriggerComponent::TriggerComponent(Entity& anEntity, const TriggerComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myTriggerType(eTriggerType(someData.myTriggerType))
	, myHasTriggered(false)
	, myRespawnValue(-1)
	, myPlayersInside(0)
	, myRespawnTime(0.f)
{
}

TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::Update(float aDelta)
{
	if (myHasTriggered == true && myTriggerType == eTriggerType::RESPAWN)
	{
		myRespawnTime -= aDelta * myPlayersInside;

		if (myLastRespawnValue > int(myRespawnTime))
		{
			myLastRespawnValue = int(myRespawnTime);
			CU::Vector3<float> position;
			memcpy(&position, myEntity.GetComponent<PhysicsComponent>()->GetPosition(), sizeof(float) * 3);
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageDisplayRespawn(position
				, true, myLastRespawnValue, myData.myValue, myEntity.GetGID()));
		}

		if (myRespawnTime <= 0.f)
		{
			PostMaster::GetInstance()->SendMessage<RespawnMessage>(RespawnMessage(myRespawnValue));
			myEntity.GetComponent<PhysicsComponent>()->RemoveFromScene();
			myHasTriggered = false;
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageDisplayRespawn({ 0.f, 0.f, 0.f }
				, false, myData.myValue, myData.myValue, myEntity.GetGID()));
		}
	}
}

void TriggerComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (aNote.myOther->GetSubType() == "playerserver")
	{
		if (aNote.myHasEntered == true)
		{
			++myPlayersInside;
			if (myTriggerType == eTriggerType::LEVEL_CHANGE)
			{
				PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_LEVEL, myData.myValue));
			}
			else if (myTriggerType == eTriggerType::MARKER)
			{
				SharedNetworkManager::GetInstance()->AddMessage(NetMessageDisplayMarker(myData.myPosition, myData.myShowMarker));
			}

			if (myData.myIsOneTime == true)
			{
				//myEntity.GetComponent<PhysicsComponent>()->RemoveFromScene();
				if (myData.myIsClientSide == false && myData.myIsPressable == false)
				{
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageSetActive>(NetMessageSetActive(false, true, myEntity.GetGID()));
				}
				PostMaster::GetInstance()->SendMessage<SetActiveMessage>(SetActiveMessage(myEntity.GetGID(), false));
			}
		}
		else if (myTriggerType == eTriggerType::RESPAWN)
		{
			--myPlayersInside;
		}
	}
}

void TriggerComponent::Activate()
{
	if (myTriggerType == eTriggerType::RESPAWN)
	{
		myRespawnTime = float(myData.myValue);
		myLastRespawnValue = myData.myValue;
		myPlayersInside = 0;
		myHasTriggered = true;
	}
}

int TriggerComponent::GetValue() const
{
	return myData.myValue;
}

bool TriggerComponent::IsClientSide() const
{
	return myData.myIsClientSide;
}

bool TriggerComponent::GetIsActiveFromStart() const
{
	return myData.myIsActiveFromStart;
}

bool TriggerComponent::IsPressable() const
{
	return myData.myIsPressable;
}