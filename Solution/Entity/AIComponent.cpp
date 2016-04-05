#include "stdafx.h"
#include "AIComponent.h"
#include "BlendedBehavior.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <NetMessageEntityState.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"
#include "BulletComponent.h"
#include "EntityFactory.h"
#include <iostream>
#include <NetMessageEnemyShooting.h>
#include <SharedProjectileManager.h>

AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation, const BulletComponentData& aBulletData)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
	, myBullets(16)
	, myBulletIndex(0)
	, myBehavior(new BlendedBehavior(myEntity, aData))
	, myShootTimer(2.f)
	, myAttackAnimationTimeCurrent(0.f)
	, myTarget(nullptr)
	, myHasRaycasted(false)
	, myHasJustSpawned(true)
	, myBulletData(aBulletData)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};
}

AIComponent::~AIComponent()
{
	myBullets.RemoveAll();
	SAFE_DELETE(myBehavior);
}

void AIComponent::Reset()
{
	myHasJustSpawned = true;
	myShootTimer = 2.f;
	myAttackAnimationTimeCurrent = 0.f;
	myTarget = nullptr;
	myHasRaycasted = false;
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

	if (myEntity.GetState() != eEntityState::DIE)
	{
		myDefendTarget = PollingStation::GetInstance()->GetCurrentDefendTarget(myEntity.GetOrientation().GetPos());
		if (myDefendTarget != nullptr && myTarget == nullptr)
		{
			myTarget = myDefendTarget;
		}
		else if (myDefendTarget == nullptr && myTarget != nullptr && myTarget->GetType() == eEntityType::TRIGGER)
		{
			myTarget = nullptr;
		}


		if (myHasRaycasted == false)
		{
			Entity* closestPlayer = PollingStation::GetInstance()->FindClosestPlayer(myEntity.GetOrientation().GetPos(), myData.myVisionRange);

			if (closestPlayer != nullptr)
			{
				CU::Vector3<float> toPlayer(CU::GetNormalized(closestPlayer->GetOrientation().GetPos() - myOrientation.GetPos()));

				Prism::PhysicsInterface::GetInstance()->RayCast(myOrientation.GetPos() + CU::Vector3<float>(0.f, 1.f, 0.f)
					, toPlayer, myData.myVisionRange, myRaycastHandler, myEntity.GetComponent<PhysicsComponent>());
				myHasRaycasted = true;
			}
		}

		myShootTimer -= aDelta;
		
		Move(aDelta, myTarget);
		if (myTarget != nullptr && myTarget != myDefendTarget)
		{
			if (myShootTimer < 0.f)
			{
				Shoot(myTarget);
				myShootTimer = 2.f;
			}
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
}

void AIComponent::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	aDirection;
	aHitPosition;
	aHitNormal;

	myHasRaycasted = false;

	myTarget = nullptr;
	if (aComponent != nullptr)
	{
		if (aComponent->GetEntity().GetSubType() == "playerserver")
		{
			myTarget = &aComponent->GetEntity();
		}
	}
}

void AIComponent::Move(float aDelta, Entity* aClosestPlayer)
{
	if (myEntity.GetState() != eEntityState::DIE)
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
		movement.y = -0.5f;
		Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);

		SetOrientation(CU::GetNormalized(movement), aDelta);
	}
}

void AIComponent::SetOrientation(const CU::Vector3<float>& aLookInDirection, float aDelta)
{
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	bool hasMoved = CU::Math::DistanceBetweenLessThanOrEqualToEpsilon(pos, myOrientation.GetPos(), 0.1f * aDelta) == false;

	myOrientation.SetPos(pos);

	static CU::Vector3<float> y(0, 1.f, 0);
	CU::Vector3<float> right(CU::GetNormalized(CU::Cross(y, aLookInDirection)));
	CU::Vector3<float> forward(CU::GetNormalized(CU::Cross(right, y)));

	bool hasRotated = forward != myOrientation.GetForward();

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

	if (myHasJustSpawned == true)
	{
		myHasJustSpawned = false;
		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, angle, myEntity.GetGID()));
		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, angle, myEntity.GetGID()));
	}
	else if (hasMoved == true || hasRotated == true)
	{
		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(pos, angle, myEntity.GetGID()));
	}
}

void AIComponent::Shoot(Entity* aClosestPlayer)
{
	aClosestPlayer;

	myEntity.SetState(eEntityState::ATTACK);
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
	Entity* requestedBullet = SharedProjectileManager::GetInstance()->RequestBullet(myBulletData);
	myAttackAnimationTimeCurrent = myData.myAttackAnimationTime;
	requestedBullet->GetComponent<BulletComponent>()->Activate(myEntity.GetOrientation());
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEnemyShooting>(NetMessageEnemyShooting(requestedBullet->GetGID(), myEntity.GetGID()));
}
