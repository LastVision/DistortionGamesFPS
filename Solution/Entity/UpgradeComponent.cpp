#include "stdafx.h"
#include "UpgradeComponent.h"
#include "UpgradeComponentData.h"
#include <PostMaster.h>
#include <EmitterMessage.h>
UpgradeComponent::UpgradeComponent(Entity& aEntity, const UpgradeComponentData& aData)
	: Component(aEntity)
	, myData(aData)
{


	CU::Vector3<float> pos = myEntity.GetOrientation().GetPos();
	pos.y += 0.25f;

	PostMaster::GetInstance()->SendMessage(EmitterMessage("Upgrade", pos, { 0.f, 1.f, 0.f }, &myEntity));
}


UpgradeComponent::~UpgradeComponent()
{
}
