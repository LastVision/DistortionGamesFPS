#include "stdafx.h"
#include "CollisionNote.h"
#include <GameStateMessage.h>
#include <PostMaster.h>
#include "TriggerComponent.h"
#include "TriggerComponentData.h"

TriggerComponent::TriggerComponent(Entity& anEntity, const TriggerComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myTriggerType(eTriggerType(someData.myTriggerType))
	, myHasTriggered(false)
{
}

TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::Update(float aDelta)
{
	aDelta;
}

void TriggerComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (myTriggerType == eTriggerType::LEVEL_CHANGE)
	{
		PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_LEVEL, myData.myValue));
	}

	if (myData.myIsOneTime == true)
	{
		myEntity.Kill();
	}
}

int TriggerComponent::GetValue() const
{
	return myData.myValue;
}

const std::string& TriggerComponent::GetID() const
{
	return myData.myID;
}