#include "stdafx.h"
#include "BulletComponent.h"
#include "PhysicsComponent.h"
#include <NetMessageEnemyShooting.h>
#include <NetMessagePosition.h>
#include <SharedNetworkManager.h>
#include "Entity.h"

BulletComponent::BulletComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myData(nullptr)
{
	myEntity.Kill(false);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_SHOOTING, this);
}


BulletComponent::~BulletComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENEMY_SHOOTING, this);

}

void BulletComponent::Activate(const CU::Matrix44<float>& anOrientation)
{
	myEntity.Reset();
	myLifetimeLeft = myData->myLifetime;
	myOrientation = anOrientation;
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, 1.f) * myOrientation + CU::Vector3<float>(0, 1.f, 0));
	myEntity.GetComponent<PhysicsComponent>()->AddToScene();

}

void BulletComponent::Update(float aDelta)
{
	if (myData != nullptr)
	{
		myLifetimeLeft -= aDelta;
		if (myLifetimeLeft < 0)
		{
			myEntity.Kill(true);
		}
		myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, myData->mySpeed * aDelta) * myOrientation);
		myEntity.GetComponent<PhysicsComponent>()->TeleportToPosition(myEntity.GetOrientation().GetPos());
		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(myEntity.GetOrientation().GetPos(), 0.f, myEntity.GetGID()));
	}
}

int BulletComponent::GetDamage() const
{
	if (myData != nullptr)
	{
		return myData->myDamage;
	}
	return 0;
}

