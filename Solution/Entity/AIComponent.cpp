#include "stdafx.h"
#include "AIComponent.h"
#include "BlendedBehavior.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <NetMessageEntityState.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"
#include "ProjectileComponent.h"
#include "EntityFactory.h"
#include <iostream>

AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
	, myBullets(16)
	, myBulletIndex(0)
	, myBehavior(new BlendedBehavior(myEntity, aData))
	, myShootTimer(2.f)
	, myAttackAnimationTimeCurrent(0.f)
{
	for (int i = 0; i < myBullets.GetCapacity(); ++i)
	{
		Entity* bullet = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, nullptr, false, CU::Vector3<float>(0, -10.f, 0));
		bullet->Kill();
		myBullets.Add(bullet);
	}
}

AIComponent::~AIComponent()
{
	myBullets.RemoveAll();
	SAFE_DELETE(myBehavior);
}

void AIComponent::Update(float aDelta)
{
	for each (Entity* bullet in myBullets)
	{
		if (bullet->IsAlive() == true)
		{
			bullet->Update(aDelta);
		}
	}

	Entity* closestPlayer = PollingStation::GetInstance()->FindClosestPlayer(myEntity.GetOrientation().GetPos(), myData.myVisionRange);

	Move(aDelta, closestPlayer);
	myShootTimer -= aDelta;
	if (closestPlayer != nullptr && myShootTimer < 0.f)
	{
		Shoot(closestPlayer);
		myShootTimer = 2.f;
	}

	if (myEntity.GetState() == eEntityState::ATTACK)
	{
		myAttackAnimationTimeCurrent -= aDelta;

		if (myAttackAnimationTimeCurrent <= 0.f)
		{
			myEntity.SetState(eEntityState::IDLE);
			SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
			myAttackAnimationTimeCurrent = 0.f;
		}
	}
}


void AIComponent::Move(float aDelta, Entity* aClosestPlayer)
{
	if (aClosestPlayer != nullptr)
	{
		myBehavior->SetTarget(aClosestPlayer->GetOrientation().GetPos());
	}

	CU::Vector3<float> movement(myBehavior->Update(aDelta));

  	if (CU::Length(movement) < 0.02f)
	{
		if (myEntity.GetState() != eEntityState::IDLE && myEntity.GetState() != eEntityState::ATTACK)
		{
			myEntity.SetState(eEntityState::IDLE);
			SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
		}
	}
	else if (myEntity.GetState() != eEntityState::WALK && myEntity.GetState() != eEntityState::ATTACK)
	{
		myEntity.SetState(eEntityState::WALK);
		SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
	}

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

void AIComponent::Shoot(Entity* aClosestPlayer)
{
	if (myBulletIndex > myBullets.Size()-1)
	{
		myBulletIndex = 0;
	}

	myEntity.SetState(eEntityState::ATTACK);
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
	myAttackAnimationTimeCurrent = myData.myAttackAnimationTime;
	
	myBullets[myBulletIndex]->GetComponent<ProjectileComponent>()->Activate(myEntity.GetOrientation());
	myBulletIndex++;
}
