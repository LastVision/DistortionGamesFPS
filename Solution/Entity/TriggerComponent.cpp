#include "stdafx.h"
#include "CollisionNote.h"
#include <GameStateMessage.h>
#include <NetMessageDisplayMarker.h>
#include <NetMessageDisplayRespawn.h>
#include <NetMessageSetActive.h>
#include <NetMessageText.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <PrintTextMessage.h>
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
	, myPickupTextRows(0)
{
	if (myData.myPickupText.size() > 0)
	{
		for each (char letter in myData.myPickupText)
		{
			if (letter == '\n')
			{
				myPickupTextRows++;
			}
		}
	}
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
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageDisplayRespawn(myPlayerRespawnPosition
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
	if (aNote.myOther->GetSubType() == "playerserver" || aNote.myOther->GetSubType() == "localplayer")
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
				if (myData.myIsClientSide == false)
				{
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageSetActive>(NetMessageSetActive(false, true, myEntity.GetGID()));
				}
				else if (myData.myIsClientSide == true)
				{
					myEntity.GetComponent<PhysicsComponent>()->RemoveFromScene();
					myEntity.RemoveFromScene();
				}
				PostMaster::GetInstance()->SendMessage<SetActiveMessage>(SetActiveMessage(myEntity.GetGID(), false));
			}

			if (myData.myPickupText.size() > 0)
			{
				CU::Vector4<float> color = UPGRADE_COLOR;

				if (myTriggerType == eTriggerType::HEALTH_PACK)
				{
					color = HEALTH_COLOR;
				}

				if (myData.myIsClientSide == false)
				{
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageText>(NetMessageText(myData.myPickupText
						, myData.myPickupTextTime, color, myPickupTextRows));
				}
				else if (myData.myIsClientSide == true)
				{
					PostMaster::GetInstance()->SendMessage<PrintTextMessage>(PrintTextMessage(myData.myPickupText
						, myData.myPickupTextTime, color, myPickupTextRows));
				}
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