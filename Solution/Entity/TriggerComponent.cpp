#include "stdafx.h"
#include "TriggerComponent.h"

TriggerComponent::TriggerComponent(Entity& anEntity, const TriggerComponentData& someData)
	: Component(anEntity)
	, myData(someData)
{
}

TriggerComponent::~TriggerComponent()
{
}
