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
	Move(aDelta);
}


void AIComponent::Move(float aDelta)
{
	myBehavior->SetTarget(PollingStation::GetInstance()->GetPlayers()[0]->GetOrientation().GetPos());
	CU::Vector3<float> movement(myBehavior->Update(aDelta));

	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);

	SetOrientation(CU::GetNormalized(movement));
}

void AIComponent::SetOrientation(const CU::Vector3<float>& aLookInDirection)
{
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	myOrientation.SetPos(pos);

	static CU::Vector3<float> y(0, 1.f, 0);
	CU::Vector3<float> right(CU::GetNormalized(CU::Cross(y, aLookInDirection)));
	CU::Vector3<float> forward(CU::GetNormalized(CU::Cross(right, y)));

	myOrientation.SetForward(forward);
	myOrientation.SetRight(right);
	myOrientation.SetUp(y);

	static CU::Vector3<float> z(0, 0, 1.f);
	float angle = acosf(CU::Dot(z, myOrientation.GetForward()));
	CU::Vector3<float> cross = CU::Cross(z, myOrientation.GetForward());
	if (CU::Dot(y, cross) < 0)
	{
		angle = -angle;
	}

	SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, angle, myEntity.GetGID()));
}