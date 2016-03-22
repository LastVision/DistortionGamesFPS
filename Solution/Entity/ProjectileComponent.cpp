#include "stdafx.h"
#include "ProjectileComponent.h"
#include "PhysicsComponent.h"
#include <NetMessageEnemyShooting.h>
#include <SharedNetworkManager.h>
#include "Entity.h"

ProjectileComponent::ProjectileComponent(Entity& anEntity, const ProjectileComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myData(aData)
{
	myEntity.Kill(false);
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_SHOOTING, this);
}


ProjectileComponent::~ProjectileComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENEMY_SHOOTING, this);

}

void ProjectileComponent::Activate(const CU::Matrix44<float>& anOrientation)
{
	myEntity.Reset();
	myLifetimeLeft = myData.myLifetime;
	myOrientation = anOrientation;
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, 1.f) * myOrientation + CU::Vector3<float>(0, 1.f, 0));
	//myEntity.GetComponent<PhysicsComponent>()->Wake();

}

void ProjectileComponent::Update(float aDelta)
{
	myLifetimeLeft -= aDelta;
	if (myLifetimeLeft < 0)
	{
		myEntity.Kill();
	}
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, myData.mySpeed * aDelta) * myOrientation);
	myEntity.GetComponent<PhysicsComponent>()->TeleportToPosition(myEntity.GetOrientation().GetPos());
}

int ProjectileComponent::GetDamage() const
{
	return myData.myDamage;
}

void ProjectileComponent::ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in& aSenderAddress)
{
	if (myEntity.GetIsClient() == true && aMessage.myGID == myEntity.GetGID())
	{
		myEntity.AddToScene();
	}
}
