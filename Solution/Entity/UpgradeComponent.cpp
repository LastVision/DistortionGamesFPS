#include "stdafx.h"
#include "UpgradeComponent.h"
#include "UpgradeComponentData.h"

UpgradeComponent::UpgradeComponent(Entity& aEntity, const UpgradeComponentData& aData)
	: Component(aEntity)
	, myData(aData)
{
}


UpgradeComponent::~UpgradeComponent()
{
}
