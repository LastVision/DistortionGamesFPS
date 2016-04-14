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
	, myShootTimer(0.f)
	, myAttackAnimationTimeCurrent(0.f)
	, myDefendTarget(nullptr)
	, myTargetPlayer(nullptr)
	, myHasRaycasted(false)
	, myHasJustSpawned(true)
	, myBulletData(aBulletData)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};
	myEntity.SetIsEnemy(true);
}

AIComponent::~AIComponent()
{
	myBullets.RemoveAll();
	SAFE_DELETE(myBehavior);
}

void AIComponent::Reset()
{
	myHasJustSpawned = true;
	myShootTimer = 0.f;
	myAttackAnimationTimeCurrent = 0.f;
	myDefendTarget = nullptr;
	myTargetPlayer = nullptr;
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

	if (myEntity.GetState() != eEntityState::DIE && myEntity.GetComponent<PhysicsComponent>()->IsInScene() == true)
	{
		if (myTargetPlayer == nullptr)
		{
			myDefendTarget = PollingStation::GetInstance()->GetCurrentDefendTarget(myEntity.GetOrientation().GetPos());
		}


		if (myHasRaycasted == false)
		{
			Entity* closestPlayer = PollingStation::GetInstance()->FindClosestPlayer(myEntity.GetOrientation().GetPos(), myData.myVisionRange);

			if (closestPlayer == nullptr)
			{
				myTargetPlayer = nullptr;
			}

			if (closestPlayer != nullptr)
			{
				CU::Vector3<float> toPlayer(CU::GetNormalized(closestPlayer->GetOrientation().GetPos() - myOrientation.GetPos()));

				Prism::PhysicsInterface::GetInstance()->RayCast(myOrientation.GetPos() + CU::Vector3<float>(0.f, 1.5f, 0.f)
					, toPlayer, myData.myVisionRange, myRaycastHandler, myEntity.GetComponent<PhysicsComponent>());
				myHasRaycasted = true;
			}
		}

		myShootTimer -= aDelta;
		
		if (myTargetPlayer != nullptr)
		{
			myPreviousTargetPosition = myTargetPlayer->GetOrientation().GetPos();

			if (myEntity.GetState() == eEntityState::ATTACK && myAttackAnimationTimeCurrent > 0.f)
			{
				SetOrientation(myTargetPlayer->GetOrientation().GetPos() - myOrientation.GetPos(), aDelta, true);
			}
			else
			{
				Move(aDelta, myTargetPlayer);
			}

			if (myShootTimer < 0.f)
			{
				Shoot(myTargetPlayer);
				myShootTimer = myData.myAttackAnimationTime;
			}
		}
		else
		{
			if (myEntity.GetState() == eEntityState::ATTACK && myAttackAnimationTimeCurrent > 0.f)
			{
				if (myDefendTarget != nullptr)
				{
					SetOrientation(myDefendTarget->GetOrientation().GetPos() - myOrientation.GetPos(), aDelta);
				}
			}
			else
			{
				if (myDefendTarget == nullptr && myPreviousTargetPosition != CU::Vector3<float>())
				{
					SetOrientation(myPreviousTargetPosition - myOrientation.GetPos(), aDelta);
				}
				else
				{
					Move(aDelta, myDefendTarget);
				}
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

	myTargetPlayer = nullptr;
	if (aComponent != nullptr)
	{
		if (aComponent->GetEntity().GetSubType() == "playerserver")
		{
			myTargetPlayer = &aComponent->GetEntity();
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
		else
		{
			myBehavior->SetTarget(myEntity.GetOrientation().GetPos());
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

void AIComponent::SetOrientation(const CU::Vector3<float>& aLookInDirection, float aDelta, bool aOnlyRotation)
{
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	bool hasMoved = CU::Math::DistanceBetweenLessThanOrEqualToEpsilon(pos, myOrientation.GetPos(), 0.1f * aDelta) == false;

	if (aOnlyRotation == false)
	{
		myOrientation.SetPos(pos);
	}
	else
	{
		pos = myOrientation.GetPos();
	}

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
	else// if (hasMoved == true || hasRotated == true)
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
	myAttackAnimationTimeCurrent = 0.4f;
	requestedBullet->GetComponent<BulletComponent>()->Activate(myEntity.GetOrientation());
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEnemyShooting>(NetMessageEnemyShooting(requestedBullet->GetGID(), myEntity.GetGID()));
}
