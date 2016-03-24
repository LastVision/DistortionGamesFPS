#include "stdafx.h"
#include <Instance.h>
#include <AudioInterface.h>

#include "ClientLevel.h"
#include <ModelLoader.h>
#include "Player.h"
#include <Scene.h>
#include <InputWrapper.h>
#include <InputComponent.h>
#include "NetworkMessageTypes.h"

#include "DebugDrawer.h"

#include <CollisionNote.h>
#include "EntityFactory.h"
#include "Entity.h"
#include <EntityData.h>
#include <FirstPersonRenderComponent.h>
#include "GameEnum.h"
#include <PhysicsComponent.h>
#include <PhysicsInterface.h>

#include <NetMessageConnectReply.h>
#include <NetMessageDisconnect.h>
#include <NetMessageEntityState.h>
#include <NetMessageLevelLoaded.h>
#include <NetMessageOnDeath.h>
#include <NetMessageHealthPack.h>
#include <NetMessageOnJoin.h>
#include <NetMessageExplosion.h>
#include <NetMessageRequestConnect.h>
#include <NetMessageShootGrenade.h>
#include <NetMessageSetActive.h>
#include <NetMessageEnemyShooting.h>

#include "ClientNetworkManager.h"
#include "DeferredRenderer.h"
#include <CubeMapGenerator.h>
#include <PointLight.h>
#include <GrenadeComponent.h>

#include <PhysicsInterface.h>
#include <PostMaster.h>
#include <TriggerComponent.h>
#include <UpgradeComponent.h>
#include <UpgradeNote.h>
#include "EmitterManager.h"
#include <EmitterMessage.h>

#include <NetworkComponent.h>
#include "ClientProjectileManager.h"
#include "ClientUnitManager.h"
#include <NetMessageActivateSpawnpoint.h>
#include "TextEventManager.h"

ClientLevel::ClientLevel()
	: myInstanceOrientations(16)
	, myInstances(16)
	, myPointLights(64)
	, myInitDone(false)
{
	Prism::PhysicsInterface::Create(std::bind(&ClientLevel::CollisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false);

	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENTITY_STATE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::ENEMY_SHOOTING, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::SHOOT_GRENADE, this);
	ClientNetworkManager::GetInstance()->Subscribe(eNetMessageType::EXPLOSION, this);

	ClientProjectileManager::Create();
	ClientUnitManager::Create();
	myScene = new Prism::Scene();
}

ClientLevel::~ClientLevel()
{
#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->ShutdownThread();
#endif

	SAFE_DELETE(myEmitterManager);
	ClientProjectileManager::Destroy();
	ClientUnitManager::Destroy();
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_DEATH, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SET_ACTIVE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENTITY_STATE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ENEMY_SHOOTING, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::SHOOT_GRENADE, this);
	ClientNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::EXPLOSION, this);

	myInstances.DeleteAll();
	myPointLights.DeleteAll();
	SAFE_DELETE(myPlayer);
	SAFE_DELETE(myScene);
	SAFE_DELETE(myDeferredRenderer);
	SAFE_DELETE(myTextManager);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopBackground", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopFirstLayer", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("StopSecondLayer", 0);
}

void ClientLevel::Init(const std::string&)
{
	CreatePlayers();

	Prism::ModelLoader::GetInstance()->Pause();
	myDeferredRenderer = new Prism::DeferredRenderer();

	myEmitterManager = new EmitterManager(*myPlayer->GetComponent<InputComponent>()->GetCamera());
	Prism::ModelLoader::GetInstance()->UnPause();
	CU::Matrix44f orientation;
	myInstanceOrientations.Add(orientation);

	Prism::Audio::AudioInterface::GetInstance()->PostEvent("PlayAll", 0);
	ClientProjectileManager::GetInstance()->CreateBullets(myScene);
	ClientProjectileManager::GetInstance()->CreateGrenades(myScene);
	ClientProjectileManager::GetInstance()->CreateExplosions();

	myTextManager = new TextEventManager(myPlayer->GetComponent<InputComponent>()->GetCamera());

}

void ClientLevel::SetMinMax(const CU::Vector3<float>& aMinPoint, const CU::Vector3<float>& aMaxPoint)
{
	myMinPoint = aMinPoint;
	myMaxPoint = aMaxPoint;
}

void ClientLevel::SetName(const std::string& aName)
{
	myName = aName;
}

void ClientLevel::Update(const float aDeltaTime)
{
	if (myInitDone == false && Prism::PhysicsInterface::GetInstance()->GetInitDone() == true)
	{
		myInitDone = true;
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageLevelLoaded());

		SET_RUNTIME(false);
		myDeferredRenderer->GenerateSHData(myScene, myMinPoint, myMaxPoint, myName);
		RESET_RUNTIME;
	}
	ClientProjectileManager::GetInstance()->Update(aDeltaTime);
	ClientUnitManager::GetInstance()->Update(aDeltaTime);
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_U))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::WALK);
	//}
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_Y))
	//{
	//	myActiveEnemies.GetLast()->SetState(eEntityState::ATTACK);
	//}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_B) == true)
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageActivateSpawnpoint(17));
	}

	SharedLevel::Update(aDeltaTime);
	myPlayer->GetComponent<FirstPersonRenderComponent>()->UpdateCoOpPositions(myPlayers);
	myPlayer->Update(aDeltaTime);
	myEmitterManager->UpdateEmitters(aDeltaTime, CU::Matrix44f());

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_I))
	{
		ClientNetworkManager::GetInstance()->AddMessage(NetMessageDisconnect(ClientNetworkManager::GetInstance()->GetGID()));
	}

	unsigned short ms = ClientNetworkManager::GetInstance()->GetResponsTime();
	float kbs = static_cast<float>(ClientNetworkManager::GetInstance()->GetDataSent());

	DEBUG_PRINT(ms);
	DEBUG_PRINT(kbs);

	//for (int i = 0; i < myPlayers.Size(); ++i)
	//{
	//	CU::Vector3f position = ClientNetworkManager::GetInstance()->GetClients()[i].myPosition;
	//	myPlayers[i]->SetPosition(position);
	//}

	DebugMusic();

	Prism::PhysicsInterface::GetInstance()->EndFrame();

	ClientNetworkManager::GetInstance()->Update(aDeltaTime);

	//myTextManager->Update(aDeltaTime);

}

void ClientLevel::Render()
{
	if (myInitDone == true)
	{
		myDeferredRenderer->Render(myScene);
		Prism::DebugDrawer::GetInstance()->RenderLinesToScreen(*myPlayer->GetComponent<InputComponent>()->GetCamera());
		//myScene->Render();
		//myDeferredRenderer->Render(myScene);
		myEmitterManager->RenderEmitters();
		myPlayer->GetComponent<FirstPersonRenderComponent>()->Render();
		//myPlayer->GetComponent<ShootingComponent>()->Render();

		//myTextManager->Render();
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageOnDeath& aMessage, const sockaddr_in&)
{
	DL_ASSERT_EXP(aMessage.myGID != 0, "Can't kill server (id 0).");

	for (Entity* e : myActiveEnemies)
	{
		if (e->GetGID() == aMessage.myGID)
		{
			e->Kill();
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in&)
{
	bool useEntityMap = true;

	if (aMessage.myGID == myPlayer->GetGID())
	{
		if (aMessage.myShouldActivate == true)
		{
			myPlayer->GetComponent<PhysicsComponent>()->Wake();
		}
		else
		{
			myPlayer->GetComponent<PhysicsComponent>()->Sleep();
		}
		return;
	}

	if (myActiveEntitiesMap.find(aMessage.myGID) == myActiveEntitiesMap.end())
	{
		useEntityMap = false;
	}
	if (useEntityMap == false && myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		return;
	}

	if (aMessage.myShouldActivate == true)
	{
		if (useEntityMap == true)
		{
			myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->AddToScene();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->AddToScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Wake();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->AddToScene();
			}
		}
	}
	else
	{
		if (useEntityMap == true)
		{
			if (myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>() != nullptr)
			{
				myActiveEntitiesMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->RemoveFromScene();
			}
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveEntitiesMap[aMessage.myGID]->RemoveFromScene();
			}
		}
		else
		{
			myActiveUnitsMap[aMessage.myGID]->GetComponent<PhysicsComponent>()->Sleep();
			if (aMessage.myIsInGraphicsScene == true)
			{
				myActiveUnitsMap[aMessage.myGID]->RemoveFromScene();
			}
		}
	}
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageEntityState& aMessage, const sockaddr_in&)
{
	if (aMessage.myGID == myPlayer->GetGID())
	{
		if (static_cast<eEntityState>(aMessage.myEntityState) == eEntityState::DIE)
		{
			myPlayer->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		}
		else if (myPlayer->GetState() == eEntityState::DIE)
		{
			myPlayer->SetState(static_cast<eEntityState>(aMessage.myEntityState));
		}
		return;
	}

	if (myActiveUnitsMap.find(aMessage.myGID) == myActiveUnitsMap.end())
	{
		return;
	}
	myActiveUnitsMap[aMessage.myGID]->SetState(static_cast<eEntityState>(aMessage.myEntityState));
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageEnemyShooting& aMessage, const sockaddr_in&)
{
	Entity* requestedBullet = ClientProjectileManager::GetInstance()->RequestBullet(aMessage.myGID);
	requestedBullet->AddToScene();
	ClientProjectileManager::GetInstance()->ActivateBullet(requestedBullet);
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageShootGrenade&, const sockaddr_in&)
{
	Entity* bullet = ClientProjectileManager::GetInstance()->RequestGrenade();
	CU::Matrix44<float> playerOrientation = myPlayer->GetOrientation();
	bullet->AddToScene();
	bullet->GetComponent<PhysicsComponent>()->AddToScene();
	bullet->Reset();
	CU::Vector3<float> pos = playerOrientation.GetPos();
	pos.y += 1.5f;
	//bullet->GetComponent<PhysicsComponent>()->TeleportToPosition(pos);
	//bullet->GetComponent<GrenadeComponent>()->Activate(aMessage.mySenderID);
	//bullet->GetComponent<PhysicsComponent>()->AddForce(playerOrientation.GetForward(), float(aMessage.myForceStrength));
}

void ClientLevel::ReceiveNetworkMessage(const NetMessageExplosion& aMessage, const sockaddr_in&)
{
	ClientProjectileManager::GetInstance()->RequestExplosion(aMessage.myPosition, aMessage.myGID);
	ClientProjectileManager::GetInstance()->KillGrenade(aMessage.myGID - 1);
}

void ClientLevel::AddLight(Prism::PointLight* aLight)
{
	myPointLights.Add(aLight);
	myScene->AddLight(aLight);
}

void ClientLevel::CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered)
{
	Entity& first = aFirst->GetEntity();
	Entity& second = aSecond->GetEntity();

	switch (first.GetType())
	{
	case eEntityType::TRIGGER:
		HandleTrigger(first, second, aHasEntered);
		break;
	case eEntityType::EXPLOSION:
		myTextManager->AddNotification("explosion");
		if (aHasEntered == true)
		{
			HandleExplosion(first, second);
		}
		break;
	}
}

void ClientLevel::DebugMusic()
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_4) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_5) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutBackground", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_6) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_7) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutFirstLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_8) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
	}
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_9) == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeOutSecondLayer", 0);
	}
}

void ClientLevel::HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered)
{
	if (aSecondEntity.GetType() == eEntityType::PLAYER && aHasEntered == true)
	{
		TriggerComponent* firstTrigger = aFirstEntity.GetComponent<TriggerComponent>();
		if (firstTrigger->GetTriggerType() == eTriggerType::UPGRADE)
		{
			myTextManager->AddNotification("upgrade");
			if (aSecondEntity.GetType() == eEntityType::PLAYER)
			{
				aSecondEntity.SendNote<UpgradeNote>(aFirstEntity.GetComponent<UpgradeComponent>()->GetData());
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInFirstLayer", 0);
			}
		}
		else if (firstTrigger->GetTriggerType() == eTriggerType::HEALTH_PACK)
		{
			myTextManager->AddNotification("healthpack");
			ClientNetworkManager::GetInstance()->AddMessage<NetMessageHealthPack>(NetMessageHealthPack(firstTrigger->GetValue()));
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("FadeInSecondLayer", 0);
		}
		aSecondEntity.SendNote<CollisionNote>(CollisionNote(&aFirstEntity, aHasEntered));
	}

	aFirstEntity.SendNote<CollisionNote>(CollisionNote(&aSecondEntity, aHasEntered));
}

void ClientLevel::CreatePlayers()
{
	myPlayer = EntityFactory::GetInstance()->CreateEntity(ClientNetworkManager::GetInstance()->GetGID(), eEntityType::UNIT, "localplayer", myScene, true, CU::Vector3<float>(0.f, 0.f, 0.f));

	myScene->SetCamera(*myPlayer->GetComponent<InputComponent>()->GetCamera());

	for each (const OtherClients& client in ClientNetworkManager::GetInstance()->GetClients())
	{
		Entity* newPlayer = EntityFactory::CreateEntity(client.myID, eEntityType::UNIT, "player", myScene, true, { 0.f, 0.f, 0.f });
		newPlayer->GetComponent<NetworkComponent>()->SetPlayer(true);
		newPlayer->AddToScene();
		newPlayer->Reset();
		myPlayers.Add(newPlayer);
		myActiveUnitsMap[newPlayer->GetGID()] = newPlayer;
	}
}