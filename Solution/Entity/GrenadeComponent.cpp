#include "stdafx.h"
#include "Entity.h"
#include "EntityFactory.h"
#include "GrenadeComponent.h"
#include <NetMessagePosition.h>
#include <NetMessageExplosion.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <SharedNetworkManager.h>

GrenadeComponent::GrenadeComponent(Entity& aEntity, const GrenadeComponentData& aComponentData
	, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aComponentData)
	, myShouldBeUpdated(false)
	, myShooterGID(0)
	, myShouldDeleteExplosion(false)
	, myShouldReallyDeleteExplosion(false)
	, myTimeUntilExplode(0.f)
{
	myExplosion = EntityFactory::CreateEntity((myEntity.GetGID() + 1), eEntityType::EXPLOSION, aScene, true, CU::Vector3<float>());
	myExplosion->GetComponent<PhysicsComponent>()->RemoveFromScene();
}

GrenadeComponent::~GrenadeComponent()
{
	SAFE_DELETE(myExplosion);
}

void GrenadeComponent::Update(float aDelta)
{
	if (myShouldReallyDeleteExplosion == true)
	{
		myShouldBeUpdated = false;
		myShouldDeleteExplosion = false;
		myExplosion->GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
	if (myShouldDeleteExplosion == true)
	{		
		myShouldReallyDeleteExplosion = true;
		return;
	}
	if (myShouldBeUpdated == true)
	{
		SharedNetworkManager::GetInstance()->AddMessage<NetMessagePosition>(NetMessagePosition(myEntity.GetOrientation().GetPos(), 0.f, myEntity.GetGID()));
		myTimeUntilExplode -= aDelta;
		if (myTimeUntilExplode <= 0.f)
		{
			//myEntity.RemoveFromScene();
			myEntity.GetComponent<PhysicsComponent>()->Sleep();

			myShouldDeleteExplosion = true;

 			myExplosion->GetComponent<PhysicsComponent>()->AddToScene();
			myExplosion->GetComponent<PhysicsComponent>()->TeleportToPosition(myEntity.GetOrientation().GetPos());
			SharedNetworkManager::GetInstance()->AddMessage<NetMessageExplosion>(NetMessageExplosion(myEntity.GetOrientation().GetPos(), myExplosion->GetGID()));
		}
	}
}

void GrenadeComponent::Activate(unsigned int aShooterGID)
{
	myShooterGID = aShooterGID;
	myTimeUntilExplode = myData.myTimeToExplode;
	myShouldBeUpdated = true;
	myShouldReallyDeleteExplosion = false;
	myShouldDeleteExplosion = false;

	if (myEntity.IsInScene() == false)
	{
		//myEntity.AddToScene();
	}
	//myEntity.GetComponent<PhysicsComponent>()->AddToScene();
	myEntity.GetComponent<PhysicsComponent>()->Wake();
}