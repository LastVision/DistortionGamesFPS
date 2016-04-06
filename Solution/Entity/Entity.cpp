#include "stdafx.h"

#include "AIComponent.h"
#include "AnimationComponent.h"
#include "FirstPersonRenderComponent.h"
#include "GraphicsComponent.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include <NetMessageOnDeath.h>
#include "NetworkComponent.h"
#include "PhysicsComponent.h"
#include "PollingStation.h"
#include "GrenadeComponent.h"
#include <Scene.h>
#include <Instance.h>
#include <EmitterMessage.h>
#include <PostMaster.h>
#include <SharedNetworkManager.h>
#include "ShootingComponent.h"
#include "SpawnpointComponent.h"
#include "TriggerComponent.h"
#include "UpgradeComponent.h"
#include "BulletComponent.h"
#include "SoundComponent.h"

Entity::Entity(unsigned int aGID, const EntityData& aEntityData, Prism::Scene* aScene, bool aClientSide, const CU::Vector3<float>& aStartPosition,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale, const std::string& aSubType)
	: myGID(aGID)
	, myScene(aScene)
	, myEntityData(aEntityData)
	, myEmitterConnection(nullptr)
	, myIsClientSide(aClientSide)
	, mySubType(aSubType)
	, myIsEnemy(false)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}

	myOrientation.SetPos(aStartPosition);

	SetRotation(aRotation);


	//has to be here fro press E-text
	if (aEntityData.myTriggerData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::TRIGGER)] = new TriggerComponent(*this, aEntityData.myTriggerData);
	}


	myRoomType = eObjectRoomType::NOT_USED_ON_SERVER;
	if (myScene != nullptr)
	{
		myRoomType = eObjectRoomType::NONE;
		if (aEntityData.myAnimationData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::ANIMATION)] = new AnimationComponent(*this, aEntityData.myAnimationData, aScene);
			//GetComponent<AnimationComponent>()->SetRotation(aRotation);
			GetComponent<AnimationComponent>()->SetScale(aScale);
			myRoomType = aEntityData.myAnimationData.myRoomType;
		}
		else if (aEntityData.myGraphicsData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::GRAPHICS)] = new GraphicsComponent(*this, aEntityData.myGraphicsData);
			//GetComponent<GraphicsComponent>()->SetRotation(aRotation);
			GetComponent<GraphicsComponent>()->SetScale(aScale);
			myRoomType = aEntityData.myGraphicsData.myRoomType;
		}
	}


	if (aEntityData.myAIComponentData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::AI)] = new AIComponent(*this, aEntityData.myAIComponentData, myOrientation, aEntityData.myProjecileData);
	}

	if (aEntityData.myGrenadeData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::GRENADE)] = new GrenadeComponent(*this, aEntityData.myGrenadeData, aScene);
	}

	if (aEntityData.myNetworkData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::NETWORK)] = new NetworkComponent(*this, myOrientation);
	}

	if (aEntityData.myHealthData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::HEALTH)] = new HealthComponent(*this, aEntityData.myHealthData);
	}

	if (aEntityData.myUpgradeData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::UPGRADE)] = new UpgradeComponent(*this, aEntityData.myUpgradeData);
	}

	if (aEntityData.myShootingData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SHOOTING)] = new ShootingComponent(*this, aScene);
	}

	if (aEntityData.myInputData.myExistsInEntity == true && myIsClientSide == true)
	{
		myComponents[static_cast<int>(eComponentType::INPUT)] = new InputComponent(*this, aEntityData.myInputData);
	}

	if (aEntityData.myFirstPersonRenderData.myExistsInEntity == true && myIsClientSide == true)
	{
		myComponents[static_cast<int>(eComponentType::FIRST_PERSON_RENDER)] = new FirstPersonRenderComponent(*this, aScene);
		if (aEntityData.myShootingData.myExistsInEntity == true)
		{
			GetComponent<ShootingComponent>()->Init(aScene);
		}
	}
	if (aEntityData.myProjecileData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::BULLET)] = new BulletComponent(*this, myOrientation);
	}
	if (aEntityData.mySpawnpointData.myExistsInEntity == true)
	{
		myComponents[static_cast<int>(eComponentType::SPAWNPOINT)] = new SpawnpointComponent(*this, aEntityData.mySpawnpointData);
		myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0.f, 1.1f, 0.f));
	}

	if (aEntityData.mySoundData.myExistsInEntity == true && myIsClientSide == true)
	{
		myComponents[static_cast<int>(eComponentType::SOUND)] = new SoundComponent(*this);
	}

	if (aEntityData.myPhysicsData.myExistsInEntity == true)
	{
		if (aEntityData.myAnimationData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, aEntityData.myAnimationData.myModelPath);
		}
		else if (aEntityData.myGraphicsData.myExistsInEntity == true)
		{
			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, aEntityData.myGraphicsData.myModelPath);
		}
		else
		{
			if (myIsClientSide == true && aEntityData.myTriggerData.myExistsInEntity == true && aEntityData.myTriggerData.myIsClientSide == false)
			{
			}
			else
			{
				DL_ASSERT_EXP(myIsClientSide == false, "Can't create PhysicsComponent on client without graphics.");
			}

			myComponents[static_cast<int>(eComponentType::PHYSICS)] = new PhysicsComponent(*this, aEntityData.myPhysicsData
				, "no path");
		}
	}

	Reset();
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DEATH, this);

};

Entity::~Entity()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DEATH, this);
	//if (myIsInScene == true)
	//{
	//	RemoveFromScene();		
	//}
	if (GetComponent<PhysicsComponent>() != nullptr)
	{
		SET_RUNTIME(false);
		GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		delete myComponents[i];
		myComponents[i] = nullptr;
	}
}

void Entity::Reset()
{
	myAlive = true;
	myState = eEntityState::IDLE;
	if (myIsClientSide == false && mySubType == "playerserver")
	{
		PollingStation::GetInstance()->AddEntity(this);
	}

	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
}

void Entity::Update(float aDeltaTime)
{
	for each (Component* component in myComponents)
	{
		if (component != nullptr)
		{
			component->Update(aDeltaTime);
		}
	}

	if (myEntityData.myPhysicsData.myPhysicsType == ePhysics::DYNAMIC)
	{
		if (myComponents[static_cast<int>(eComponentType::NETWORK)] == nullptr)
		{
			memcpy(&myOrientation.myMatrix[0], GetComponent<PhysicsComponent>()->GetOrientation(), sizeof(float) * 16);
		}
	}
}

void Entity::AddComponent(Component* aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent->GetType())] == nullptr, "Tried to add component several times");
	myComponents[int(aComponent->GetType())] = aComponent;
}

void Entity::RemoveComponent(eComponentType aComponent)
{
	DL_ASSERT_EXP(myComponents[int(aComponent)] != nullptr, "Tried to remove an nonexisting component");
	delete myComponents[int(aComponent)];
	myComponents[int(aComponent)] = nullptr;
}

void Entity::AddToScene()
{
	DL_ASSERT_EXP(myIsInScene == false, "Tried to add Entity to scene twice");
	DL_ASSERT_EXP(myIsClientSide == true, "You can't add Entity to scene on server side.");

	if (GetComponent<GraphicsComponent>() != nullptr && GetComponent<GraphicsComponent>()->GetInstance() != nullptr)
	{
		myScene->AddInstance(GetComponent<GraphicsComponent>()->GetInstance(), myRoomType);
	}
	else if (GetComponent<AnimationComponent>() != nullptr && GetComponent<AnimationComponent>()->GetInstance() != nullptr)
	{
		myScene->AddInstance(GetComponent<AnimationComponent>()->GetInstance(), myRoomType);
		GetComponent<AnimationComponent>()->AddWeaponToScene(myScene);
	}

	myIsInScene = true;
}

void Entity::RemoveFromScene()
{
	DL_ASSERT_EXP(myIsInScene == true, "Tried to remove Entity not in scene");
	DL_ASSERT_EXP(myIsClientSide == true, "You can't remove Entity to scene on server side.");

	if (GetComponent<GraphicsComponent>() != nullptr)
	{
		myScene->RemoveInstance(GetComponent<GraphicsComponent>()->GetInstance());
	}
	else if (GetComponent<AnimationComponent>() != nullptr)
	{
		myScene->RemoveInstance(GetComponent<AnimationComponent>()->GetInstance());
		GetComponent<AnimationComponent>()->RemoveWeaponFromScene(myScene);
	}

	myIsInScene = false;
}

eEntityType Entity::GetType() const
{
	return myEntityData.myType;
}

void Entity::AddEmitter(Prism::ParticleEmitterInstance* anEmitterConnection)
{
	myEmitterConnection = anEmitterConnection;
}

Prism::ParticleEmitterInstance* Entity::GetEmitter()
{
	return myEmitterConnection;
}

void Entity::Kill(bool aRemoveFromPhysics)
{
	myAlive = false;

	if (myIsInScene == true)
	{
		RemoveFromScene();
		myIsInScene = false;
	}

	if (myIsClientSide == false)
	{
		SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnDeath(myGID));
		if (mySubType == "player")
		{
			PollingStation::GetInstance()->RemovePlayer(this);
		}
	}

	if (aRemoveFromPhysics == true && myEntityData.myPhysicsData.myExistsInEntity == true)
	{
		GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
}

bool Entity::GetIsClient()
{
	return myIsClientSide;
}

void Entity::ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myGID)
	{
		Kill();
	}
}