#include "stdafx.h"
#include "AIComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <SharedNetworkManager.h>

AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
{
}


AIComponent::~AIComponent()
{
}

void AIComponent::Update(float aDelta)
{
	CU::Vector3<float> movement(1.f, 0.f, 0.f);
	movement *= 0.13f;
	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	myOrientation.SetPos(pos);

	SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, 0, myEntity.GetGID()));
}
