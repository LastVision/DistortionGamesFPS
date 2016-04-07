#include "stdafx.h"
#include "UpgradeComponent.h"
#include "UpgradeComponentData.h"
#include <PostMaster.h>
#include <EmitterMessage.h>
UpgradeComponent::UpgradeComponent(Entity& aEntity, const UpgradeComponentData& aData)
	: Component(aEntity)
	, myData(aData)
{
	PostMaster::GetInstance()->SendMessage(EmitterMessage("Upgrade", myEntity.GetOrientation().GetPos(), { 0.f, 1.f, 0.f }));
}


UpgradeComponent::~UpgradeComponent()
{
}
