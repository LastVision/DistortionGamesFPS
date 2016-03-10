#include "stdafx.h"
#include "CollisionNote.h"
#include "TriggerComponent.h"
#include "TriggerComponentData.h"

TriggerComponent::TriggerComponent(Entity& anEntity, const TriggerComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myTriggerType(eTriggerType(someData.myTriggerType))
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


}