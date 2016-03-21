#include "stdafx.h"
#include "AIComponent.h"
#include "BlendedBehavior.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"
#include "ProjectileComponent.h"
#include "EntityFactory.h"
#include <iostream>

AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
	, myBullets(64)
	, myBulletIndex(0)
	, myBehavior(new BlendedBehavior(myEntity, aData))
{
	myShootTime = 1.f;


	//for (int i = 0; i < myBullets.GetCapacity(); ++i)
	//{
	//	Entity* bullet = EntityFactory::CreateEntity((50000 + i), eEntityType::BULLET, nullptr, false, CU::Vector3<float>());
	//	myBullets.Add(bullet);
	//}
}

AIComponent::~AIComponent()
{
	myBullets.RemoveAll();
	SAFE_DELETE(myBehavior);
}

void AIComponent::Update(float aDelta)
{
	Move(aDelta);
	myShootTime -= aDelta;
	//if (myShootTime < 0.f)
	//{
	//	Shoot();
	//	myShootTime = 5.f;
	//}
}


void AIComponent::Move(float aDelta)
{
	Entity* closestPlayer = PollingStation::GetInstance()->FindClosestPlayer(myEntity.GetOrientation().GetPos(), myData.myVisionRange);
	if (closestPlayer != nullptr)
	{
		myBehavior->SetTarget(closestPlayer->GetOrientation().GetPos());
	}

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

void AIComponent::Shoot()
{
	if (myBulletIndex > myBullets.Size()-1)
	{
		myBulletIndex = 0;
	}

	CU::Vector3<float> forward = myOrientation.GetForward();
	CU::Vector3<float> pos = myOrientation.GetPos();
	myBullets[myBulletIndex]->GetComponent<ProjectileComponent>()->SetPosition({ pos.x, 0.5f, pos.z });
	myBullets[myBulletIndex]->GetComponent<ProjectileComponent>()->SetForward(forward);
	myBullets[myBulletIndex]->GetComponent<ProjectileComponent>()->Activate();
	std::cout << "I shot" << std::endl;

	myBulletIndex++;
}
