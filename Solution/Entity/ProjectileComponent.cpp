#include "stdafx.h"
#include "Entity.h"
#include "EntityFactory.h"
#include "ProjectileComponent.h"
#include "PhysicsComponent.h"
#include <PostMaster.h>

ProjectileComponent::ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData
	, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aComponentData)
	, myTimeUntilExplode(2.f)
	, myShouldBeUpdated(false)
	, myShooterGID(0)
	, myShouldDeleteExplosion(false)
	, myShouldReallyDeleteExplosion(false)
{
	myExplosion = EntityFactory::CreateEntity((20000), eEntityType::EXPLOSION, aScene, true, CU::Vector3<float>());
	//myExplosion->GetComponent<PhysicsComponent>()->TeleportToPosition({ -10.f, 0.f, 0.f });
	//myExplosion->GetComponent<PhysicsComponent>()->RemoveFromScene();
}

ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Update(float aDelta)
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
		myTimeUntilExplode -= aDelta;
		if (myTimeUntilExplode <= 0.f)
		{
			myEntity.RemoveFromScene();
			myEntity.GetComponent<PhysicsComponent>()->Sleep();
			
			//PostMaster::GetInstance()->SendMessage(ExplosionMessage(myEntity.GetGID(), myEntity.GetOrientation().GetPos()));
			myShouldDeleteExplosion = true;

			myExplosion->GetComponent<PhysicsComponent>()->AddToScene();
			//myExplosion->GetComponent<PhysicsComponent>()->AddToScene();
			myExplosion->GetComponent<PhysicsComponent>()->TeleportToPosition(myEntity.GetOrientation().GetPos());
		}
	}
}

void ProjectileComponent::Activate(unsigned int aShooterGID)
{
	myShooterGID = aShooterGID;
	myTimeUntilExplode = 2.f;
	myShouldBeUpdated = true;
	myShouldReallyDeleteExplosion = false;
	myShouldDeleteExplosion = false;

	if (myEntity.IsInScene() == false)
	{
		myEntity.SetPosition({0.f, 0.f, 0.f});
		myEntity.AddToScene();
		myEntity.GetComponent<PhysicsComponent>()->Wake();
	}
}