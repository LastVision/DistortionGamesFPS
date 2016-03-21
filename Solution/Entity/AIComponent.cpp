#include "stdafx.h"
#include "AIComponent.h"
#include "BlendedBehavior.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"


AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
	, myBehavior(new BlendedBehavior(myEntity))
{
}


AIComponent::~AIComponent()
{
	SAFE_DELETE(myBehavior);
}

void AIComponent::Update(float aDelta)
{
	myBehavior->SetTarget(PollingStation::GetInstance()->GetPlayers()[0]->GetOrientation().GetPos());
	CU::Vector3<float> movement(myBehavior->Update(aDelta));
	//movement *= 0.13f;
	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	myOrientation.SetPos(pos);

	CU::Vector3<float> direction(CU::GetNormalized(movement));
	CU::Vector3<float> up(0, 1.f, 0);
	CU::Vector3<float> right(CU::GetNormalized(CU::Cross(up, direction)));
	CU::Vector3<float> forward(CU::GetNormalized(CU::Cross(right, up)));

	myOrientation.SetForward(forward);
	myOrientation.SetRight(right);
	myOrientation.SetUp(up);


	Entity* target = PollingStation::GetInstance()->FindClosestEntityToEntity(myEntity);
	float rad = 0.f;
	if (target != nullptr)
	{
		rad = CU::Dot(target->GetOrientation().GetPos(), myEntity.GetOrientation().GetPos());
		rad *= M_PI;
	}

	float angle = acosf(CU::Dot(CU::Vector3<float>(0, 0, 1.f), myOrientation.GetForward()));
	CU::Vector3<float> cross = CU::Cross(CU::Vector3<float>(0, 0, 1.f), myOrientation.GetForward());
	if (CU::Dot(CU::Vector3<float>(0, 1.f, 0), cross) < 0) { // Or > 0
		angle = -angle;
	}

	SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, angle, myEntity.GetGID()));
}
